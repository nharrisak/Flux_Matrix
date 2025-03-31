#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Channel8"
#define AIRWINDOWS_DESCRIPTION "Channel7 with updates from Capacitor2 and Slew3. More and better of what Channel is."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','g' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	kParam_Three = 2,
	//Add your parameters here...
	kNumberOfParameters=3
};
static const int kNeve = 1;
static const int kAPI = 2;
static const int kSSL = 3;
static const int kDefaultValue_ParamOne = kNeve;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static char const * const enumStrings0[] = { "", "Neve", "API", "SSL", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Console Type", .min = 1, .max = 3, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Drive", .min = 0, .max = 20000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		uint32_t fpd;
		double iirSampleA;
		double iirSampleB;
		bool flip;
		double lastSampleA;
		double lastSampleB;
		double lastSampleC;
	
	struct _dram {
		};
	_dram* dram;
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
	Float64 density = GetParameter( kParam_Two )/100.0; //0-2
	Float64 phattity = density - 1.0;
	if (density > 1.0) density = 1.0; //max out at full wet for Spiral aspect
	if (phattity < 0.0) phattity = 0.0; //
	Float64 nonLin = 5.0-density; //number is smaller for more intense, larger for more subtle
	Float64 output = GetParameter( kParam_Three );
	Float64 iirAmount = 0.005832;
	Float64 threshold = 0.33362176;	
	switch (console)
	{
		case 1: iirAmount = 0.005832; threshold = 0.33362176; break; //Neve
		case 2: iirAmount = 0.004096; threshold = 0.59969536; break; //API
		case 3: iirAmount = 0.004913; threshold = 0.84934656; break; //SSL
	}
	iirAmount /= overallscale; //we've learned not to try and adjust threshold for sample rate
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		Float64 dielectricScale = fabs(2.0-((inputSample+nonLin)/nonLin));
		
		if (flip)
		{
			iirSampleA = (iirSampleA * (1.0 - (iirAmount * dielectricScale))) + (inputSample * iirAmount * dielectricScale);
			inputSample = inputSample - iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1.0 - (iirAmount * dielectricScale))) + (inputSample * iirAmount * dielectricScale);
			inputSample = inputSample - iirSampleB;
		}
		//highpass section including capacitor modeling nonlinearity
		double drySample = inputSample;
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		
		double phatSample = sin(inputSample * 1.57079633);
		inputSample *= 1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
		double distSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0) ?1:fabs(inputSample));
		
		inputSample = distSample; //purest form is full Spiral
		if (density < 1.0) inputSample = (drySample*(1-density))+(distSample*density); //fade Spiral aspect
		if (phattity > 0.0) inputSample = (inputSample*(1-phattity))+(phatSample*phattity); //apply original Density on top
		
		Float64 clamp = (lastSampleB - lastSampleC) * 0.381966011250105;
		clamp -= (lastSampleA - lastSampleB) * 0.6180339887498948482045;
		clamp += inputSample - lastSampleA; //regular slew clamping added

		lastSampleC = lastSampleB;
		lastSampleB = lastSampleA;
		lastSampleA = inputSample; //now our output relates off lastSampleB
		
		if (clamp > threshold)
			inputSample = lastSampleB + threshold;
		if (-clamp > threshold)
			inputSample = lastSampleB - threshold;
		//slew section		
		lastSampleA = (lastSampleA*0.381966011250105)+(inputSample*0.6180339887498948482045); //split the difference between raw and smoothed for buffer
				
		flip = !flip;
		
		if (output < 1.0)
		{
			inputSample *= output;
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	iirSampleA = iirSampleB = 0.0;
	flip = false;
	lastSampleA = lastSampleB = lastSampleC = 0.0;
}
};
