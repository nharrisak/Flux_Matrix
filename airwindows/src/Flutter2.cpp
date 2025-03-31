#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Flutter2"
#define AIRWINDOWS_DESCRIPTION "The flutter from ToTape7, standalone."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','l','v' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Flutter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;

		int gcount;		
		double sweep;
		double nextmax;
		
		uint32_t fpd;
	
	struct _dram {
			double dL[1002];
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	double flutDepth = pow(GetParameter( kParam_A ),4)*overallscale*90;
	if (flutDepth > 498.0) flutDepth = 498.0;
	double flutFrequency = (0.02*pow(GetParameter( kParam_B ),3))/overallscale;
	double wet = GetParameter( kParam_C );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		//begin Flutter
		if (gcount < 0 || gcount > 999) gcount = 999;
		dram->dL[gcount] = inputSample;
		int count = gcount;
		double offset = flutDepth + (flutDepth * sin(sweep));
		sweep += nextmax * flutFrequency;
		if (sweep > (M_PI*2.0)) {sweep -= M_PI*2.0; nextmax = 0.24 + (fpd / (double)UINT32_MAX * 0.74);}
		count += (int)floor(offset);
		inputSample = (dram->dL[count-((count > 999)?1000:0)] * (1-(offset-floor(offset))));
		inputSample += (dram->dL[count+1-((count+1 > 999)?1000:0)] * (offset-floor(offset)));
		gcount--;
		//end Flutter
		
		if (wet < 1.0) {inputSample = (inputSample * wet) + (drySample * (1.0-wet));}

		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int temp = 0; temp < 1001; temp++) {dram->dL[temp] = 0.0;}
	gcount = 0;	
	sweep = M_PI;
	nextmax = 0.5;	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
