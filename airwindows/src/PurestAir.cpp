#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestAir"
#define AIRWINDOWS_DESCRIPTION "A brightness experiment."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','r' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Air Boost", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Air Limit", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double last1Sample;
		double halfwaySample;
		double halfDrySample;
		double halfDiffSample;
		double diffSample;
		double lastSample;
		Float64 s1;
		Float64 s2;
		Float64 s3;
		Float64 apply;
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

	Float64 applyTarget = GetParameter( kParam_One );
	Float64 threshold = pow((1-fabs(applyTarget)),3);
	if (applyTarget > 0) applyTarget *= 3;
	
	Float64 intensity = pow(GetParameter( kParam_Two ),2)*5.0;
	Float64 wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		halfDrySample = halfwaySample = (inputSample + last1Sample) / 2.0;
		last1Sample = inputSample;
		s3 = s2;
		s2 = s1;
		s1 = inputSample;
		Float64 m1 = (s1-s2)*((s1-s2)/1.3);
		Float64 m2 = (s2-s3)*((s1-s2)/1.3);
		Float64 sense = fabs((m1-m2)*((m1-m2)/1.3))*intensity;
		//this will be 0 for smooth, high for SSS
		apply += applyTarget - sense;
		apply *= 0.5;
		if (apply < -1.0) apply = -1.0;
		
		Float64 clamp = halfwaySample - halfDrySample;
		if (clamp > threshold) halfwaySample = lastSample + threshold;
		if (-clamp > threshold) halfwaySample = lastSample - threshold;
		lastSample = halfwaySample;
		
		clamp = inputSample - lastSample;
		if (clamp > threshold) inputSample = lastSample + threshold;
		if (-clamp > threshold) inputSample = lastSample - threshold;
		lastSample = inputSample;
		
		diffSample = *sourceP - inputSample;
		halfDiffSample = halfDrySample - halfwaySample;
		
		inputSample = *sourceP + ((diffSample + halfDiffSample)*apply);
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
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
	last1Sample = 0.0;
	halfwaySample = halfDrySample = halfDiffSample = diffSample = 0.0;
	lastSample = 0.0;
	s1 = s2 = s3 = 0.0;
	apply = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
