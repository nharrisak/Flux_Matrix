#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pear2"
#define AIRWINDOWS_DESCRIPTION "My Pear filter plus nonlinearity."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','e','c' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "NonLin", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		prevSampL2,
		prevSlewL2,
		prevSampR2,
		prevSlewR2,
		prevSampL3,
		prevSlewL3,
		prevSampR3,
		prevSlewR3,
		prevSampL4,
		prevSlewL4,
		prevSampR4,
		prevSlewR4,
		prevSampL5,
		prevSlewL5,
		prevSampR5,
		prevSlewR5,
		prevSampL6,
		prevSlewL6,
		prevSampR6,
		prevSlewR6,
		prevSampL7,
		prevSlewL7,
		prevSampR7,
		prevSlewR7,
		prevSampL8,
		prevSlewL8,
		prevSampR8,
		prevSlewR8,
		prevSampL9,
		prevSlewL9,
		prevSampR9,
		prevSlewR9,
		prevSampL10,
		prevSlewL10,
		prevSampR10,
		prevSlewR10,
		prevSampL11,
		prevSlewL11,
		prevSampR11,
		prevSlewR11,
		prevSampL12,
		prevSlewL12,
		prevSampR12,
		prevSlewR12,
		prevSampL13,
		prevSlewL13,
		prevSampR13,
		prevSlewR13,
		prevSampL14,
		prevSlewL14,
		prevSampR14,
		prevSlewR14,
		prevSampL15,
		prevSlewL15,
		prevSampR15,
		prevSlewR15,
		prevSampL16,
		prevSlewL16,
		prevSampR16,
		prevSlewR16,
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	double pear[pear_total]; //probably worth just using a number here
	
	double freqA;
	double freqB;
	double nonLinA;
	double nonLinB;
	double wetA;
	double wetB;
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();

	freqA = freqB;
	freqB = pow(GetParameter( kParam_One ),2);
	if (freqB < 0.0001) freqB = 0.0001;
	nonLinA = nonLinB;
	nonLinB = pow(GetParameter( kParam_Two ),2);
	double maxPoles = pow(GetParameter( kParam_Three ),3)*pear_total;
	wetA = wetB;
	wetB = GetParameter( kParam_Four ); //inv-dry-wet for highpass
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double freq = (freqA*temp)+(freqB*(1.0-temp));
		double nonLin = (nonLinA*temp)+(nonLinB*(1.0-temp));
		double wet = (wetA*temp)+(wetB*(1.0-temp));
		double dry = 2.0-(wet*2.0);
		if (dry > 1.0) dry = 1.0; //full dry for use with inv, to 0.0 at full wet
		wet = (wet*2.0)-1.0; //inv-dry-wet for highpass
						
		for (int x = 0; x < maxPoles; x += 4) {
			double di = fabs(freq*(1.0+(inputSampleL*nonLin))); if (di > 1.0) di = 1.0;
			double slew = ((inputSampleL - pear[x]) + pear[x+1])*di*0.5;
			pear[x] = inputSampleL = (di * inputSampleL) + ((1.0-di) * (pear[x] + pear[x+1]));
			pear[x+1] = slew;
			di = fabs(freq*(1.0+(inputSampleR*nonLin))); if (di > 1.0) di = 1.0;
			slew = ((inputSampleR - pear[x+2]) + pear[x+3])*di*0.5;
			pear[x+2] = inputSampleR = (di * inputSampleR) + ((1.0-di) * (pear[x+2] + pear[x+3]));
			pear[x+3] = slew;
		}
		
		inputSampleL *= wet;
		inputSampleR *= wet;
		drySampleL *= dry;
		drySampleR *= dry;
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		
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
	for (int x = 0; x < pear_total; x++) pear[x] = 0.0;
	freqA = freqB = 0.5;
	nonLinA = nonLinB = 0.0;
	wetA = wetB = 0.5;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
