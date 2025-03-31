#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TPDFWide"
#define AIRWINDOWS_DESCRIPTION "Still TPDF dither but better and wider. With 16/24 bit output and a DeRez control."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','P','E' )
enum {

	kParam_One = 0,
	kParam_Two = 1,
	//Add your parameters here...
	kNumberOfParameters=2
};
static const int kCD = 0;
static const int kHD = 1;
static const int kDefaultValue_ParamOne = kHD;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, };
static char const * const enumStrings0[] = { "CD 16 bit", "HD 24 bit", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Quantizer", .min = 0, .max = 1, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
		uint32_t fpdL;
		uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	bool highres = false;
	if (GetParameter( kParam_One ) == 1) highres = true;
	Float32 scaleFactor;
	if (highres) scaleFactor = 8388608.0;
	else scaleFactor = 32768.0;
	Float32 derez = GetParameter( kParam_Two );
	if (derez > 0.0) scaleFactor *= pow(1.0-derez,6);
	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	Float32 outScale = scaleFactor;
	if (outScale < 8.0) outScale = 8.0;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;

		inputSampleL *= scaleFactor;
		inputSampleR *= scaleFactor;
		//0-1 is now one bit, now we dither
		
		double ditherL = -1.0;
		ditherL += (double(fpdL)/UINT32_MAX);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		ditherL += (double(fpdL)/UINT32_MAX);
		//TPDF: two 0-1 random noises
		
		double ditherR = -1.0;
		ditherR += (double(fpdR)/UINT32_MAX);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		ditherR += (double(fpdR)/UINT32_MAX);
		//TPDF: two 0-1 random noises
		
		if (fabs(ditherL-ditherR) < 0.5) {
			ditherL = -1.0;
			ditherL += (double(fpdL)/UINT32_MAX);
			fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
			ditherL += (double(fpdL)/UINT32_MAX);
		}
		
		if (fabs(ditherL-ditherR) < 0.5) {
			ditherR = -1.0;
			ditherR += (double(fpdR)/UINT32_MAX);
			fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
			ditherR += (double(fpdR)/UINT32_MAX);
		}
		
		if (fabs(ditherL-ditherR) < 0.5) {
			ditherL = -1.0;
			ditherL += (double(fpdL)/UINT32_MAX);
			fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
			ditherL += (double(fpdL)/UINT32_MAX);
		}
				
		inputSampleL = floor(inputSampleL+ditherL);
		inputSampleR = floor(inputSampleR+ditherR);
		
		inputSampleL /= outScale;
		inputSampleR /= outScale;

		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		//pseudorandom number updater

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
