#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FireAmp"
#define AIRWINDOWS_DESCRIPTION "A bright, loud, tubey amp sim for leads and dirt guitar."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','i','r' )
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
		Float64 smoothA;
		Float64 smoothB;
		Float64 smoothC;
		Float64 smoothD;
		Float64 smoothE;
		Float64 smoothF;
		Float64 smoothG;
		Float64 smoothH;
		Float64 smoothI;
		Float64 smoothJ;
		Float64 smoothK;
		Float64 smoothL;
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 iirSampleF;
		Float64 iirSampleG;
		Float64 iirSampleH;
		Float64 iirSampleI;
		Float64 iirSampleJ;
		Float64 iirSampleK;
		Float64 iirSampleL;
		Float64 iirLowpass;
		Float64 iirSpkA;
		Float64 iirSpkB;
		Float64 iirSub;
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
	
	double bassfill = GetParameter( kParam_One );
	double outputlevel = GetParameter( kParam_Three );
	double wet = GetParameter( kParam_Four );
	
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	double startlevel = bassfill;
	double samplerate = GetSampleRate();
	double basstrim = bassfill / 16.0;
	double toneEQ = (GetParameter( kParam_Two ) / samplerate)*22050.0;
	double EQ = (basstrim / samplerate)*22050.0;
	double bleed = outputlevel/16.0;
	double bassfactor = 1.0-(basstrim*basstrim);
	double BEQ = (bleed / samplerate)*22050.0;
	int diagonal = (int)(0.000861678*samplerate);
	if (diagonal > 127) diagonal = 127;
	int side = (int)(diagonal/1.4142135623730951);
	int down = (side + diagonal)/2;
	//now we've got down, side and diagonal as offsets and we also use three successive samples upfront
	
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
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		double basscut = 0.98;
		//we're going to be shifting this as the stages progress
		double inputlevel = startlevel;
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleA = (iirSampleA * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleA*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		double bridgerectifier = (smoothA + inputSample);
		smoothA = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleB = (iirSampleB * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleB*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothB + inputSample);
		smoothB = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		
		outSample = (inputSample * fixB[fix_a0]) + fixB[fix_sL1];
		fixB[fix_sL1] = (inputSample * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
		fixB[fix_sL2] = (inputSample * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleC = (iirSampleC * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleC*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothC + inputSample);
		smoothC = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleD = (iirSampleD * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleD*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothD + inputSample);
		smoothD = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * fixC[fix_a0]) + fixC[fix_sL1];
		fixC[fix_sL1] = (inputSample * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sL2];
		fixC[fix_sL2] = (inputSample * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleE = (iirSampleE * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleE*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothE + inputSample);
		smoothE = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleF = (iirSampleF * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleF*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothF + inputSample);
		smoothF = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * fixD[fix_a0]) + fixD[fix_sL1];
		fixD[fix_sL1] = (inputSample * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sL2];
		fixD[fix_sL2] = (inputSample * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleG = (iirSampleG * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleG*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothG + inputSample);
		smoothG = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleH = (iirSampleH * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleH*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothH + inputSample);
		smoothH = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * fixE[fix_a0]) + fixE[fix_sL1];
		fixE[fix_sL1] = (inputSample * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sL2];
		fixE[fix_sL2] = (inputSample * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleI = (iirSampleI * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleI*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothI + inputSample);
		smoothI = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleJ = (iirSampleJ * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleJ*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothJ + inputSample);
		smoothJ = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * fixF[fix_a0]) + fixF[fix_sL1];
		fixF[fix_sL1] = (inputSample * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sL2];
		fixF[fix_sL2] = (inputSample * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleK = (iirSampleK * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleK*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothK + inputSample);
		smoothK = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleL = (iirSampleL * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleL*basscut);
		//highpass
		inputSample -= (inputSample * (fabs(inputSample) * 0.654) * (fabs(inputSample) * 0.654) );
		//overdrive
		bridgerectifier = (smoothL + inputSample);
		smoothL = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		iirLowpass = (iirLowpass * (1 - toneEQ)) + (inputSample * toneEQ);
		inputSample = iirLowpass;
		//lowpass. The only one of this type.
		
		iirSpkA = (iirSpkA * (1 -  BEQ)) + (inputSample * BEQ);
		//extra lowpass for 4*12" speakers
		if (count < 0 || count > 128) {count = 128;}
		double resultB = 0.0;
		if (flip)
		{
			dram->Odd[count+128] = dram->Odd[count] = iirSpkA;
			resultB = (dram->Odd[count+down] + dram->Odd[count+side] + dram->Odd[count+diagonal]);
		}
		else
		{
			dram->Even[count+128] = dram->Even[count] = iirSpkA;
			resultB = (dram->Even[count+down] + dram->Even[count+side] + dram->Even[count+diagonal]);
		}
		count--;
		iirSpkB = (iirSpkB * (1 - BEQ)) + (resultB * BEQ);
		inputSample += (iirSpkB * bleed);
		//extra lowpass for 4*12" speakers
		
		bridgerectifier = fabs(inputSample*outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		
		iirSub = (iirSub * (1 - BEQ)) + (inputSample * BEQ);
		inputSample += (iirSub * bassfill * outputlevel);
		
		double randy = ((rand()/(double)RAND_MAX)*0.053);
		inputSample = ((inputSample*(1-randy))+(storeSample*randy))*outputlevel;
		storeSample = inputSample;
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
			
			dram->b[84] = dram->b[83]; dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.31698250313308396  - (0.08140616497621633*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.47229016949915326  - (0.27680278993637253*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.30410109086044956  - (0.35629113432046489*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.81766210474551260  - (0.26808782337659753*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.19868872545506663  - (0.11105517193919669*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.39115909132567039  - (0.12630622002682679*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.76881891559343574  - (0.40879849500403143*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.87146861782680340  - (0.59529560488000599*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.79504575932563670  - (0.60877047551611796*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.61653017622406314  - (0.47662851438557335*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.40718195794382067  - (0.24955839378539713*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.31794900040616203  - (0.04169792259600613*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.41075032540217843  + (0.00368483996076280*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.56901352922170667  - (0.11027360805893105*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.62443222391889264  - (0.22198075154245228*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.53462856723129204  - (0.22933544545324852*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.34441703361995046  - (0.12956809502269492*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.13947052337867882  + (0.00339775055962799*fabs(dram->b[18]))));
			inputSample += (dram->b[19] * (0.03771252648928484  - (0.10863931549251718*fabs(dram->b[19]))));
			inputSample += (dram->b[20] * (0.18280210770271693  - (0.17413646599296417*fabs(dram->b[20]))));
			inputSample += (dram->b[21] * (0.24621986701761467  - (0.14547053270435095*fabs(dram->b[21]))));
			inputSample += (dram->b[22] * (0.22347075142737360  - (0.02493869490104031*fabs(dram->b[22]))));
			inputSample += (dram->b[23] * (0.14346348482123716  + (0.11284054747963246*fabs(dram->b[23]))));
			inputSample += (dram->b[24] * (0.00834364862916028  + (0.24284684053733926*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.11559740296078347  - (0.32623054435304538*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.18067604561283060  - (0.32311481551122478*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.22927997789035612  - (0.26991539052832925*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.28487666578669446  - (0.22437227250279349*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.31992973037153838  - (0.15289876100963865*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.35174606303520733  - (0.05656293023086628*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.36894898011375254  + (0.04333925421463558*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.32567576055307507  + (0.14594589410921388*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.27440135050585784  + (0.15529667398122521*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.21998973785078091  + (0.05083553737157104*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.10323624876862457  - (0.04651829594199963*fabs(dram->b[35]))));
			inputSample += (dram->b[36] * (0.02091603687851074  + (0.12000046818439322*fabs(dram->b[36]))));
			inputSample += (dram->b[37] * (0.11344930914138468  + (0.17697142512225839*fabs(dram->b[37]))));
			inputSample += (dram->b[38] * (0.22766779627643968  + (0.13645102964003858*fabs(dram->b[38]))));
			inputSample += (dram->b[39] * (0.38378309953638229  - (0.01997653307333791*fabs(dram->b[39]))));
			inputSample += (dram->b[40] * (0.52789400804568076  - (0.21409137428422448*fabs(dram->b[40]))));
			inputSample += (dram->b[41] * (0.55444630296938280  - (0.32331980931576626*fabs(dram->b[41]))));
			inputSample += (dram->b[42] * (0.42333237669264601  - (0.26855847463044280*fabs(dram->b[42]))));
			inputSample += (dram->b[43] * (0.21942831522035078  - (0.12051365248820624*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.00584169427830633  - (0.03706970171280329*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.24279799124660351  - (0.17296440491477982*fabs(dram->b[45]))));
			inputSample -= (dram->b[46] * (0.40173760787507085  - (0.21717989835163351*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.43930035724188155  - (0.16425928481378199*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.41067765934041811  - (0.10390115786636855*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.34409235547165967  - (0.07268159377411920*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.26542883122568151  - (0.05483457497365785*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.22024754776138800  - (0.06484897950087598*fabs(dram->b[51]))));
			inputSample -= (dram->b[52] * (0.20394367993632415  - (0.08746309731952180*fabs(dram->b[52]))));
			inputSample -= (dram->b[53] * (0.17565242431124092  - (0.07611309538078760*fabs(dram->b[53]))));
			inputSample -= (dram->b[54] * (0.10116623231246825  - (0.00642818706295112*fabs(dram->b[54]))));
			inputSample -= (dram->b[55] * (0.00782648272053632  + (0.08004141267685004*fabs(dram->b[55]))));
			inputSample += (dram->b[56] * (0.05059046006747323  - (0.12436676387548490*fabs(dram->b[56]))));
			inputSample += (dram->b[57] * (0.06241531553254467  - (0.11530779547021434*fabs(dram->b[57]))));
			inputSample += (dram->b[58] * (0.04952694587101836  - (0.08340945324333944*fabs(dram->b[58]))));
			inputSample += (dram->b[59] * (0.00843873294401687  - (0.03279659052562903*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.05161338949440241  - (0.03428181149163798*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.08165520146902012  - (0.08196746092283110*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.06639532849935320  - (0.09797462781896329*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.02953430910661621  - (0.09175612938515763*fabs(dram->b[63]))));
			inputSample += (dram->b[64] * (0.00741058547442938  + (0.05442091048731967*fabs(dram->b[64]))));
			inputSample += (dram->b[65] * (0.01832866125391727  + (0.00306243693643687*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.00526964230373573  - (0.04364102661136410*fabs(dram->b[66]))));
			inputSample -= (dram->b[67] * (0.00300984373848200  + (0.09742737841278880*fabs(dram->b[67]))));
			inputSample -= (dram->b[68] * (0.00413616769576694  + (0.14380661694523073*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.00588769034931419  + (0.16012843578892538*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.00688588239450581  + (0.14074464279305798*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.02277307992926315  + (0.07914752191801366*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.04627166091180877  - (0.00192787268067208*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.05562045897455786  - (0.05932868727665747*fabs(dram->b[73]))));
			inputSample -= (dram->b[74] * (0.05134243784922165  - (0.08245334798868090*fabs(dram->b[74]))));
			inputSample -= (dram->b[75] * (0.04719409472239919  - (0.07498680629253825*fabs(dram->b[75]))));
			inputSample -= (dram->b[76] * (0.05889738914266415  - (0.06116127018043697*fabs(dram->b[76]))));
			inputSample -= (dram->b[77] * (0.09428363535111127  - (0.06535868867863834*fabs(dram->b[77]))));
			inputSample -= (dram->b[78] * (0.15181756953225126  - (0.08982979655234427*fabs(dram->b[78]))));
			inputSample -= (dram->b[79] * (0.20878969456036670  - (0.10761070891499538*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.22647885581813790  - (0.08462542510349125*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.19723482443646323  - (0.02665160920736287*fabs(dram->b[81]))));
			inputSample -= (dram->b[82] * (0.16441643451155163  + (0.02314691954338197*fabs(dram->b[82]))));
			inputSample -= (dram->b[83] * (0.15201914054931515  + (0.04424903493886839*fabs(dram->b[83]))));
			inputSample -= (dram->b[84] * (0.15454370641307855  + (0.04223203797913008*fabs(dram->b[84]))));
			
			temp = (inputSample + smoothCabB)/3.0;
			smoothCabB = inputSample;
			inputSample = temp/4.0;
			
			
			randy = ((double(fpd)/UINT32_MAX)*0.057);
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
	smoothA = 0.0;
	smoothB = 0.0;
	smoothC = 0.0;
	smoothD = 0.0;
	smoothE = 0.0;
	smoothF = 0.0;
	smoothG = 0.0;
	smoothH = 0.0;
	smoothI = 0.0;
	smoothJ = 0.0;
	smoothK = 0.0;
	smoothL = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	iirSampleI = 0.0;
	iirSampleJ = 0.0;
	iirSampleK = 0.0;
	iirSampleL = 0.0;
	iirLowpass = 0.0;
	iirSpkA = 0.0;
	iirSpkB = 0.0;
	iirSub = 0.0;
	register int fcount;
	for (fcount = 0; fcount < 257; fcount++) {dram->Odd[fcount] = 0.0; dram->Even[fcount] = 0.0;}
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
