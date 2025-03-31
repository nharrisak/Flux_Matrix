#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Coils2"
#define AIRWINDOWS_DESCRIPTION "A transformer overdrive emulator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','j' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Saturation", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Cheapness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		double biquadA[9];
		double biquadB[9];
		double hysteresis;
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
	
	Float64 distScaling = pow(1.0-GetParameter( kParam_One ),2);
	if (distScaling < 0.0001) distScaling = 0.0001;
	biquadA[0] = 600.0/GetSampleRate();
	biquadA[1] = 0.01+(pow(GetParameter( kParam_Two ),2)*0.5);
	double iirAmount = biquadA[1]/overallscale;
	double K = tan(M_PI * biquadA[0]);
	double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
	biquadA[2] = K / biquadA[1] * norm;
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
	biquadB[0] = (21890.0-(biquadA[1]*890.0))/GetSampleRate();
	biquadB[1] = 0.89;
	K = tan(M_PI * biquadB[0]);
	norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0 * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
	Float64 wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		if (biquadA[0] < 0.49999) {
			double tempSample = (inputSample * biquadA[2]) + biquadA[7];
			biquadA[7] = -(tempSample * biquadA[5]) + biquadA[8];
			biquadA[8] = (inputSample * biquadA[4]) - (tempSample * biquadA[6]);
			inputSample = tempSample; //create bandpass of clean tone
		}
		double diffSample = (drySample-inputSample)/distScaling; //mids notched out		
		if (biquadB[0] < 0.49999) {
			double tempSample = (diffSample * biquadB[2]) + biquadB[7];
			biquadB[7] = (diffSample * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
			biquadB[8] = (diffSample * biquadB[4]) - (tempSample * biquadB[6]);
			diffSample = tempSample; //lowpass filter the notch content before distorting
		}
		hysteresis = (hysteresis * (1.0-iirAmount)) + (diffSample * iirAmount);
		if (fabs(hysteresis)<1.18e-37) hysteresis = 0.0; else diffSample -= hysteresis;
		if (diffSample > 1.571) diffSample = 1.571; else if (diffSample < -1.571) diffSample = -1.571;
		if (hysteresis > 1.571) hysteresis = 1.571; else if (hysteresis < -1.571) hysteresis = -1.571;
		inputSample += (sin(diffSample)-sin(hysteresis))*distScaling; //apply transformer distortions
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
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
	for (int x = 0; x < 9; x++) {biquadA[x] = 0.0; biquadB[x] = 0.0;}
	hysteresis = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
