#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pear"
#define AIRWINDOWS_DESCRIPTION "The testbed for a new filter, implemented as a highpass/lowpass shelf."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','e','b' )
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
{ .name = "bitFreq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
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
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	double pear[pear_total]; //probably worth just using a number here
	
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
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 3) cycleEnd = 3;
	cycleEnd--; //this is going to be 0 for 44.1 or 48k, 1 for 88.2 or 96k, 2 for 176 or 192k.
	//DIFFERENT! Offsetting the bt shift
	
	int bitshiftFreq = (10-(GetParameter( kParam_One )*10.0))+cycleEnd;
	double freq = 1.0;
	switch (bitshiftFreq)
	{
		case 16: freq = 0.0000152587890625; break;
		case 15: freq = 0.000030517578125; break;
		case 14: freq = 0.00006103515625; break;
		case 13: freq = 0.0001220703125; break;
		case 12: freq = 0.000244140625; break;
		case 11: freq = 0.00048828125; break;
		case 10: freq = 0.0009765625; break;
		case 9: freq = 0.001953125; break;
		case 8: freq = 0.00390625; break;
		case 7: freq = 0.0078125; break;
		case 6: freq = 0.015625; break;
		case 5: freq = 0.03125; break;
		case 4: freq = 0.0625; break;
		case 3: freq = 0.125; break;
		case 2: freq = 0.25; break;
		case 1: freq = 0.5; break;
		case 0: freq = 1.0; break;
	}
	double maxPoles = GetParameter( kParam_Two )*pear_total;
	double wet = (GetParameter( kParam_Three )*2.0)-1.0; //inv-dry-wet for highpass
	double dry = 2.0-(GetParameter( kParam_Three )*2.0);
	if (dry > 1.0) dry = 1.0; //full dry for use with inv, to 0.0 at full wet
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		//Pear is a variation on a Holt filter, made to act like my biquad filters:
		//its variables (L and R prevSample and prevSlew) contained in arrays for easier handling.
		//On top of that, the array is used to facilitate use of the filter in a stack which will
		//sharpen its very weak frequency response performance (3dB/oct per pole) and allow it to run
		//in a loop. Many earlier Airwindows filters did this 'unrolled', often with variables given
		//lettered suffixes like iirA through iirZ. The use of an array will make this a lot tidier.
		
		for (int x = 0; x < maxPoles; x += 4) {
			double slew = ((inputSampleL - pear[x]) + pear[x+1])*freq*0.5;
			pear[x] = inputSampleL = (freq * inputSampleL) + ((1.0-freq) * (pear[x] + pear[x+1]));
			pear[x+1] = slew;
			slew = ((inputSampleR - pear[x+2]) + pear[x+3])*freq*0.5;
			pear[x+2] = inputSampleR = (freq * inputSampleR) + ((1.0-freq) * (pear[x+2] + pear[x+3]));
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
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
