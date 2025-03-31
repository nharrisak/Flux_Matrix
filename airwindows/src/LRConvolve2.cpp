#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "LRConvolve2"
#define AIRWINDOWS_DESCRIPTION "Multiplies each channel by the other, plus Soar!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','R','D' )
enum {

	kParam_A =0,
	kNumberOfParameters=1
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Soar", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"

	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	double soar = 0.3-(GetParameter( kParam_A )*0.3);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;		
		
		//blame Jannik Asfaig (BoyXx76) for this (and me) :D
		double out = 0.0;
		double inL = fabs(inputSampleL)+(soar*soar);
		double inR = fabs(inputSampleR)+(soar*soar);		
		if (inputSampleL > 0.0 && inputSampleR > 0.0) out = fmax((sqrt(inR/inL)*inL)-soar,0.0);
		if (inputSampleL < 0.0 && inputSampleR > 0.0) out = fmin((-sqrt(inR/inL)*inL)+soar,0.0);
		if (inputSampleL > 0.0 && inputSampleR < 0.0) out = fmin((-sqrt(inR/inL)*inL)+soar,0.0);
		if (inputSampleL < 0.0 && inputSampleR < 0.0) out = fmax((sqrt(inR/inL)*inL)-soar,0.0);
		inputSampleL = inputSampleR = out;
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
