#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MidSide"
#define AIRWINDOWS_DESCRIPTION "Mid/side conversion utility plugins."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','i','e' )
enum {

	kParam_One =0,
	//Add your parameters here...
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
{ .name = "Mid/Side", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
	
#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
		
	double inputSampleL;
	double inputSampleR;
	double mid;
	double side;
	
	Float64 sidegain = GetParameter( kParam_One ) * 2.0;
	Float64 midgain = 2.0 - sidegain;
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		//assign working variables
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;

		mid *= midgain;
		side *= sidegain;
		
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)mid, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		mid += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		frexpf((float)side, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		side += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*outputL = mid;
		*outputR = side;
		
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
