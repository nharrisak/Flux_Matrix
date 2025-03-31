#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Gatelinked"
#define AIRWINDOWS_DESCRIPTION "Gatelinked"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','t' )
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
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble Sustain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Sustain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack Speed", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
		Float64 iirLowpassAR;
		Float64 iirLowpassBR;
		Float64 iirHighpassAR;
		Float64 iirHighpassBR;
		Float64 iirLowpassAL;
		Float64 iirLowpassBL;
		Float64 iirHighpassAL;
		Float64 iirHighpassBL;
		Float64 treblefreq;
		Float64 bassfreq;
		bool flip;
	uint32_t fpdL;
	uint32_t fpdR;

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	//speed settings around release
	Float64 threshold = pow(GetParameter( kParam_One ),2);
	//gain settings around threshold
	Float64 trebledecay = pow(1.0-GetParameter( kParam_Two ),2)/4196.0;
	Float64 bassdecay =  pow(1.0-GetParameter( kParam_Three ),2)/8192.0;
	Float64 slowAttack = (pow(GetParameter( kParam_Four ),3)*3)+0.003;
	Float64 wet = GetParameter( kParam_Five );
	slowAttack /= overallscale;
	trebledecay /= overallscale;
	bassdecay /= overallscale;
	trebledecay += 1.0;
	bassdecay += 1.0;
	Float64 attackSpeed;
	Float64 highestSample;

	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		if (fabs(inputSampleL) > fabs(inputSampleR)) {
			attackSpeed = slowAttack - (fabs(inputSampleL)*slowAttack*0.5);
			highestSample = fabs(inputSampleL);
		} else {
			attackSpeed = slowAttack - (fabs(inputSampleR)*slowAttack*0.5); //we're triggering off the highest amplitude
			highestSample = fabs(inputSampleR); //and making highestSample the abs() of that amplitude
		}
		
		if (attackSpeed < 0.0) attackSpeed = 0.0;
		//softening onset click depending on how hard we're getting it
		
		if (flip)
		{
			if (highestSample > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0) treblefreq = 2.0;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0) bassfreq = 0.0;
				iirLowpassAL = iirLowpassBL = inputSampleL;
				iirHighpassAL = iirHighpassBL = 0.0;
				iirLowpassAR = iirLowpassBR = inputSampleR;
				iirHighpassAR = iirHighpassBR = 0.0;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0) {
				iirLowpassAL = inputSampleL;
				iirLowpassAR = inputSampleR;
			} else {
				iirLowpassAL = (iirLowpassAL * (1.0 - treblefreq)) + (inputSampleL * treblefreq);
				iirLowpassAR = (iirLowpassAR * (1.0 - treblefreq)) + (inputSampleR * treblefreq);
			}
			
			if (bassfreq > 1.0) bassfreq = 1.0;
			
			if (bassfreq > 0.0) {
				iirHighpassAL = (iirHighpassAL * (1.0 - bassfreq)) + (inputSampleL * bassfreq);
				iirHighpassAR = (iirHighpassAR * (1.0 - bassfreq)) + (inputSampleR * bassfreq);
			} else {
				iirHighpassAL = 0.0;
				iirHighpassAR = 0.0;
			}
			
			if (treblefreq > bassfreq) {
				inputSampleL = (iirLowpassAL - iirHighpassAL);
				inputSampleR = (iirLowpassAR - iirHighpassAR);
			} else {
				inputSampleL = 0.0;
				inputSampleR = 0.0;
			}
		}
		else
		{
			if (highestSample > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0) treblefreq = 2.0;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0) bassfreq = 0.0;
				iirLowpassAL = iirLowpassBL = inputSampleL;
				iirHighpassAL = iirHighpassBL = 0.0;
				iirLowpassAR = iirLowpassBR = inputSampleR;
				iirHighpassAR = iirHighpassBR = 0.0;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0) {
				iirLowpassBL = inputSampleL;
				iirLowpassBR = inputSampleR;
			} else {
				iirLowpassBL = (iirLowpassBL * (1.0 - treblefreq)) + (inputSampleL * treblefreq);
				iirLowpassBR = (iirLowpassBR * (1.0 - treblefreq)) + (inputSampleR * treblefreq);
			}
			
			if (bassfreq > 1.0) bassfreq = 1.0;

			if (bassfreq > 0.0) {
				iirHighpassBL = (iirHighpassBL * (1.0 - bassfreq)) + (inputSampleL * bassfreq);
				iirHighpassBR = (iirHighpassBR * (1.0 - bassfreq)) + (inputSampleR * bassfreq);
			} else {
				iirHighpassBL = 0.0;
				iirHighpassBR = 0.0;
			}
			
			if (treblefreq > bassfreq) {
				inputSampleL = (iirLowpassBL - iirHighpassBL);
				inputSampleR = (iirLowpassBR - iirHighpassBR);
			} else {
				inputSampleL = 0.0;
				inputSampleR = 0.0;
			}			
		}
		//done full gated envelope filtered effect
		inputSampleL  = ((1-wet)*drySampleL)+(wet*inputSampleL);
		inputSampleR  = ((1-wet)*drySampleR)+(wet*inputSampleR);
		//we're going to set up a dry/wet control instead of a min. threshold
		
		flip = !flip;
				
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
	iirLowpassAL = 0.0;
	iirLowpassBL = 0.0;
	iirHighpassAL = 0.0;
	iirHighpassBL = 0.0;
	iirLowpassAR = 0.0;
	iirLowpassBR = 0.0;
	iirHighpassAR = 0.0;
	iirHighpassBR = 0.0;
	treblefreq = 1.0;
	bassfreq = 0.0;
	flip = false;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
