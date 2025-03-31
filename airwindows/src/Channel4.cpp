#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Channel4"
#define AIRWINDOWS_DESCRIPTION "A tone coloring plugin that gives more of an analog feel."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','c' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	//Add your parameters here...
	kNumberOfParameters=2
};
static const int kNeve = 1;
static const int kAPI = 2;
static const int kSSL = 3;
static const int kDefaultValue_ParamOne = kNeve;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static char const * const enumStrings0[] = { "", "Neve", "API", "SSL", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Console Type", .min = 1, .max = 3, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Drive", .min = 0, .max = 10000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
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
 
		Float64 fpNShapeA;
		Float64 fpNShapeB;
		Float64 iirSampleA;
		Float64 iirSampleB;
		bool flip;
		Float64 lastSample;
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int console = (int) GetParameter( kParam_One );
	Float64 density = pow(GetParameter( kParam_Two )/100.0,2);
	double bridgerectifier;
	Float64 iirAmount = 0.005832;
	Float64 threshold = 0.33362176;
	Float64 clamp;
	double inputSample;
	Float32 fpTemp;
	Float64 fpOld = 0.618033988749894848204586; //golden ratio!
	Float64 fpNew = 1.0 - fpOld;
	
	switch (console)
	{
		case 1: iirAmount = 0.005832; threshold = 0.33362176; break; //Neve
		case 2: iirAmount = 0.004096; threshold = 0.59969536; break; //API
		case 3: iirAmount = 0.004913; threshold = 0.84934656; break; //SSL
	}
	iirAmount /= overallscale;
	threshold /= overallscale; //now with 96K AND working selector!
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		
		if (flip)
		{
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample = inputSample - iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample = inputSample - iirSampleB;
		}
		//highpass section
		bridgerectifier = fabs(inputSample)*1.57079633;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = (inputSample*(1-density))+(bridgerectifier*density);
		else inputSample = (inputSample*(1-density))-(bridgerectifier*density);
		//drive section
		clamp = inputSample - lastSample;
		if (clamp > threshold)
			inputSample = lastSample + threshold;
		if (-clamp > threshold)
			inputSample = lastSample - threshold;
		lastSample = inputSample;
		//slew section
		
		//noise shaping to 32-bit floating point
		if (flip) {
			fpTemp = inputSample;
			fpNShapeA = (fpNShapeA*fpOld)+((inputSample-fpTemp)*fpNew);
			inputSample += fpNShapeA;
		}
		else {
			fpTemp = inputSample;
			fpNShapeB = (fpNShapeB*fpOld)+((inputSample-fpTemp)*fpNew);
			inputSample += fpNShapeB;
		}
		//end noise shaping on 32 bit output
		flip = !flip;
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpNShapeA = 0.0;
	fpNShapeB = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	lastSample = 0.0;
}
};
