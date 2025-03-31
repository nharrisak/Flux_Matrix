#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Balanced"
#define AIRWINDOWS_DESCRIPTION "Lets you run an XLR input into a stereo input, sum it to mono, and use it."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','l' )
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
{ .name = "Bit Shift", .min = 0, .max = 8000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
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
	
	int bitshiftGain = GetParameter( kParam_One );
	double gain = 1.0; //default unity gain
	switch (bitshiftGain)
	{
		case 0: gain = 0.5; break;
		case 1: gain = 1.0; break;
		case 2: gain = 2.0; break;
		case 3: gain = 4.0; break;
		case 4: gain = 8.0; break;
		case 5: gain = 16.0; break;
		case 6: gain = 32.0; break;
		case 7: gain = 64.0; break;
		case 8: gain = 128.0; break;
	}
	//we are directly punching in the gain values rather than calculating them
	//unlike regular BitShiftGain, we default to 0.5 for unity gain, and so on
	//because we're combining two channels.
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double side = inputSampleL - inputSampleR;
		//tip is left, to add negative ring (right) to combine 'em is the same as subtracting them
		//end result is, mono output is made up of half of each balanced input combined. Note that we don't just
		//flip the ring input, because we need to combine them to cancel out interference.

		inputSampleL = side*gain;
		inputSampleR = side*gain;
		//assign mono as result of balancing of channels

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
