#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SampleUndelay"
#define AIRWINDOWS_DESCRIPTION "AU-only SampleDelay featuring negative delay."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','a','n' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
static const int kDefaultValue_ParamFour = 1.0;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "ms", .min = -5000, .max = 5000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Samples", .min = 0, .max = 10000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Subsample", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int gcount;
		uint32_t fpd;
	
	struct _dram {
			double p[16386];
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	Float64 coarse = (50.0 + GetParameter( kParam_One )) * (44.1*overallscale);
	Float64 fine = GetParameter( kParam_Two );
	Float64 subsample = GetParameter( kParam_Three );
	int offset = floor(coarse + fine);
	if (offset < 0) offset = 0; //insanity check
	if (offset > 16380) offset = 16380; //insanity check
	int maxtime = 16382;
	Float64 phase = GetParameter( kParam_Four );
	Float64 dryLevel = 1.0-fabs(phase);
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		if (gcount < 0 || gcount > maxtime) {gcount = maxtime;}
		int count = gcount;
		dram->p[count] = inputSample;
		count += offset;
		inputSample = dram->p[count-((count > maxtime)?maxtime+1:0)]*(1.0 - subsample);
		inputSample += dram->p[count+1-((count+1 > maxtime)?maxtime+1:0)]*subsample;
		gcount--;
		
		if (phase < 1.0) inputSample *= phase;
		
		if (dryLevel > 0.0) inputSample += (drySample * dryLevel);
		
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
	for(int count = 0; count < 16385; count++) {dram->p[count] = 0.0;}
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
