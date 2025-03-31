#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ContentHideD"
#define AIRWINDOWS_DESCRIPTION "Mutes audio except for brief glimpses."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','9' )
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
};
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"

	double demotimer;
	uint32_t fpd;

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
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		
		double duck = 0.0;
		demotimer -= (1.0/overallscale);
		
		if (demotimer < 441.0) duck = 1.0 - (demotimer/441.0);
		if (demotimer > 44100.0) duck = ((demotimer-44100.0)/441.0);
		if (duck > 1.0) duck = 1.0; duck = sin((1.0-duck)*1.57);
		
		if ((demotimer < 1) || (demotimer > 441000.0)) {
			double randy = (double(fpd)/(double)UINT32_MAX); //0 to 1 the noise, may not be needed		
			demotimer = 100000.0*(randy+2.0);
		}
		
		inputSampleL *= duck;
		inputSampleR *= duck;
		
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		
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
	demotimer = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	return noErr;
}

};
