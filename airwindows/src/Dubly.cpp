#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dubly"
#define AIRWINDOWS_DESCRIPTION "A retro vibe and atmosphere maker."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','f' )
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
{ .name = "Dubly", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		double iirSampleA;
		double iirSampleB;
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double dublyAmount = pow(GetParameter( kParam_One ),3)*0.16;
	double outlyAmount = pow(GetParameter( kParam_One ),3)*0.160618;
	double gain = outlyAmount+1.0;
	double iirAmount = 0.4275/overallscale;
	double wet = GetParameter( kParam_Two );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		iirSampleA = (iirSampleA * (1.0 - iirAmount)) + (inputSample * iirAmount);
		double doubly = inputSample - iirSampleA;
		if (doubly > 1.0) doubly = 1.0; if (doubly < -1.0) doubly = -1.0;
		if (doubly > 0) doubly = log(1.0+(255*fabs(doubly)))/2.40823996531;
		if (doubly < 0) doubly = -log(1.0+(255*fabs(doubly)))/2.40823996531;
		inputSample += doubly*dublyAmount;
		//end Dubly encode
		
		inputSample *= gain;
		inputSample = sin(inputSample);
		inputSample /= gain;
		
		iirSampleB = (iirSampleB * (1.0 - iirAmount)) + (inputSample * iirAmount);
		doubly = inputSample - iirSampleB;
		if (doubly > 1.0) doubly = 1.0; if (doubly < -1.0) doubly = -1.0;
		if (doubly > 0) doubly = log(1.0+(255*fabs(doubly)))/2.40823996531;
		if (doubly < 0) doubly = -log(1.0+(255*fabs(doubly)))/2.40823996531;
		inputSample -= doubly*outlyAmount;
		//end Dubly decode
		
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
	iirSampleA = 0.0; iirSampleB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
