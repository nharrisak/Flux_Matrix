#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MidAmp"
#define AIRWINDOWS_DESCRIPTION "A clean amp sim meant to work like a loud Twin or something of that nature."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','i','d' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Gain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 lastSample;
		Float64 storeSample;
		Float64 lastSlew;
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 iirSampleF;
		Float64 iirSampleG;
		Float64 iirSampleH;
		bool flip;
		int count; //amp
		
		double lastCabSample;
		double smoothCabA;
		double smoothCabB; //cab
		
		double lastRef[10];
		int cycle;	//undersampling
		
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		double fixA[fix_total];
		double fixB[fix_total];
		double fixC[fix_total];
		double fixD[fix_total];
		double fixE[fix_total];
		double fixF[fix_total]; //filtering
		uint32_t fpd;
		
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 Odd[257];
		Float64 Even[257];
		double b[90];
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;

	Float64 inputlevel = GetParameter( kParam_One )*3.0;
	Float64 samplerate = GetSampleRate();
	Float64 EQ = (GetParameter( kParam_Two )/ samplerate)*22050.0;
	Float64 basstrim = GetParameter( kParam_Two );
	Float64 outputlevel = GetParameter( kParam_Three );
	double wet = GetParameter( kParam_Four );
	
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= samplerate;
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	Float64 bleed = outputlevel/6.0;
	Float64 BEQ = (bleed / samplerate)*44100.0;
	Float64 resultB;
	int side = (int)(0.0006092985*samplerate);
	if (side > 126) side = 126;
	int down = side + 1;
	inputlevel = (inputlevel + (inputlevel*basstrim))/2.0;
	
	double cutoff = (15000.0+(GetParameter( kParam_Two )*10000.0)) / GetSampleRate();
	if (cutoff > 0.49) cutoff = 0.49; //don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; //or if cutoff's too low
	
	fixF[fix_freq] = fixE[fix_freq] = fixD[fix_freq] = fixC[fix_freq] = fixB[fix_freq] = fixA[fix_freq] = cutoff;
	
    fixA[fix_reso] = 4.46570214;
	fixB[fix_reso] = 1.51387132;
	fixC[fix_reso] = 0.93979296;
	fixD[fix_reso] = 0.70710678;
	fixE[fix_reso] = 0.52972649;
	fixF[fix_reso] = 0.50316379;
	
	double K = tan(M_PI * fixA[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
	fixA[fix_a0] = K * K * norm;
	fixA[fix_a1] = 2.0 * fixA[fix_a0];
	fixA[fix_a2] = fixA[fix_a0];
	fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
	fixB[fix_a0] = K * K * norm;
	fixB[fix_a1] = 2.0 * fixB[fix_a0];
	fixB[fix_a2] = fixB[fix_a0];
	fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / fixC[fix_reso] + K * K);
	fixC[fix_a0] = K * K * norm;
	fixC[fix_a1] = 2.0 * fixC[fix_a0];
	fixC[fix_a2] = fixC[fix_a0];
	fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixC[fix_b2] = (1.0 - K / fixC[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / fixD[fix_reso] + K * K);
	fixD[fix_a0] = K * K * norm;
	fixD[fix_a1] = 2.0 * fixD[fix_a0];
	fixD[fix_a2] = fixD[fix_a0];
	fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixD[fix_b2] = (1.0 - K / fixD[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / fixE[fix_reso] + K * K);
	fixE[fix_a0] = K * K * norm;
	fixE[fix_a1] = 2.0 * fixE[fix_a0];
	fixE[fix_a2] = fixE[fix_a0];
	fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixE[fix_b2] = (1.0 - K / fixE[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / fixF[fix_reso] + K * K);
	fixF[fix_a0] = K * K * norm;
	fixF[fix_a1] = 2.0 * fixF[fix_a0];
	fixF[fix_a2] = fixF[fix_a0];
	fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixF[fix_b2] = (1.0 - K / fixF[fix_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		double outSample = (inputSample * fixA[fix_a0]) + fixA[fix_sL1];
		fixA[fix_sL1] = (inputSample * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
		fixA[fix_sL2] = (inputSample * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		double skew = (inputSample - lastSample);
		lastSample = inputSample;
		//skew will be direction/angle
		double bridgerectifier = fabs(skew);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skew > 0) skew = bridgerectifier;
		else skew = -bridgerectifier;
		//skew is now sined and clamped and then re-amplified again
		skew *= inputSample;
		
		outSample = (inputSample * fixB[fix_a0]) + fixB[fix_sL1];
		fixB[fix_sL1] = (inputSample * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
		fixB[fix_sL2] = (inputSample * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		double offset = (1.0 - EQ) + (fabs(inputSample)*EQ);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		iirSampleA = (iirSampleA * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleA*0.8);
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skew;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633+skew))+(bridgerectifier*(1.57079633+skew));
		else inputSample = (inputSample*(-0.57079633+skew))-(bridgerectifier*(1.57079633+skew));
		//overdrive
		iirSampleC = (iirSampleC * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - (iirSampleC*0.6);
		//highpass
		skew /= 1.57079633;
		//finished first gain stage
		
		outSample = (inputSample * fixC[fix_a0]) + fixC[fix_sL1];
		fixC[fix_sL1] = (inputSample * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sL2];
		fixC[fix_sL2] = (inputSample * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		offset = (1.0 + offset) / 2.0;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633+skew))+(bridgerectifier*(1.57079633+skew));
		else inputSample = (inputSample*(-0.57079633+skew))-(bridgerectifier*(1.57079633+skew));
		//overdrive
		iirSampleG = (iirSampleG * (1 - EQ)) + (inputSample * EQ);
		inputSample = inputSample - (iirSampleG*0.4);
		//highpass. no further basscut, no more highpasses
		iirSampleD = (iirSampleD * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleD;
		skew /= 1.57079633;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * fixD[fix_a0]) + fixD[fix_sL1];
		fixD[fix_sL1] = (inputSample * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sL2];
		fixD[fix_sL2] = (inputSample * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633+skew))+(bridgerectifier*(1.57079633+skew));
		else inputSample = (inputSample*(-0.57079633+skew))-(bridgerectifier*(1.57079633+skew));
		//output stage has less gain, no highpass, straight lowpass
		iirSampleE = (iirSampleE * (1 - EQ)) + (inputSample * EQ);
		inputSample = iirSampleE;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * fixE[fix_a0]) + fixE[fix_sL1];
		fixE[fix_sL1] = (inputSample * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sL2];
		fixE[fix_sL2] = (inputSample * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		iirSampleF = (iirSampleF * (1 - (offset * BEQ))) + (inputSample * (offset * BEQ));
		//extra lowpass for 4*12" speakers
		if (count < 0 || count > 128) {count = 128;}
		if (flip)
		{
			dram->Odd[count+128] = dram->Odd[count] = iirSampleF;
			resultB = (dram->Odd[count+down] + dram->Odd[count+side]);
		}
		else
		{
			dram->Even[count+128] = dram->Even[count] = iirSampleF;
			resultB = (dram->Even[count+down] + dram->Even[count+side]);
		}
		count--;
		
		
		iirSampleB = (iirSampleB * (1 - (offset * BEQ))) + (resultB * (offset * BEQ));
		inputSample += (iirSampleB*bleed);
		//extra lowpass for 4*12" speakers
		
		iirSampleH = (iirSampleH * (1 - (offset * BEQ))) + (inputSample * (offset * BEQ));
		inputSample += iirSampleH;
		
inputSample = sin(inputSample*outputlevel);		
		double randy = ((double(fpd)/UINT32_MAX)*0.047);
		inputSample = ((inputSample*(1-randy))+(storeSample*randy))*outputlevel;
		storeSample = inputSample;
		
		outSample = (inputSample * fixF[fix_a0]) + fixF[fix_sL1];
		fixF[fix_sL1] = (inputSample * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sL2];
		fixF[fix_sL2] = (inputSample * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		flip = !flip;

		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider
		//amp
		
		cycle++;
		if (cycle == cycleEnd) {
			
			//drySample = inputSample;
			double temp = (inputSample + smoothCabA)/3.0;
			smoothCabA = inputSample;
			inputSample = temp;
			
			dram->b[87] = dram->b[86]; dram->b[86] = dram->b[85]; dram->b[85] = dram->b[84]; dram->b[84] = dram->b[83]; dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
			dram->b[79] = dram->b[78]; dram->b[78] = dram->b[77]; dram->b[77] = dram->b[76]; dram->b[76] = dram->b[75]; dram->b[75] = dram->b[74]; dram->b[74] = dram->b[73]; dram->b[73] = dram->b[72]; dram->b[72] = dram->b[71]; 
			dram->b[71] = dram->b[70]; dram->b[70] = dram->b[69]; dram->b[69] = dram->b[68]; dram->b[68] = dram->b[67]; dram->b[67] = dram->b[66]; dram->b[66] = dram->b[65]; dram->b[65] = dram->b[64]; dram->b[64] = dram->b[63]; 
			dram->b[63] = dram->b[62]; dram->b[62] = dram->b[61]; dram->b[61] = dram->b[60]; dram->b[60] = dram->b[59]; dram->b[59] = dram->b[58]; dram->b[58] = dram->b[57]; dram->b[57] = dram->b[56]; dram->b[56] = dram->b[55]; 
			dram->b[55] = dram->b[54]; dram->b[54] = dram->b[53]; dram->b[53] = dram->b[52]; dram->b[52] = dram->b[51]; dram->b[51] = dram->b[50]; dram->b[50] = dram->b[49]; dram->b[49] = dram->b[48]; dram->b[48] = dram->b[47]; 
			dram->b[47] = dram->b[46]; dram->b[46] = dram->b[45]; dram->b[45] = dram->b[44]; dram->b[44] = dram->b[43]; dram->b[43] = dram->b[42]; dram->b[42] = dram->b[41]; dram->b[41] = dram->b[40]; dram->b[40] = dram->b[39]; 
			dram->b[39] = dram->b[38]; dram->b[38] = dram->b[37]; dram->b[37] = dram->b[36]; dram->b[36] = dram->b[35]; dram->b[35] = dram->b[34]; dram->b[34] = dram->b[33]; dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample;
			inputSample += (dram->b[1] * (1.31819680801404560  + (0.00362521700518292*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.37738284126127919  + (0.14134596126256205*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.09957637225311622  + (0.33199581815501555*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.62025358899656258  + (0.37476042042088142*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.12926194402137478  + (0.24702655568406759*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.28927985011367602  - (0.13289168298307708*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.56518146339033448  - (0.11026641793526121*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.59843200696815069  - (0.10139909232154271*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.45219971861789204  - (0.13313355255903159*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.32520490032331351  - (0.29009061730364216*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.29773131872442909  - (0.45307495356996669*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.31738895975218867  - (0.43198591958928922*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.33336150604703757  - (0.24240412850274029*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.32461638442042151  - (0.02779297492397464*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.27812829473787770  + (0.15565718905032455*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.19413454458668097  + (0.32087693535188599*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.12378036344480114  + (0.37736575956794161*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.12550494837257106  + (0.25593811142722300*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.17725736033713696  + (0.07708896413593636*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.22023699647700670  - (0.01600371273599124*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.21987645486953747  + (0.00973336938352798*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.15014276479707978  + (0.11602269600138954*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.05176520203073560  + (0.20383164255692698*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.04276687165294867  + (0.17785002166834518*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.15951546388137597  + (0.06748854885822464*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.30211952144352616  - (0.03440494237025149*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.36462803375134506  - (0.05874284362202409*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.32283960219377539  + (0.01189623197958362*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.19245178663350720  + (0.11088858383712991*fabs(dram->b[29]))));
			inputSample += (dram->b[30] * (0.00681589563349590  - (0.16314250963457660*fabs(dram->b[30]))));
			inputSample += (dram->b[31] * (0.20927798345622584  - (0.16952981620487462*fabs(dram->b[31]))));
			inputSample += (dram->b[32] * (0.25638846543430976  - (0.11462562122281153*fabs(dram->b[32]))));
			inputSample += (dram->b[33] * (0.04584495673888605  + (0.04669671229804190*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.25221561978187662  - (0.19250758741703761*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.35662801992424953  - (0.12244680002787561*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.21498114329314663  + (0.12152120956991189*fabs(dram->b[36]))));
			inputSample += (dram->b[37] * (0.00968605571673376  - (0.30597812512858558*fabs(dram->b[37]))));
			inputSample += (dram->b[38] * (0.18029119870614621  - (0.31569386468576782*fabs(dram->b[38]))));
			inputSample += (dram->b[39] * (0.22744437185251629  - (0.18028438460422197*fabs(dram->b[39]))));
			inputSample += (dram->b[40] * (0.09725687638959078  + (0.05479918522830433*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.17970389267353537  - (0.29222750363124067*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.42371969704763018  - (0.34924957781842314*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.43313266755788055  - (0.11503731970288061*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.22178165627851801  + (0.25002925550036226*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.00410198176852576  + (0.43283281457037676*fabs(dram->b[45]))));
			inputSample += (dram->b[46] * (0.09072426344812032  - (0.35318250460706446*fabs(dram->b[46]))));
			inputSample += (dram->b[47] * (0.08405839183965140  - (0.16936391987143717*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.01110419756114383  - (0.01247164991313877*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.18593334646855278  - (0.14513260199423966*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.33665010871497486  - (0.14456206192169668*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.32644968491439380  + (0.01594380759082303*fabs(dram->b[51]))));
			inputSample -= (dram->b[52] * (0.14855437679485431  + (0.23555511219002742*fabs(dram->b[52]))));
			inputSample += (dram->b[53] * (0.05113019250820622  - (0.35556617126595202*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.12915754942362243  - (0.28571671825750300*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.07406865846069306  - (0.10543886479975995*fabs(dram->b[55]))));
			inputSample -= (dram->b[56] * (0.03669573814193980  - (0.03194267657582078*fabs(dram->b[56]))));
			inputSample -= (dram->b[57] * (0.13429103278009327  - (0.06145796486786051*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.17884021749974641  - (0.00156626902982124*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.16138212225178239  + (0.09402070836837134*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.10867028245257521  + (0.15407963447815898*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.06312416389213464  + (0.11241095544179526*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.05826376574081994  - (0.03635253545701986*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.07991631148258237  - (0.18041947557579863*fabs(dram->b[63]))));
			inputSample -= (dram->b[64] * (0.07777397532506500  - (0.20817156738202205*fabs(dram->b[64]))));
			inputSample -= (dram->b[65] * (0.03812528734394271  - (0.13679963125162486*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.00204900323943951  + (0.04009000730101046*fabs(dram->b[66]))));
			inputSample += (dram->b[67] * (0.01779599498119764  - (0.04218637577942354*fabs(dram->b[67]))));
			inputSample += (dram->b[68] * (0.00950301949319113  - (0.07908911305044238*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.04283600714814891  + (0.02716262334097985*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.14478320837041933  - (0.08823515277628832*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.23250267035795688  - (0.15334197814956568*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.22369031446225857  - (0.08550989980799503*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.11142757883989868  + (0.08321482928259660*fabs(dram->b[73]))));
			inputSample += (dram->b[74] * (0.02752318631713307  - (0.25252906099212968*fabs(dram->b[74]))));
			inputSample += (dram->b[75] * (0.11940028414727490  - (0.34358127205009553*fabs(dram->b[75]))));
			inputSample += (dram->b[76] * (0.12702057126698307  - (0.31808560130583663*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.03639067777025356  - (0.17970282734717846*fabs(dram->b[77]))));
			inputSample -= (dram->b[78] * (0.11389848143835518  + (0.00470616711331971*fabs(dram->b[78]))));
			inputSample -= (dram->b[79] * (0.23024072979374310  - (0.09772245468884058*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.24389015061112601  - (0.09600959885615798*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.16680269075295703  - (0.05194978963662898*fabs(dram->b[81]))));
			inputSample -= (dram->b[82] * (0.05108041495077725  - (0.01796071525570735*fabs(dram->b[82]))));
			inputSample += (dram->b[83] * (0.06489835353859555  - (0.00808013770331126*fabs(dram->b[83]))));
			inputSample += (dram->b[84] * (0.15481511440233464  - (0.02674063848284838*fabs(dram->b[84]))));
			inputSample += (dram->b[85] * (0.18620867857907253  - (0.01786423699465214*fabs(dram->b[85]))));
			inputSample += (dram->b[86] * (0.13879832139055756  + (0.01584446839973597*fabs(dram->b[86]))));
			inputSample += (dram->b[87] * (0.04878235109120615  + (0.02962866516075816*fabs(dram->b[87]))));
			
			temp = (inputSample + smoothCabB)/3.0;
			smoothCabB = inputSample;
			inputSample = temp/4.0;
			
			
			randy = ((double(fpd)/UINT32_MAX)*0.039);
			drySample = ((((inputSample*(1-randy))+(lastCabSample*randy))*wet)+(drySample*(1.0-wet)))*outputlevel;
			lastCabSample = inputSample;
			inputSample = drySample; //cab
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		} //undersampling
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	storeSample = 0.0;
	lastSlew = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	for (int fcount = 0; fcount < 257; fcount++) {dram->Odd[fcount] = 0.0; dram->Even[fcount] = 0.0;}
	count = 0;
	flip = false; //amp
	
	for(int fcount = 0; fcount < 90; fcount++) {dram->b[fcount] = 0;}
	smoothCabA = 0.0; smoothCabB = 0.0; lastCabSample = 0.0; //cab
	
	for (int fcount = 0; fcount < 9; fcount++) {lastRef[fcount] = 0.0;}
	cycle = 0; //undersampling
	
	for (int x = 0; x < fix_total; x++) {
		fixA[x] = 0.0;
		fixB[x] = 0.0;
		fixC[x] = 0.0;
		fixD[x] = 0.0;
		fixE[x] = 0.0;
		fixF[x] = 0.0;
	}	//filtering
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
