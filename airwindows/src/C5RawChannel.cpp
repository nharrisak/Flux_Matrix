#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "C5RawChannel"
#define AIRWINDOWS_DESCRIPTION "C5RawChannel"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','5','S' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Centering Force", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 lastSampleChannel;
		Float64 lastFXChannel;
		uint32_t fpd;
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

	double centering = GetParameter( kParam_One ) * 0.5;
	centering = 1.0 - pow(centering,5);
	//we can set our centering force from zero to rather high, but
	//there's a really intense taper on it forcing it to mostly be almost 1.0.
	//If it's literally 1.0, we don't even apply it, and you get the original
	//Xmas Morning bugged-out Console5, which is the default setting for Raw Console5.
	Float64 difference;

	double inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;

		difference = lastSampleChannel - inputSample;
		lastSampleChannel = inputSample;
		//derive slew part off direct sample measurement + from last time
		
		if (difference > 1.0) difference = 1.0;
		if (difference < -1.0) difference = -1.0;
		inputSample = lastFXChannel + asin(difference);
		lastFXChannel = inputSample;
		if (centering < 1.0) lastFXChannel *= centering;
		//if we're using the crude centering force, it's applied here
		if (lastFXChannel > 1.0) lastFXChannel = 1.0;
		if (lastFXChannel < -1.0) lastFXChannel = -1.0;
		//build new signal off what was present in output last time
		//slew aspect
		
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//amplitude aspect
		
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
	lastFXChannel = 0.0;
	lastSampleChannel = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
