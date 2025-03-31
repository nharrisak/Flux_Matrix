#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ZBandpass"
#define AIRWINDOWS_DESCRIPTION "A bandpass made to sound and act like the Emu e6400 Ultra bandpass."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','B','a' )
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
{ .name = "Input", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double biquadA[11];
		double biquadB[11];
		double biquadC[11];
		double biquadD[11];
		double biquadE[11];
		double biquadF[11];
		double iirSampleA;

		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	biquadA[0] = ((pow(GetParameter( kParam_Two ),4)*14300.0)/GetSampleRate())+0.00079;
	double clipFactor = 1.0 - ((1.0 - GetParameter( kParam_Two ))*0.304);

	biquadA[1] = 0.314;
	double K = tan(M_PI * biquadA[0]);
	double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
	biquadA[2] = K / biquadA[1] * norm;
	biquadA[3] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
	for (int x = 0; x < 7; x++) {biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x];}
	
	//opamp stuff
	double inTrim = GetParameter( kParam_One )*10.0;
	inTrim *= inTrim; inTrim *= inTrim;
	double outPad = GetParameter( kParam_Three )*10.0;
	double iirAmountA = 0.00069/overallscale;
	biquadF[0] = biquadE[0] = 15500.0 / GetSampleRate();
    biquadF[1] = biquadE[1] = 0.935;
	K = tan(M_PI * biquadE[0]); //lowpass
	norm = 1.0 / (1.0 + K / biquadE[1] + K * K);
	biquadE[2] = K * K * norm;
	biquadE[3] = 2.0 * biquadE[2];
	biquadE[4] = biquadE[2];
	biquadE[5] = 2.0 * (K * K - 1.0) * norm;
	biquadE[6] = (1.0 - K / biquadE[1] + K * K) * norm;
	for (int x = 0; x < 7; x++) biquadF[x] = biquadE[x];
	//end opamp stuff	
	
 	double trim = 0.1+(3.712*biquadA[0]);
	double wet = pow(GetParameter( kParam_Four ),2);
	double aWet = 1.0;
	double bWet = 1.0;
	double cWet = 1.0;
	double dWet = wet*4.0;
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0) {aWet = dWet; bWet = 0.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 2.0) {bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 3.0) {cWet = dWet - 2.0; dWet = 0.0;}
	else {dWet -= 3.0;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	double outSample = 0.0;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		double overallDrySample = drySample;
		
		if (inTrim != 1.0) inputSample *= inTrim;
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		inputSample *= trim;
		
		outSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample; biquadA[10] = biquadA[9];
		if (outSample > 1.0) outSample = 1.0;
		if (outSample < -1.0) outSample = -1.0;
		drySample = inputSample = biquadA[9] = outSample; //DF1
		
		if (bWet > 0.0) {
			inputSample /= clipFactor;
			outSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; biquadB[10] = biquadB[9]; 
			if (outSample > 1.0) outSample = 1.0;
			if (outSample < -1.0) outSample = -1.0;
			biquadB[9] = outSample; //DF1
			drySample = inputSample = (outSample * bWet) + (drySample * (1.0-bWet));
		}
		if (cWet > 0.0) {
			inputSample /= clipFactor;
			outSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSample; biquadC[10] = biquadC[9]; 
			if (outSample > 1.0) outSample = 1.0;
			if (outSample < -1.0) outSample = -1.0;
			biquadC[9] = outSample; //DF1
			drySample = inputSample = (outSample * cWet) + (drySample * (1.0-cWet));
		}
		if (dWet > 0.0) {
			inputSample /= clipFactor;
			outSample = biquadD[2]*inputSample+biquadD[3]*biquadD[7]+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSample; biquadD[10] = biquadD[9]; 
			if (outSample > 1.0) outSample = 1.0;
			if (outSample < -1.0) outSample = -1.0;
			biquadD[9] = outSample; //DF1
			drySample = inputSample = (outSample * dWet) + (drySample * (1.0-dWet));
		}
		
		inputSample /= clipFactor;
		//opamp stage
		if (fabs(iirSampleA)<1.18e-37) iirSampleA = 0.0;
		iirSampleA = (iirSampleA * (1.0 - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		outSample = biquadE[2]*inputSample+biquadE[3]*biquadE[7]+biquadE[4]*biquadE[8]-biquadE[5]*biquadE[9]-biquadE[6]*biquadE[10];
		biquadE[8] = biquadE[7]; biquadE[7] = inputSample; inputSample = outSample; biquadE[10] = biquadE[9]; biquadE[9] = inputSample; //DF1		
		
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		inputSample -= (inputSample*inputSample*inputSample*inputSample*inputSample*0.1768);
		
		outSample = biquadF[2]*inputSample+biquadF[3]*biquadF[7]+biquadF[4]*biquadF[8]-biquadF[5]*biquadF[9]-biquadF[6]*biquadF[10];
		biquadF[8] = biquadF[7]; biquadF[7] = inputSample; inputSample = outSample; biquadF[10] = biquadF[9]; biquadF[9] = inputSample; //DF1
		
		if (outPad != 1.0) inputSample *= outPad;		
		//end opamp stage
		
		if (aWet !=1.0) {
			inputSample = (inputSample * aWet) + (overallDrySample * (1.0-aWet));
		}	
		
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
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0; biquadE[x] = 0.0; biquadF[x] = 0.0;}
	iirSampleA = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
