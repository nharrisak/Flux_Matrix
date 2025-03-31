#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Distance"
#define AIRWINDOWS_DESCRIPTION "A sound design or reverb far-away-izer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','u' )
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
{ .name = "Distance", .min = 0, .max = 2000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 lastclamp;
		Float64 clamp;
		Float64 change;
		Float64 thirdresult;
		Float64 prevresult;
		Float64 last;
		double fpNShape;
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

	//This code will pass-thru the audio data.
	//This is where you want to process data to produce an effect.
	//persistent- lastclamp, clamp, change, last
	
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	Float64 softslew = (pow(GetParameter( kParam_One ),3)*12)+.6;
	softslew *= overallscale;
	Float64 filtercorrect = softslew / 2.0;
	Float64 thirdfilter = softslew / 3.0;
	Float64 levelcorrect = 1.0 + (softslew / 6.0);
	Float64 postfilter;
	Float64 wet = GetParameter( kParam_Two );
	Float64 dry = 1.0-wet;
	Float64 bridgerectifier;
	Float64 inputSample;
	Float64 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		
		inputSample *= softslew;
		lastclamp = clamp;
		clamp = inputSample - last;
		postfilter = change = fabs(clamp - lastclamp);
		postfilter += filtercorrect;
		if (change > 1.5707963267949) change = 1.5707963267949;
		bridgerectifier = (1.0-sin(change));
		if (bridgerectifier < 0.0) bridgerectifier = 0.0;
		inputSample = last + (clamp * bridgerectifier);
		last = inputSample;
		inputSample /= softslew;
		inputSample += (thirdresult * thirdfilter);
		inputSample /= (thirdfilter + 1.0);
		inputSample += (prevresult * postfilter);
		inputSample /= (postfilter + 1.0);
		//do an IIR like thing to further squish superdistant stuff
		thirdresult = prevresult;
		prevresult = inputSample;
		inputSample *= levelcorrect;

		if (wet < 1.0) inputSample = (drySample * dry)+(inputSample*wet);
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	thirdresult = prevresult = lastclamp = clamp = change = last = 0.0;
	//just an example
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
