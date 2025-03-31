#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Golem"
#define AIRWINDOWS_DESCRIPTION "Lets you blend a stereo track of two mics on an amp."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','o','m' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
static const int kIn = 1;
static const int kOut = 2;
static const int kBassIn = 3;
static const int kBassOut = 4;
static const int kTiming = 5;
static const int kBoth = 6;
static const int kDefaultValue_ParamThree = kIn;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, };
static char const * const enumStrings2[] = { "", "Reinforcement- Two Mics Range", "Cancellation- Two Mics Range", "Reinforcement- Amp/DI Range", "Cancellation- Amp/DI Range", "Defeat Timing Adjustment", "Defeat Timing & Gain", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Balance", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Offset", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Phase", .min = 1, .max = 6, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings2 },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
    
	Float64 p[4099];
	int count;
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	int phase = (int) GetParameter( kParam_Three );
	Float64 balance = GetParameter( kParam_One ) / 2.0;
	Float64 gainL = 0.5 - balance;
	Float64 gainR = 0.5 + balance;
	Float64 range = 30.0;
	if (phase == 3) range = 700.0;
	if (phase == 4) range = 700.0;
	Float64 offset = pow(GetParameter( kParam_Two ),5) * range;
	if (phase > 4) offset = 0.0;
	if (phase > 5)
		{
			gainL = 0.5;
			gainR = 0.5;
		}
	int near = (int)floor(fabs(offset));
	Float64 farLevel = fabs(offset) - near;
	int far = near + 1;
	Float64 nearLevel = 1.0 - farLevel;
	
	double inputSampleL;
	double inputSampleR;
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		//assign working variables

		if (phase == 2) inputSampleL = -inputSampleL;
		if (phase == 4) inputSampleL = -inputSampleL;

		inputSampleL *= gainL;
		inputSampleR *= gainR;

		if (count < 1 || count > 2048) {count = 2048;}
		
		if (offset > 0)
			{
				p[count+2048] = p[count] = inputSampleL;
				inputSampleL = p[count+near]*nearLevel;
				inputSampleL += p[count+far]*farLevel;
				
				//consider adding third sample just to bring out superhighs subtly, like old interpolation hacks
				//or third and fifth samples, ditto
				
			}
		
		if (offset < 0)
			{
				p[count+2048] = p[count] = inputSampleR;
				inputSampleR = p[count+near]*nearLevel;
				inputSampleR += p[count+far]*farLevel;
			}

		count -= 1;
		
		inputSampleL = inputSampleL + inputSampleR;
		inputSampleR = inputSampleL;
		//the output is totally mono

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
		//We have pointers to increment- every actual value reference has to be to a *value
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
};
int _airwindowsAlgorithm::reset(void) {

{
	for(count = 0; count < 4098; count++) {p[count] = 0.0;}
	count = 0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	
	return noErr;
}

};
