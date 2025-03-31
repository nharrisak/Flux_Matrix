#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Overheads"
#define AIRWINDOWS_DESCRIPTION "For compressing only part of the sound, strangely!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','O','v','e' )
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
{ .name = "Compr", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sharp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double ovhGain;
	int ovhCount;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double ovhL[130];
	double ovhR[130];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double ovhTrim = pow(GetParameter( kParam_One ),3);
	ovhTrim += 1.0; ovhTrim *= ovhTrim;
	int offset = (pow(GetParameter( kParam_Two ),7) * 16.0 * overallscale)+1;
	double wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		//begin Overheads compressor
		inputSampleL *= ovhTrim; inputSampleR *= ovhTrim;
		ovhCount--; if (ovhCount < 0 || ovhCount > 128) ovhCount = 128; dram->ovhL[ovhCount] = inputSampleL; dram->ovhR[ovhCount] = inputSampleR;
		double ovhClamp = sin(fabs(inputSampleL - dram->ovhL[(ovhCount+offset)-(((ovhCount+offset) > 128)?129:0)])*(ovhTrim-1.0)*64.0);
		ovhGain *= (1.0 - ovhClamp); ovhGain += ((1.0-ovhClamp) * ovhClamp);
		ovhClamp = sin(fabs(inputSampleR - dram->ovhR[(ovhCount+offset)-(((ovhCount+offset) > 128)?129:0)])*(ovhTrim-1.0)*64.0);
		ovhGain *= (1.0 - ovhClamp); ovhGain += ((1.0-ovhClamp) * ovhClamp);
		if (ovhGain > 1.0) ovhGain = 1.0; if (ovhGain < 0.0) ovhGain = 0.0;
		inputSampleL *= ovhGain; inputSampleR *= ovhGain;
		//end Overheads compressor
		
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
	ovhGain = 1.0;
	for(int count = 0; count < 129; count++) {dram->ovhL[count] = 0.0; dram->ovhR[count] = 0.0;}
	ovhCount = 0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
