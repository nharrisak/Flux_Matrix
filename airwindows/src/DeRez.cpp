#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeRez"
#define AIRWINDOWS_DESCRIPTION "An analog-style bit and sample rate crusher with continuous adjustments. This entry includes the Patreon blurb from mid-2018, because the things it says about the build systems, what they target and why I'm doing it, are still relevant today. That makes that stuff its own sort of Airwindowspedia entry as part of the DeRez entry."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','R' )
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
{ .name = "Rate", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 lastSample;
		Float64 heldSample;
		Float64 position;
		Float64 incrementA;
		Float64 incrementB;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{

	//This code will pass-thru the audio data.
	//This is where you want to process data to produce an effect.

	
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float64 targetA = pow(GetParameter( kParam_One ),3)+0.0005;
	if (targetA > 1.0) targetA = 1.0;
	Float64 soften = (1.0 + targetA)/2;
	Float64 targetB = pow(1.0-GetParameter( kParam_Two ),3) / 3;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	targetA /= overallscale;	

	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23 && (targetB == 0)) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		incrementA = ((incrementA*999.0)+targetA)/1000.0;
		incrementB = ((incrementB*999.0)+targetB)/1000.0;
		//incrementA is the frequency derez
		//incrementB is the bit depth derez
		position += incrementA;
		double outputSample = heldSample;
		if (position > 1.0)
			{
				position -= 1.0;
				heldSample = (lastSample * position) + (inputSample * (1-position));
				outputSample = (outputSample * (1-soften)) + (heldSample * soften);
				//softens the edge of the derez
			}
		inputSample = outputSample;
		double offset = inputSample;
		if (incrementB > 0.0005)
			{
				if (inputSample > 0)
					{
						while (offset > 0) {offset -= incrementB;}
						inputSample -= offset;
						//it's below 0 so subtracting adds the remainder
					}
				if (inputSample < 0)
					{
						while (offset < 0) {offset += incrementB;}
						inputSample -= offset;
						//it's above 0 so subtracting subtracts the remainder
					}
				inputSample *= (1.0 - incrementB);
			}
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither

		*destP = inputSample;
		lastSample = drySample;
		
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	heldSample = 0.0;
	position = 0.0;
	incrementA = 0.0;
	incrementB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
