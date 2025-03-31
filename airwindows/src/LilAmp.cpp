#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "LilAmp"
#define AIRWINDOWS_DESCRIPTION "A tiny amp sim, like a little bitty practice amp without that much gain."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','i','l' )
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
 
		Float64 lastSample;
		Float64 storeSample;
		Float64 lastSlew;
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 OddA;
		Float64 OddB;
		Float64 OddC;
		Float64 OddD;
		Float64 OddE;
		Float64 EvenA;
		Float64 EvenB;
		Float64 EvenC;
		Float64 EvenD;
		Float64 EvenE;
		bool flip; //amp
		
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
		uint32_t fpd;
	
	struct _dram {
			double b[90];
		double fixA[fix_total];
		double fixB[fix_total];
		double fixC[fix_total];
		double fixD[fix_total];
		double fixE[fix_total];
		double fixF[fix_total]; //filtering
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;

	double inputlevel = GetParameter( kParam_One )*6.0;
	double EQ = (GetParameter( kParam_Two )/ GetSampleRate())*22050;
	double basstrim = GetParameter( kParam_Two );
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
	
	double skewlevel = pow(basstrim,2) * outputlevel;
	
	double cutoff = (15000.0+(GetParameter( kParam_Two )*10000.0)) / GetSampleRate();
	if (cutoff > 0.49) cutoff = 0.49; //don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; //or if cutoff's too low
	
	dram->fixF[fix_freq] = dram->fixE[fix_freq] = dram->fixD[fix_freq] = dram->fixC[fix_freq] = dram->fixB[fix_freq] = dram->fixA[fix_freq] = cutoff;
	
    dram->fixA[fix_reso] = 4.46570214;
	dram->fixB[fix_reso] = 1.51387132;
	dram->fixC[fix_reso] = 0.93979296;
	dram->fixD[fix_reso] = 0.70710678;
	dram->fixE[fix_reso] = 0.52972649;
	dram->fixF[fix_reso] = 0.50316379;
	
	double K = tan(M_PI * dram->fixA[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / dram->fixA[fix_reso] + K * K);
	dram->fixA[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = 2.0 * dram->fixA[fix_a0];
	dram->fixA[fix_a2] = dram->fixA[fix_a0];
	dram->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixA[fix_b2] = (1.0 - K / dram->fixA[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / dram->fixB[fix_reso] + K * K);
	dram->fixB[fix_a0] = K * K * norm;
	dram->fixB[fix_a1] = 2.0 * dram->fixB[fix_a0];
	dram->fixB[fix_a2] = dram->fixB[fix_a0];
	dram->fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixB[fix_b2] = (1.0 - K / dram->fixB[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / dram->fixC[fix_reso] + K * K);
	dram->fixC[fix_a0] = K * K * norm;
	dram->fixC[fix_a1] = 2.0 * dram->fixC[fix_a0];
	dram->fixC[fix_a2] = dram->fixC[fix_a0];
	dram->fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixC[fix_b2] = (1.0 - K / dram->fixC[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / dram->fixD[fix_reso] + K * K);
	dram->fixD[fix_a0] = K * K * norm;
	dram->fixD[fix_a1] = 2.0 * dram->fixD[fix_a0];
	dram->fixD[fix_a2] = dram->fixD[fix_a0];
	dram->fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixD[fix_b2] = (1.0 - K / dram->fixD[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / dram->fixE[fix_reso] + K * K);
	dram->fixE[fix_a0] = K * K * norm;
	dram->fixE[fix_a1] = 2.0 * dram->fixE[fix_a0];
	dram->fixE[fix_a2] = dram->fixE[fix_a0];
	dram->fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixE[fix_b2] = (1.0 - K / dram->fixE[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / dram->fixF[fix_reso] + K * K);
	dram->fixF[fix_a0] = K * K * norm;
	dram->fixF[fix_a1] = 2.0 * dram->fixF[fix_a0];
	dram->fixF[fix_a2] = dram->fixF[fix_a0];
	dram->fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixF[fix_b2] = (1.0 - K / dram->fixF[fix_reso] + K * K) * norm;
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		double outSample = (inputSample * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSample * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSample * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
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
		skew *= skewlevel;
		inputSample *= basstrim;
		inputSample *= inputlevel;
		double offset = (1.0 - EQ) + (fabs(inputSample)*EQ);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		iirSampleA = (iirSampleA * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - iirSampleA;
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skew;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633+skew))+(bridgerectifier*(1.57079633+skew));
		else inputSample = (inputSample*(-0.57079633+skew))-(bridgerectifier*(1.57079633+skew));
		//overdrive
		iirSampleC = (iirSampleC * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleC;
		//lowpass. Use offset from before gain stage
		//finished first gain stage
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= inputlevel;
		offset = (1.0 + offset) / 2.0;
		iirSampleB = (iirSampleB * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = inputSample - iirSampleB;
		//highpass
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skew;
		if (inputSample > 0) inputSample = (inputSample*(-0.57079633+skew))+(bridgerectifier*(1.57079633+skew));
		else inputSample = (inputSample*(-0.57079633+skew))-(bridgerectifier*(1.57079633+skew));
		//overdrive
		iirSampleD = (iirSampleD * (1 - (offset * EQ))) + (inputSample * (offset * EQ));
		inputSample = iirSampleD;
		//lowpass. Use offset from before gain stage
		
		if (flip)
		{
			OddD = OddC; OddC = OddB; OddB = OddA; OddA = inputSample;
			inputSample = (OddA + OddB + OddC + OddD) / 4.0;
		}
		else
		{
			EvenD = EvenC; EvenC = EvenB; EvenB = EvenA; EvenA = inputSample;
			inputSample = (EvenA + EvenB + EvenC + EvenD) / 4.0;
		}
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		bridgerectifier = fabs(inputSample) + skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSample > 0) inputSample = (inputSample*-0.57079633)+(bridgerectifier*1.57079633);
		else inputSample = (inputSample*-0.57079633)-(bridgerectifier*1.57079633);
		//output stage has less gain, no highpass, straight lowpass
		iirSampleE = (iirSampleE * (1 - EQ)) + (inputSample * EQ);
		inputSample = iirSampleE;
		//lowpass. Use offset from before gain stage
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample = sin(inputSample*outputlevel);
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		double randy = ((double(fpd)/UINT32_MAX)*0.034);
		inputSample = ((inputSample*(1-randy))+(storeSample*randy))*outputlevel;
		storeSample = inputSample;
		
		outSample = (inputSample * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSample * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSample * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
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
			
			dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.42133070619855229  - (0.18270903813104500*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.47209686171873821  - (0.27954009590498585*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (1.34648011331265294  - (0.47178343556301960*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.82133804036124580  - (0.41060189990353935*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.21628057120466901  - (0.26062442734317454*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.30306716082877883  + (0.10067648425439185*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.69484313178531876  - (0.09655574841702286*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.88320822356980833  - (0.26501644327144314*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.81326147029423723  - (0.31115926837054075*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.56728759049069222  - (0.23304233545561287*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.33340326645198737  - (0.12361361388240180*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.20280263733605616  - (0.03531960962500105*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.15864533788751345  + (0.00355160825317868*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.12544767480555119  + (0.01979010423176500*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.06666788902658917  + (0.00188830739903378*fabs(dram->b[15]))));
			inputSample += (dram->b[16] * (0.02977793355081072  + (0.02304216615605394*fabs(dram->b[16]))));
			inputSample += (dram->b[17] * (0.12821526330916558  + (0.02636238376777800*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.19933812710210136  - (0.02932657234709721*fabs(dram->b[18]))));
			inputSample += (dram->b[19] * (0.18346460191225772  - (0.12859581955080629*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00088697526755385  + (0.15855257539277415*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.28904286712096761  + (0.06226286786982616*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.49133546282552537  - (0.06512851581813534*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.52908013030763046  - (0.13606992188523465*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.45897241332311706  - (0.15527194946346906*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.35535938629924352  - (0.13634771941703441*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.26185269405237693  - (0.08736651482771546*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.19997351944186473  - (0.01714565029656306*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.18894054145105646  + (0.04557612705740050*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.24043993691153928  + (0.05267500387081067*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.29191852873822671  + (0.01922151122971644*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.29399783430587761  - (0.02238952856106585*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.26662219155294159  - (0.07760819463416335*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.20881206667122221  - (0.11930017354479640*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.12916658879944876  - (0.11798638949823513*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.07678815166012012  - (0.06826864734598684*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.08568505484529348  - (0.00510459741104792*fabs(dram->b[36]))));
			inputSample -= (dram->b[37] * (0.13613615872486634  + (0.02288223583971244*fabs(dram->b[37]))));
			inputSample -= (dram->b[38] * (0.17426657494209266  + (0.02723737220296440*fabs(dram->b[38]))));
			inputSample -= (dram->b[39] * (0.17343619261009030  + (0.01412920547179825*fabs(dram->b[39]))));
			inputSample -= (dram->b[40] * (0.14548368977428555  - (0.02640418940455951*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.10485295885802372  - (0.06334665781931498*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.06632268974717079  - (0.05960343688612868*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.06915692039882040  - (0.03541337869596061*fabs(dram->b[43]))));
			inputSample -= (dram->b[44] * (0.11889611687783583  - (0.02250608307287119*fabs(dram->b[44]))));
			inputSample -= (dram->b[45] * (0.14598456370320673  + (0.00280345943128246*fabs(dram->b[45]))));
			inputSample -= (dram->b[46] * (0.12312084125613143  + (0.04947283933434576*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.11379940289994711  + (0.06590080966570636*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.12963290754003182  + (0.02597647654256477*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.12723837402978638  - (0.04942071966927938*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.09185015882996231  - (0.10420810015956679*fabs(dram->b[50]))));
			inputSample -= (dram->b[51] * (0.04011592913036545  - (0.10234174227772008*fabs(dram->b[51]))));
			inputSample += (dram->b[52] * (0.00992597785057113  + (0.05674042373836896*fabs(dram->b[52]))));
			inputSample += (dram->b[53] * (0.04921452178306781  - (0.00222698867111080*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.06096504883783566  - (0.04040426549982253*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.04113530718724200  - (0.04190143593049960*fabs(dram->b[55]))));
			inputSample += (dram->b[56] * (0.01292699017654650  - (0.01121994018532499*fabs(dram->b[56]))));
			inputSample -= (dram->b[57] * (0.00437123132431870  - (0.02482497612289103*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.02090571264211918  - (0.03732746039260295*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.04749751678612051  - (0.02960060937328099*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.07675095194206227  - (0.02241927084099648*fabs(dram->b[60]))));
			inputSample -= (dram->b[61] * (0.08879414028581609  - (0.01144281133042115*fabs(dram->b[61]))));
			inputSample -= (dram->b[62] * (0.07378854974999530  + (0.02518742701599147*fabs(dram->b[62]))));
			inputSample -= (dram->b[63] * (0.04677309194488959  + (0.08984657372223502*fabs(dram->b[63]))));
			inputSample -= (dram->b[64] * (0.02911874044176449  + (0.14202665940555093*fabs(dram->b[64]))));
			inputSample -= (dram->b[65] * (0.02103564720234969  + (0.14640411976171003*fabs(dram->b[65]))));
			inputSample -= (dram->b[66] * (0.01940626429101940  + (0.10867274382865903*fabs(dram->b[66]))));
			inputSample -= (dram->b[67] * (0.03965401793931531  + (0.04775225375522835*fabs(dram->b[67]))));
			inputSample -= (dram->b[68] * (0.08102486457314527  - (0.03204447425666343*fabs(dram->b[68]))));
			inputSample -= (dram->b[69] * (0.11794849372825778  - (0.12755667382696789*fabs(dram->b[69]))));
			inputSample -= (dram->b[70] * (0.11946469076758266  - (0.20151394599125422*fabs(dram->b[70]))));
			inputSample -= (dram->b[71] * (0.07404630324668053  - (0.21300634351769704*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.00477584437144086  - (0.16864707684978708*fabs(dram->b[72]))));
			inputSample += (dram->b[73] * (0.05924822014377220  + (0.09394651445109450*fabs(dram->b[73]))));
			inputSample += (dram->b[74] * (0.10060989907457370  + (0.00419196431884887*fabs(dram->b[74]))));
			inputSample += (dram->b[75] * (0.10817907203844988  - (0.07459664480796091*fabs(dram->b[75]))));
			inputSample += (dram->b[76] * (0.08701102204768002  - (0.11129477437630560*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.05673785623180162  - (0.10638640242375266*fabs(dram->b[77]))));
			inputSample += (dram->b[78] * (0.02944190197442081  - (0.08499792583420167*fabs(dram->b[78]))));
			inputSample += (dram->b[79] * (0.01570145445652971  - (0.06190456843465320*fabs(dram->b[79]))));
			inputSample += (dram->b[80] * (0.02770233032476748  - (0.04573713136865480*fabs(dram->b[80]))));
			inputSample += (dram->b[81] * (0.05417160459175360  - (0.03965651064634598*fabs(dram->b[81]))));
			inputSample += (dram->b[82] * (0.06080831637644498  - (0.02909500789113911*fabs(dram->b[82]))));
			
			temp = (inputSample + smoothCabB)/3.0;
			smoothCabB = inputSample;
			inputSample = temp/4.0;
			
			
			randy = ((double(fpd)/UINT32_MAX)*0.085);
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
	OddA = 0.0;
	OddB = 0.0;
	OddC = 0.0;
	OddD = 0.0;
	OddE = 0.0;
	EvenA = 0.0;
	EvenB = 0.0;
	EvenC = 0.0;
	EvenD = 0.0;
	EvenE = 0.0;
	flip = false; //amp
	
	for(int fcount = 0; fcount < 90; fcount++) {dram->b[fcount] = 0;}
	smoothCabA = 0.0; smoothCabB = 0.0; lastCabSample = 0.0; //cab
	
	for (int fcount = 0; fcount < 9; fcount++) {lastRef[fcount] = 0.0;}
	cycle = 0; //undersampling
	
	for (int x = 0; x < fix_total; x++) {
		dram->fixA[x] = 0.0;
		dram->fixB[x] = 0.0;
		dram->fixC[x] = 0.0;
		dram->fixD[x] = 0.0;
		dram->fixE[x] = 0.0;
		dram->fixF[x] = 0.0;
	}	//filtering
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
