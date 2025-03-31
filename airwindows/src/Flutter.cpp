#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Flutter"
#define AIRWINDOWS_DESCRIPTION "The flutter from ToTape6, standalone."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','l','u' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Flutter", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	int gcount;		
	double rateof;
	double sweep;
	double nextmax;
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
struct _dram {
	double dL[1002];
	double dR[1002];
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double depth = pow(GetParameter( kParam_One ),2)*overallscale*70;
	double fluttertrim = (0.0024*pow(GetParameter( kParam_One ),2))/overallscale;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		if (gcount < 0 || gcount > 999) gcount = 999;
		dram->dL[gcount] = inputSampleL; dram->dR[gcount] = inputSampleR;
		int count = gcount;
		double offset = depth + (depth * pow(rateof,2) * sin(sweep));
		count += (int)floor(offset);
		
		inputSampleL = (dram->dL[count-((count > 999)?1000:0)] * (1-(offset-floor(offset))));
		inputSampleL += (dram->dL[count+1-((count+1 > 999)?1000:0)] * (offset-floor(offset)));
		inputSampleR = (dram->dR[count-((count > 999)?1000:0)] * (1-(offset-floor(offset))));
		inputSampleR += (dram->dR[count+1-((count+1 > 999)?1000:0)] * (offset-floor(offset)));
		
		rateof = (rateof * (1.0-fluttertrim)) + (nextmax * fluttertrim);
		sweep += rateof * fluttertrim;
		if (sweep >= (M_PI*2.0)) {sweep -= M_PI; nextmax = 0.24 + (fpdL / (double)UINT32_MAX * 0.74);}
		//apply to input signal only when flutter is present, interpolate samples
		gcount--;
		
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
	for (int temp = 0; temp < 1001; temp++) {dram->dL[temp] = 0.0;dram->dR[temp] = 0.0;}
	gcount = 0;	
	sweep = M_PI;
	rateof = 0.5;
	nextmax = 0.5;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
