#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SoftGate"
#define AIRWINDOWS_DESCRIPTION "A gate that can mute hiss and smooth sample tails."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','o','g' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Bright Threshold", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darken Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Base Silence", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	Float64 storedL[2];
	Float64 diffL;
	Float64 storedR[2];
	Float64 diffR;
	Float64 gate;
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	Float64 threshold = pow(GetParameter( kParam_One ),6);
	Float64 recovery = pow((GetParameter( kParam_Two )*0.5),6);
	recovery /= overallscale;
	Float64 baseline = pow(GetParameter( kParam_Three ),6);
	Float64 invrec = 1.0 - recovery;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		storedL[1] = storedL[0];
		storedL[0] = inputSampleL;
		diffL = storedL[0] - storedL[1];
		
		storedR[1] = storedR[0];
		storedR[0] = inputSampleR;
		diffR = storedR[0] - storedR[1];
		
		if (gate > 0) {gate = ((gate-baseline) * invrec) + baseline;}
		
		if ((fabs(diffR) > threshold) || (fabs(diffL) > threshold)) {gate = 1.1;}
		else {gate = (gate * invrec); if (threshold > 0) {gate += ((fabs(inputSampleL)/threshold) * recovery);gate += ((fabs(inputSampleR)/threshold) * recovery);}}
		
		if (gate < 0) gate = 0;
		
		if (gate < 1.0)
		{
			storedL[0] = storedL[1] + (diffL * gate);		
			storedR[0] = storedR[1] + (diffR * gate);
		}
		
		if (gate < 1) {
			inputSampleL = (inputSampleL * gate) + (storedL[0] * (1.0-gate));
			inputSampleR = (inputSampleR * gate) + (storedR[0] * (1.0-gate));
		}
		
		
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
	storedL[0] = storedL[1] = 0.0;
	diffL = 0.0;
	storedR[0] = storedR[1] = 0.0;
	diffR = 0.0;
	gate = 1.1;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
