#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Weight"
#define AIRWINDOWS_DESCRIPTION "A very accurate sub-bass boost based on Holt."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','W','e','i' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Frequency", .min = 2000, .max = 12000, .def = 6000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Weight", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;

		double previousSample[9];
		double previousTrend[9];
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
	
	Float64 targetFreq = (GetParameter( kParam_One )-20.0)/100.0;
	//gives us a 0-1 value like the VST will be. For the VST, start with 0-1 and
	//have the plugin display the number as 20-120.
	
	targetFreq = ((targetFreq+0.53)*0.2)/sqrt(overallscale);
	//must use square root of what the real scale would be, to get correct output
	Float64 alpha = pow(targetFreq,4);
	Float64 out = GetParameter( kParam_Two );
	Float64 resControl = (out*0.05)+0.2;
	Float64 beta = (alpha * pow(resControl,2));	
	alpha += ((1.0-beta)*pow(targetFreq,3)); //correct for droop in frequency
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		//begin Weight
		double trend;
		double forecast;
		for (int i = 0; i < 8; i++) {
			trend = (beta * (inputSample - previousSample[i]) + ((0.999-beta) * previousTrend[i]));
			forecast = previousSample[i] + previousTrend[i];
			inputSample = (alpha * inputSample) + ((0.999-alpha) * forecast);
			previousSample[i] = inputSample; previousTrend[i] = trend;
		}
		//inputSample is now the bass boost to be added
		
		inputSample *= out;
		inputSample += drySample;
		
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
	
	for (int i = 0; i < 8; i++) {
		previousSample[i] = 0.0;
		previousTrend[i] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
