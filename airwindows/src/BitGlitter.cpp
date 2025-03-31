#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BitGlitter"
#define AIRWINDOWS_DESCRIPTION "An old-sampler style digital lo-fi plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','!' )
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
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bit Glitter", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 ataLastSample;
		Float64 ataHalfwaySample;
		Float64 ataDrySample;
		Float64 lastSample;
		Float64 heldSampleA;
		Float64 heldSampleB;
		Float64 positionA;
		Float64 positionB;
		Float64 lastOutputSample;
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 factor = GetParameter( kParam_Two )+1.0;
	factor = pow(factor,7)+2.0;
	int divvy = (int)(factor*overallscale);
	Float64 rateA = 1.0 / divvy;
	Float64 rezA = 0.0016666666666667; //looks to be a fixed bitcrush
	Float64 rateB = 1.61803398875 / divvy;
	Float64 rezB = 0.0026666666666667; //looks to be a fixed bitcrush
	Float64 offset;
	Float64 ingain = pow(10.0,GetParameter( kParam_One )/14.0); //add adjustment factor
	Float64 outgain = pow(10.0,GetParameter( kParam_Three )/14.0); //add adjustment factor
	Float64 wet = GetParameter( kParam_Four );

	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		double drySample = inputSample;
		//first, the distortion section
		inputSample *= ingain;
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		inputSample *= 1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output
		inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0) ?1:fabs(inputSample));
		
		ataDrySample = inputSample;
		ataHalfwaySample = (inputSample + ataLastSample ) / 2.0;
		ataLastSample = inputSample;
		//setting up crude oversampling
		
		//begin raw sample
		positionA += rateA;
		double outputSample = heldSampleA;
		if (positionA > 1.0)
		{
			positionA -= 1.0;
			heldSampleA = (lastSample * positionA) + (inputSample * (1-positionA));
			outputSample = (outputSample * 0.5) + (heldSampleA * 0.5);
			//softens the edge of the derez
		}
		if (outputSample > 0)
		{
			offset = outputSample;
			while (offset > 0) {offset -= rezA;}
			outputSample -= offset;
			//it's below 0 so subtracting adds the remainder
		}
		if (outputSample < 0)
		{
			offset = outputSample;
			while (offset < 0) {offset += rezA;}
			outputSample -= offset;
			//it's above 0 so subtracting subtracts the remainder
		}
		outputSample *= (1.0 - rezA);
		if (fabs(outputSample) < rezA) outputSample = 0.0;
		inputSample = outputSample;
		//end raw sample
		
		//begin interpolated sample
		positionB += rateB;
		outputSample = heldSampleB;
		if (positionB > 1.0)
		{
			positionB -= 1.0;
			heldSampleB = (lastSample * positionB) + (ataHalfwaySample * (1-positionB));
			outputSample = (outputSample * 0.5) + (heldSampleB * 0.5);
			//softens the edge of the derez
		}
		if (outputSample > 0)
		{
			offset = outputSample;
			while (offset > 0) {offset -= rezB;}
			outputSample -= offset;
			//it's below 0 so subtracting adds the remainder
		}
		if (outputSample < 0)
		{
			offset = outputSample;
			while (offset < 0) {offset += rezB;}
			outputSample -= offset;
			//it's above 0 so subtracting subtracts the remainder
		}
		outputSample *= (1.0 - rezB);
		if (fabs(outputSample) < rezB) outputSample = 0.0;
		ataHalfwaySample = outputSample;
		//end interpolated sample
		
		inputSample += ataHalfwaySample;
		inputSample /= 2.0;
		//plain old blend the two
		
		outputSample = (inputSample * (1.0-(wet/2))) + (lastOutputSample*(wet/2));
		//darken to extent of wet in wet/dry, maximum 50%
		lastOutputSample = inputSample;
		outputSample *= outgain;
		
		if (wet < 1.0) {
			outputSample = (drySample * (1.0-wet)) + (outputSample * wet);
		}
		
		*destP = outputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataLastSample = 0.0;
	ataHalfwaySample = 0.0;
	lastSample = 0.0;
	heldSampleA = 0.0;
	positionA = 0.0;
	heldSampleB = 0.0;
	positionB = 0.0;
	lastOutputSample = 0.0;
}
};
