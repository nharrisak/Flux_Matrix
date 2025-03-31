#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "LeadAmp"
#define AIRWINDOWS_DESCRIPTION "An amp sim with a clear, penetrating, vocal tone."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','e','a' )
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
		Float64 iirLowpass;
		Float64 iirSpkA;
		Float64 iirSpkB;
		Float64 iirSub;
		bool flip;
		int count;
		
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
			Float64 Odd[257];
		Float64 Even[257];
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
	
	double bassfill = GetParameter( kParam_One );
	double startlevel = bassfill;
	double samplerate = GetSampleRate();
	double basstrim = bassfill / 10.0;
	double toneEQ = (GetParameter( kParam_Two ) / samplerate)*22050.0;
	double EQ = (basstrim / samplerate)*22050.0;
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
				
		double basscut = 0.99;
		//we're going to be shifting this as the stages progress
		double inputlevel = startlevel;
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleA = (iirSampleA * (1 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleA*basscut);
		//highpass
		double bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//overdrive
		bridgerectifier = (smoothA + inputSample);
		smoothA = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		
		outSample = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (outSample * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (outSample * dram->fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleB = (iirSampleB * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleB*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothB + inputSample);
		smoothB = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixC[fix_a0]) + dram->fixC[fix_sL1];
		dram->fixC[fix_sL1] = (inputSample * dram->fixC[fix_a1]) - (outSample * dram->fixC[fix_b1]) + dram->fixC[fix_sL2];
		dram->fixC[fix_sL2] = (inputSample * dram->fixC[fix_a2]) - (outSample * dram->fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleC = (iirSampleC * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleC*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothC + inputSample);
		smoothC = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleD = (iirSampleD * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleD*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothD + inputSample);
		smoothD = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixD[fix_a0]) + dram->fixD[fix_sL1];
		dram->fixD[fix_sL1] = (inputSample * dram->fixD[fix_a1]) - (outSample * dram->fixD[fix_b1]) + dram->fixD[fix_sL2];
		dram->fixD[fix_sL2] = (inputSample * dram->fixD[fix_a2]) - (outSample * dram->fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleE = (iirSampleE * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleE*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothE + inputSample);
		smoothE = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleF = (iirSampleF * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleF*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothF + inputSample);
		smoothF = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixE[fix_a0]) + dram->fixE[fix_sL1];
		dram->fixE[fix_sL1] = (inputSample * dram->fixE[fix_a1]) - (outSample * dram->fixE[fix_b1]) + dram->fixE[fix_sL2];
		dram->fixE[fix_sL2] = (inputSample * dram->fixE[fix_a2]) - (outSample * dram->fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleG = (iirSampleG * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleG*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothG + inputSample);
		smoothG = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleH = (iirSampleH * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleH*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothH + inputSample);
		smoothH = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		outSample = (inputSample * dram->fixF[fix_a0]) + dram->fixF[fix_sL1];
		dram->fixF[fix_sL1] = (inputSample * dram->fixF[fix_a1]) - (outSample * dram->fixF[fix_b1]) + dram->fixF[fix_sL2];
		dram->fixF[fix_sL2] = (inputSample * dram->fixF[fix_a2]) - (outSample * dram->fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
				
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleI = (iirSampleI * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleI*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothI + inputSample);
		smoothI = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleJ = (iirSampleJ * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleJ*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothJ + inputSample);
		smoothJ = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		inputSample *= inputlevel;
		inputlevel = ((inputlevel * 7.0)+1.0)/8.0;
		iirSampleK = (iirSampleK * (1.0 - EQ)) + (inputSample *  EQ);
		basscut *= bassfactor;
		inputSample = inputSample - (iirSampleK*basscut);
		//highpass
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//overdrive
		bridgerectifier = (smoothK + inputSample);
		smoothK = inputSample;
		inputSample = bridgerectifier;
		//two-sample averaging lowpass
		
		iirLowpass = (iirLowpass * (1.0 - toneEQ)) + (inputSample * toneEQ);
		inputSample = iirLowpass;
		//lowpass. The only one of this type.
		
		iirSpkA = (iirSpkA * (1.0 -  BEQ)) + (inputSample * BEQ);
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
		inputSample += (iirSpkB*bleed);
		//extra lowpass for 4*12" speakers
		
		iirSub = (iirSub * (1 - BEQ)) + (inputSample * BEQ);
		inputSample += (iirSub * bassfill);
		
		
		bridgerectifier = fabs(inputSample*outputlevel);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		
		double randy = ((double(fpd)/UINT32_MAX)*0.084);
		inputSample = ((inputSample*(1.0-randy))+(storeSample*randy))*outputlevel;
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
			
			dram->b[85] = dram->b[84]; dram->b[84] = dram->b[83]; dram->b[83] = dram->b[82]; dram->b[82] = dram->b[81]; dram->b[81] = dram->b[80]; dram->b[80] = dram->b[79]; 
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
			inputSample += (dram->b[1] * (1.30406584776167445  - (0.01410622186823351*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (1.09350974154373559  + (0.34478044709202327*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (0.52285510059938256  + (0.84225842837363574*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.00018126260714707  - (1.02446537989058117*fabs(dram->b[4]))));
			inputSample -= (dram->b[5] * (0.34943699771860115  - (0.84094709567790016*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.53068048407937285  - (0.49231169327705593*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.48631669406792399  - (0.08965111766223610*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.28099201947014130  + (0.23921137841068607*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.10333290012666248  + (0.35058962687321482*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.06605032198166226  + (0.23447405567823365*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.10485808661261729  + (0.05025985449763527*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.13231190973014911  - (0.05484648240248013*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.12926184767180304  - (0.04054223744746116*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.13802696739839460  + (0.01876754906568237*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.16548980700926913  + (0.06772130758771169*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.14469310965751475  + (0.10590928840978781*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.07838457396093310  + (0.13120101199677947*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.05123031606187391  + (0.13883400806512292*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.08906103481939850  + (0.07840461228402337*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.13939265522625241  + (0.01194366471800457*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.14957600717294034  + (0.07687598594361914*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.14112708654047090  + (0.20118461131186977*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.14961020766492997  + (0.30005716443826147*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.16130382224652270  + (0.40459872030013055*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.15679868471080052  + (0.47292767226083465*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.16456530552807727  + (0.45182121471666481*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.16852385701909278  + (0.38272684270752266*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.13317562760966850  + (0.28829580273670768*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.09396196532150952  + (0.18886898332071317*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.10133496956734221  + (0.11158788414137354*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.16097596389376778  + (0.02621299102374547*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.21419006394821866  - (0.03585678078834797*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.21273234570555244  - (0.02574469802924526*fabs(dram->b[33]))));
			inputSample -= (dram->b[34] * (0.16934948798707830  + (0.01354331184333835*fabs(dram->b[34]))));
			inputSample -= (dram->b[35] * (0.11970436472852493  + (0.04242183865883427*fabs(dram->b[35]))));
			inputSample -= (dram->b[36] * (0.09329023656747724  + (0.06890873292358397*fabs(dram->b[36]))));
			inputSample -= (dram->b[37] * (0.10255328436608116  + (0.11482972519137427*fabs(dram->b[37]))));
			inputSample -= (dram->b[38] * (0.13883223352796811  + (0.18016014431438840*fabs(dram->b[38]))));
			inputSample -= (dram->b[39] * (0.16532844286979087  + (0.24521957638633446*fabs(dram->b[39]))));
			inputSample -= (dram->b[40] * (0.16254607738965438  + (0.25669472097572482*fabs(dram->b[40]))));
			inputSample -= (dram->b[41] * (0.15353207135544752  + (0.15048064682912729*fabs(dram->b[41]))));
			inputSample -= (dram->b[42] * (0.13039046390746015  - (0.00200335414623601*fabs(dram->b[42]))));
			inputSample -= (dram->b[43] * (0.06707245032180627  - (0.06498125592578702*fabs(dram->b[43]))));
			inputSample += (dram->b[44] * (0.01427326441869788  + (0.01940451360783622*fabs(dram->b[44]))));
			inputSample += (dram->b[45] * (0.06151238306578224  - (0.07335755969763329*fabs(dram->b[45]))));
			inputSample += (dram->b[46] * (0.04685840498892526  - (0.14258849371688248*fabs(dram->b[46]))));
			inputSample -= (dram->b[47] * (0.00950136304466093  + (0.14379354707665129*fabs(dram->b[47]))));
			inputSample -= (dram->b[48] * (0.06245771575493557  + (0.07639718586346110*fabs(dram->b[48]))));
			inputSample -= (dram->b[49] * (0.07159593175777741  - (0.00595536565276915*fabs(dram->b[49]))));
			inputSample -= (dram->b[50] * (0.03167929390245019  - (0.03856769526301793*fabs(dram->b[50]))));
			inputSample += (dram->b[51] * (0.01890898565110766  + (0.00760539424271147*fabs(dram->b[51]))));
			inputSample += (dram->b[52] * (0.04926161137832240  - (0.06411014430053390*fabs(dram->b[52]))));
			inputSample += (dram->b[53] * (0.05768814623421683  - (0.15068618173358578*fabs(dram->b[53]))));
			inputSample += (dram->b[54] * (0.06144258297076708  - (0.21200636329120301*fabs(dram->b[54]))));
			inputSample += (dram->b[55] * (0.06348341960185613  - (0.19620269813094307*fabs(dram->b[55]))));
			inputSample += (dram->b[56] * (0.04877736350310589  - (0.11864999881200111*fabs(dram->b[56]))));
			inputSample += (dram->b[57] * (0.01010950997574472  - (0.02630070679113791*fabs(dram->b[57]))));
			inputSample -= (dram->b[58] * (0.02929178864801191  - (0.04439260202207482*fabs(dram->b[58]))));
			inputSample -= (dram->b[59] * (0.03484517126321562  - (0.04508635396034735*fabs(dram->b[59]))));
			inputSample -= (dram->b[60] * (0.00547176780437610  - (0.00205637806941426*fabs(dram->b[60]))));
			inputSample += (dram->b[61] * (0.02278296865283977  - (0.00063732526427685*fabs(dram->b[61]))));
			inputSample += (dram->b[62] * (0.02688982591366477  + (0.05333738901586284*fabs(dram->b[62]))));
			inputSample += (dram->b[63] * (0.01942012754957055  + (0.10942832669749143*fabs(dram->b[63]))));
			inputSample += (dram->b[64] * (0.01572585258756565  + (0.11189204189054594*fabs(dram->b[64]))));
			inputSample += (dram->b[65] * (0.01490550715016034  + (0.04449822818925343*fabs(dram->b[65]))));
			inputSample += (dram->b[66] * (0.01715683226376727  - (0.06944648050933899*fabs(dram->b[66]))));
			inputSample += (dram->b[67] * (0.02822659878011318  - (0.17843652160132820*fabs(dram->b[67]))));
			inputSample += (dram->b[68] * (0.03758307610456144  - (0.21986013433664692*fabs(dram->b[68]))));
			inputSample += (dram->b[69] * (0.03275008021608433  - (0.15869878676112170*fabs(dram->b[69]))));
			inputSample += (dram->b[70] * (0.01855749786752354  - (0.02337224995718105*fabs(dram->b[70]))));
			inputSample += (dram->b[71] * (0.00217095395782931  + (0.10971764224593601*fabs(dram->b[71]))));
			inputSample -= (dram->b[72] * (0.01851381451105007  - (0.17214910008793413*fabs(dram->b[72]))));
			inputSample -= (dram->b[73] * (0.04722574936345419  - (0.14341588977845254*fabs(dram->b[73]))));
			inputSample -= (dram->b[74] * (0.07151540514482006  - (0.04684695724814321*fabs(dram->b[74]))));
			inputSample -= (dram->b[75] * (0.06827195484995092  + (0.07022207121861397*fabs(dram->b[75]))));
			inputSample -= (dram->b[76] * (0.03290227240464227  + (0.16328400808152735*fabs(dram->b[76]))));
			inputSample += (dram->b[77] * (0.01043861198275382  - (0.20184486126076279*fabs(dram->b[77]))));
			inputSample += (dram->b[78] * (0.03236563559476477  - (0.17125821306380920*fabs(dram->b[78]))));
			inputSample += (dram->b[79] * (0.02040121529932702  - (0.09103660189829657*fabs(dram->b[79]))));
			inputSample -= (dram->b[80] * (0.00509649513318102  + (0.01170360991547489*fabs(dram->b[80]))));
			inputSample -= (dram->b[81] * (0.01388353426600228  - (0.03588955538451771*fabs(dram->b[81]))));
			inputSample -= (dram->b[82] * (0.00523671715033842  - (0.07068798057534148*fabs(dram->b[82]))));
			inputSample += (dram->b[83] * (0.00665852487721137  + (0.11666210640054926*fabs(dram->b[83]))));
			inputSample += (dram->b[84] * (0.01593540832939290  + (0.15844892856402149*fabs(dram->b[84]))));
			inputSample += (dram->b[85] * (0.02080509201836796  + (0.17186274420065850*fabs(dram->b[85]))));
			
			temp = (inputSample + smoothCabB)/3.0;
			smoothCabB = inputSample;
			inputSample = temp/4.0;
			
			randy = ((double(fpd)/UINT32_MAX)*0.079);
			drySample = ((((inputSample*(1.0-randy))+(lastCabSample*randy))*wet)+(drySample*(1.0-wet)))*outputlevel;
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
