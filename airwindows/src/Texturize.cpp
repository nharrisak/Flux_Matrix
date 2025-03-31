#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Texturize"
#define AIRWINDOWS_DESCRIPTION "A hidden-noise plugin for adding sonic texture to things."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','e','x' )
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
{ .name = "Bright", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Punchy", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		bool polarity;
		double lastSample;
		double iirSample;
		double noiseA;
		double noiseB;
		double noiseC;
		bool flip;
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
	
	Float64 slewAmount = ((pow(GetParameter( kParam_One ),2.0)*4.0)+0.71)/overallscale;
	Float64 dynAmount = pow(GetParameter( kParam_Two ),2.0);
	Float64 wet = pow(GetParameter( kParam_Three ),5);
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		if (inputSample < 0) {
			if (polarity == true) {
				if (noiseA < 0) flip = true;
				else flip = false;
			}
			polarity = false;
		} else polarity = true;		
		
		if (flip) noiseA += (double(fpd)/UINT32_MAX);
		else noiseA -= (double(fpd)/UINT32_MAX);
		//here's the guts of the random walk		
		flip = !flip;
		
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		if (dynAmount < 0.4999) inputSample = (inputSample*dynAmount*2.0) + (sin(inputSample)*(1.0-(dynAmount*2.0)));		
		if (dynAmount > 0.5001) inputSample = (asin(inputSample)*((dynAmount*2.0)-1.0)) + (inputSample*(1.0-((dynAmount*2.0)-1.0)));
		//doing this in two steps means I get to not run an extra sin/asin function per sample
		
		noiseB = sin(noiseA*(0.2-(dynAmount*0.125))*fabs(inputSample));
		
		double slew = fabs(inputSample-lastSample)*slewAmount;
		lastSample = inputSample*(0.86-(dynAmount*0.125));
		
		if (slew > 1.0) slew = 1.0;
		double iirIntensity = slew;
		iirIntensity *= 2.472;
		iirIntensity *= iirIntensity;
		if (iirIntensity > 1.0) iirIntensity = 1.0;
		
		iirSample = (iirSample * (1.0 - iirIntensity)) + (noiseB * iirIntensity);
		noiseB = iirSample;
		noiseB = (noiseB * slew) + (noiseC * (1.0-slew));
		noiseC = noiseB;
		
		inputSample = (noiseC * wet) + (drySample * (1.0-wet));
		
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
	polarity = false;
	lastSample = 0.0;
	iirSample = 0.0;
	noiseA = 0.0;
	noiseB = 0.0;
	noiseC = 0.0;
	flip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
