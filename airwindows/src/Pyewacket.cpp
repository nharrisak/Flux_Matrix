#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pyewacket"
#define AIRWINDOWS_DESCRIPTION "An old school compressor for high definition transients. Adds no fatness, just energy."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','y','e' )
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
{ .name = "Input Gain", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Gain", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 

	uint32_t fpdL;
	uint32_t fpdR;
	Float64 chase;
	Float64 lastrectifierL;
	Float64 lastrectifierR;
	
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	if (overallscale < 0.1) overallscale = 1.0;
	//insanity check
	double fpOld = 0.618033988749894848204586; //golden ratio!
	double fpNew = 1.0 - fpOld;	
	double inputSampleL;
	double inputSampleR;
	double drySampleL;
	double drySampleR;
	Float64 bridgerectifier;
	Float64 temprectifier;
	Float64 inputSense;
	
	Float64 inputGain = pow(10.0,(GetParameter( kParam_One ))/20.0);
	Float64 attack = ((GetParameter( kParam_Two )+0.5)*0.006)/overallscale;
	Float64 decay = ((GetParameter( kParam_Two )+0.01)*0.0004)/overallscale;
	Float64 outputGain = pow(10.0,(GetParameter( kParam_Three ))/20.0);
	Float64 wet;
	Float64 maxblur;
	Float64 blurdry;
	Float64 out;
	Float64 dry;
	
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		
		if (inputGain != 1.0) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		//we will take the greater of either channel and just use that, then apply the result
		//to both stereo channels.
		if (chase < inputSense) chase += attack;
		if (chase > 1.0) chase = 1.0;
		if (chase > inputSense) chase -= decay;
		if (chase < 0.0) chase = 0.0;
		//chase will be between 0 and ? (if input is super hot)
		out = wet = chase;
		if (wet > 1.0) wet = 1.0;
		maxblur = wet * fpNew;
		blurdry = 1.0 - maxblur;
		//scaled back so that blur remains balance of both
		if (out > fpOld) out = fpOld - (out - fpOld);
		if (out < 0.0) out = 0.0;
		dry = 1.0 - wet;
		
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;

		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		temprectifier = 1-cos(bridgerectifier);
		bridgerectifier = ((lastrectifierL*maxblur) + (temprectifier*blurdry));
		lastrectifierL = temprectifier;
		//starved version is also blurred by one sample
		if (inputSampleL > 0) inputSampleL = (inputSampleL*dry)+(bridgerectifier*out);
		else inputSampleL = (inputSampleL*dry)-(bridgerectifier*out);
		
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		temprectifier = 1-cos(bridgerectifier);
		bridgerectifier = ((lastrectifierR*maxblur) + (temprectifier*blurdry));
		lastrectifierR = temprectifier;
		//starved version is also blurred by one sample
		if (inputSampleR > 0) inputSampleR = (inputSampleR*dry)+(bridgerectifier*out);
		else inputSampleR = (inputSampleR*dry)-(bridgerectifier*out);
		
		if (outputGain != 1.0) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
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
	chase = 1.0;
	lastrectifierL = 0.0;
	lastrectifierR = 0.0;
	return noErr;
}

};
