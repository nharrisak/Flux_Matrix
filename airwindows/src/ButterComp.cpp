#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ButterComp"
#define AIRWINDOWS_DESCRIPTION "My softest, smoothest compressor."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','u','t' )
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
{ .name = "Compress", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float64 controlpos;
		Float64 controlneg;
		Float64 targetpos;
		Float64 targetneg;
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
	Float64 overallscale = 2.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 inputpos;
	Float64 inputneg;
	Float64 calcpos;
	Float64 calcneg;
	Float64 outputpos;
	Float64 outputneg;
	double totalmultiplier;
	double inputSample;
	Float64 drySample;
	Float64 inputgain = pow(10.0,(GetParameter( kParam_One )*14.0)/20.0);
	Float64 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	Float64 outputgain = inputgain;
	outputgain -= 1.0;
	outputgain /= 1.5;
	outputgain += 1.0;
	Float64 divisor = 0.012 * (GetParameter( kParam_One ) / 135.0);
	divisor /= overallscale;
	Float64 remainder = divisor;
	divisor = 1.0 - divisor;
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		inputSample *= inputgain;		
		inputpos = inputSample + 1.0;
		if (inputpos < 0.0) inputpos = 0.0;
		outputpos = inputpos / 2.0;
		if (outputpos > 1.0) outputpos = 1.0;		
		inputpos *= inputpos;
		targetpos *= divisor;
		targetpos += (inputpos * remainder);
		calcpos = pow((1.0/targetpos),2);

		inputneg = (-inputSample) + 1.0;
		if (inputneg < 0.0) inputneg = 0.0;
		outputneg = inputneg / 2.0;
		if (outputneg > 1.0) outputneg = 1.0;		
		inputneg *= inputneg;
		targetneg *= divisor;
		targetneg += (inputneg * remainder);
		calcneg = pow((1.0/targetneg),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1

		//this is an example of a simple C bug one might get.
		//ButterComp2 has this routine the way it was intended, but in the original ButterComp
		//and the reissue (kept exactly the same sonically) there's an if statement that went:
		// if (fpFlip = true) {}
		//That's an assignment operator. So ButterComp Original never interleaved compressors
		//(though it still did the bi-polar thing it was meant to do)
		//and this is simplified code, leaving out the parts that never executed.
		if (inputSample > 0)
			{ //working on pos
				controlpos *= divisor;
				controlpos += (calcpos*remainder);
			}
		else
			{ //working on neg
				controlneg *= divisor;
				controlneg += (calcneg*remainder);
			}
		//this causes each of the four to update only when active and in the correct 'flip'

		totalmultiplier = (controlpos * outputpos) + (controlneg * outputneg);
		//this combines the sides according to flip, blending relative to the input value

		inputSample *= totalmultiplier;
		inputSample /= outputgain;
		if (wet < 1.0) inputSample = (drySample * (1.0-wet))+(inputSample*wet);

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
	controlpos = 1.0;
	controlneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
