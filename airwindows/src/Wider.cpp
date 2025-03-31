#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Wider"
#define AIRWINDOWS_DESCRIPTION "Airwindows stereo space shaping."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','W','i','d' )
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
{ .name = "Width", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Center", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	uint32_t fpdL;
	uint32_t fpdR;
	int count;
#include "../include/template2.h"
struct _dram {
	Float64 p[4099];
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	double inputSampleL;
	double inputSampleR;
	Float64 drySampleL;
	Float64 drySampleR;
	double mid;
	double side;
	Float64 out;
	Float64 densityside = GetParameter( kParam_One );
	Float64 densitymid = GetParameter( kParam_Two );
	Float64 wet = GetParameter( kParam_Three );
	//removed unnecessary dry variable
	wet *= 0.5; //we make mid-side by adding/subtracting both channels into each channel
	//and that's why we gotta divide it by 2: otherwise everything's doubled. So, premultiply it to save an extra 'math'
	Float64 offset = (densityside-densitymid)/2;
	if (offset > 0) offset = sin(offset);
	if (offset < 0) offset = -sin(-offset);
	offset = -(pow(offset,4) * 20 * overallscale);
	int near = (int)floor(fabs(offset));
	Float64 farLevel = fabs(offset) - near;
	int far = near + 1;
	Float64 nearLevel = 1.0 - farLevel;
	Float64 bridgerectifier;
	//interpolating the sample
	
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		//assign working variables		
		mid = inputSampleL + inputSampleR;
		side = inputSampleL - inputSampleR;
		//assign mid and side. Now, High Impact code
		
		if (densityside != 0.0)
		{
			out = fabs(densityside);
			bridgerectifier = fabs(side)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			//max value for sine function
			if (densityside > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (side > 0) side = (side*(1-out))+(bridgerectifier*out);
			else side = (side*(1-out))-(bridgerectifier*out);
			//blend according to density control
		}
		
		if (densitymid != 0.0)
		{
			out = fabs(densitymid);
			bridgerectifier = fabs(mid)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			//max value for sine function
			if (densitymid > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (mid > 0) mid = (mid*(1-out))+(bridgerectifier*out);
			else mid = (mid*(1-out))-(bridgerectifier*out);
			//blend according to density control
		}
		
		if (count < 1 || count > 2048) {count = 2048;}
		if (offset > 0)
		{
			dram->p[count+2048] = dram->p[count] = mid;
			mid = dram->p[count+near]*nearLevel;
			mid += dram->p[count+far]*farLevel;
		}
		
		if (offset < 0)
		{
			dram->p[count+2048] = dram->p[count] = side;
			side = dram->p[count+near]*nearLevel;
			side += dram->p[count+far]*farLevel;
		}
		count -= 1;
		
		inputSampleL = (drySampleL * (1.0-wet)) + ((mid+side) * wet);
		inputSampleR = (drySampleR * (1.0-wet)) + ((mid-side) * wet);
		
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
	for(int fcount = 0; fcount < 4098; fcount++) {dram->p[fcount] = 0.0;}
	count = 0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}


};
