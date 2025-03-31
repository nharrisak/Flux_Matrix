#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Podcast"
#define AIRWINDOWS_DESCRIPTION "A simpler pile of curve-style compressors with hard clipping."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','e' )
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
{ .name = "Boost", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float64 c1;
		Float64 c2;
		Float64 c3;
		Float64 c4;
		Float64 c5;
		//the compressor
		
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

	Float64 compress = 1.0 + pow(GetParameter( kParam_One ),2);
	Float64 wet = GetParameter( kParam_Two );
	Float64 speed1 = 64.0 / pow(compress,2);
	speed1 *= overallscale;
	Float64 speed2 = speed1 * 1.4;
	Float64 speed3 = speed2 * 1.5;
	Float64 speed4 = speed3 * 1.6;
	Float64 speed5 = speed4 * 1.7;
	Float64 trigger;
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
				
		inputSample *= c1;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c1 += trigger/speed5;
		if (c1 > compress) c1 = compress;
		//compress stage
		
		inputSample *= c2;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c2 += trigger/speed4;
		if (c2 > compress) c2 = compress;
		//compress stage
				
		inputSample *= c3;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c3 += trigger/speed3;
		if (c3 > compress) c3 = compress;
		//compress stage
		
		inputSample *= c4;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c4 += trigger/speed2;
		if (c4 > compress) c4 = compress;
		//compress stage
				
		inputSample *= c5;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c5 += trigger/speed1;
		if (c5 > compress) c5 = compress;
		//compress stage
		
		if (compress > 1.0) inputSample /= compress;
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		if (inputSample > 0.999) inputSample = 0.999;
		if (inputSample < -0.999) inputSample = -0.999;
		
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
	c1 = 2.0; c2 = 2.0; c3 = 2.0; c4 = 2.0; c5 = 2.0; //startup comp gains
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
