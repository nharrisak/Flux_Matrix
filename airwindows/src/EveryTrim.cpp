#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "EveryTrim"
#define AIRWINDOWS_DESCRIPTION "Left/Right, Mid/Side, and Master in one plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','v','g' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Left", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Right", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Side", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Master", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	uint32_t fpdL;
	uint32_t fpdR;
	
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
	
	Float64 leftgain = pow(10.0,GetParameter( kParam_One )/20.0);
	Float64 rightgain = pow(10.0,GetParameter( kParam_Two )/20.0);
	Float64 midgain = pow(10.0,GetParameter( kParam_Three )/20.0);
	Float64 sidegain = pow(10.0,GetParameter( kParam_Four )/20.0);
	Float64 mastergain = pow(10.0,GetParameter( kParam_Five )/20.0) * 0.5;
	
	leftgain *= mastergain;
	rightgain *= mastergain;
	
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
		inputSampleL = (mid+side) * leftgain;
		inputSampleR = (mid-side) * rightgain;
		//contains mastergain and the gain trim fixing the mid/side
		
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
