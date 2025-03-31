#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "RingModulator"
#define AIRWINDOWS_DESCRIPTION "Repitches sounds mathematically, not harmonically."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','i','n' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Soar", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"

	double sinePosL;
	double sinePosR;
	double incLA;
	double incLB;
	double incRA;
	double incRB;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();

	incLA = incLB; incLB = pow(GetParameter( kParam_A ),5)/overallscale;
	incRA = incRB; incRB = pow(GetParameter( kParam_B ),5)/overallscale;
	double soar = 0.3-(GetParameter( kParam_C )*0.3);
	double wet = pow(GetParameter( kParam_D ),2);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double incL = (incLA*temp)+(incLB*(1.0-temp));
		double incR = (incRA*temp)+(incRB*(1.0-temp));

		sinePosL += incL;
		if (sinePosL > 6.283185307179586) sinePosL -= 6.283185307179586;
		double sinResultL = sin(sinePosL);
		sinePosR += incR;
		if (sinePosR > 6.283185307179586) sinePosR -= 6.283185307179586;
		double sinResultR = sin(sinePosR);
		
		double out = 0.0;
		double snM = fabs(sinResultL)+(soar*soar);
		double inM = fabs(inputSampleL);
		if (inM < snM) {
			inM = fabs(sinResultL);
			snM = fabs(inputSampleL)+(soar*soar);
		}
		
		if (inputSampleL > 0.0 && sinResultL > 0.0) out = fmax((sqrt(inM/snM)*snM)-soar,0.0);
		if (inputSampleL < 0.0 && sinResultL > 0.0) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0);
		if (inputSampleL > 0.0 && sinResultL < 0.0) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0);
		if (inputSampleL < 0.0 && sinResultL < 0.0) out = fmax((sqrt(inM/snM)*snM)-soar,0.0);
		inputSampleL = out;
		
		out = 0.0;
		snM = fabs(sinResultR)+(soar*soar);
		inM = fabs(inputSampleR);
		if (inM < snM) {
			inM = fabs(sinResultR);
			snM = fabs(inputSampleR)+(soar*soar);
		}
		
		if (inputSampleR > 0.0 && sinResultR > 0.0) out = fmax((sqrt(inM/snM)*snM)-soar,0.0);
		if (inputSampleR < 0.0 && sinResultR > 0.0) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0);
		if (inputSampleR > 0.0 && sinResultR < 0.0) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0);
		if (inputSampleR < 0.0 && sinResultR < 0.0) out = fmax((sqrt(inM/snM)*snM)-soar,0.0);
		inputSampleR = out;
		
		if (wet != 1.0) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider

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
	sinePosL = 0.0;
	sinePosR = 0.0;
	incLA = 0.0; incLB = 0.0;
	incRA = 0.0; incRB = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
