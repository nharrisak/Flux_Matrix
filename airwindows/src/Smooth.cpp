#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Smooth"
#define AIRWINDOWS_DESCRIPTION "Can tame pointy sounds or make drums explode."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','m','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Smooth", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;

		
		Float64 lastSampleA;
		Float64 gainA;
		Float64 lastSampleB;
		Float64 gainB;
		Float64 lastSampleC;
		Float64 gainC;
		Float64 lastSampleD;
		Float64 gainD;
		Float64 lastSampleE;
		Float64 gainE;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
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
	Float64 clamp;
	Float64 chase = pow(GetParameter( kParam_One ),2);
	Float64 makeup = (1.0+(chase*1.6)) * GetParameter( kParam_Two );
	Float64 ratio = chase * 24.0;
	chase /= overallscale;
	chase *= 0.083; // set up the ratio of new val to old
	Float64 wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		clamp = fabs(inputSample - lastSampleA);
		clamp = sin(clamp*ratio);
		lastSampleA = inputSample;
		gainA *= (1.0 - chase);
		gainA += ((1.0-clamp) * chase);
		if (gainA > 1.0) gainA = 1.0;
		if (gainA < 0.0) gainA = 0.0;
		if (gainA != 1.0) inputSample *= gainA;
		
		clamp = fabs(inputSample - lastSampleB);
		clamp = sin(clamp*ratio);
		lastSampleB = inputSample;
		gainB *= (1.0 - chase);
		gainB += ((1.0-clamp) * chase);
		if (gainB > 1.0) gainB = 1.0;
		if (gainB < 0.0) gainB = 0.0;
		if (gainB != 1.0) inputSample *= gainB;
		
		clamp = fabs(inputSample - lastSampleC);
		clamp = sin(clamp*ratio);
		lastSampleC = inputSample;
		gainC *= (1.0 - chase);
		gainC += ((1.0-clamp) * chase);
		if (gainC > 1.0) gainC = 1.0;
		if (gainC < 0.0) gainC = 0.0;
		if (gainC != 1.0) inputSample *= gainC;
		
		clamp = fabs(inputSample - lastSampleD);
		clamp = sin(clamp*ratio);
		lastSampleD = inputSample;
		gainD *= (1.0 - chase);
		gainD += ((1.0-clamp) * chase);
		if (gainD > 1.0) gainD = 1.0;
		if (gainD < 0.0) gainD = 0.0;
		if (gainD != 1.0) inputSample *= gainD;
		
		clamp = fabs(inputSample - lastSampleE);
		clamp = sin(clamp*ratio);
		lastSampleE = inputSample;
		gainE *= (1.0 - chase);
		gainE += ((1.0-clamp) * chase);
		if (gainE > 1.0) gainE = 1.0;
		if (gainE < 0.0) gainE = 0.0;
		if (gainE != 1.0) inputSample *= gainE;
		
		if (makeup !=1.0) {
			inputSample *= makeup;
		}

		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSampleA = 0.0;
	gainA = 1.0;
	lastSampleB = 0.0;
	gainB = 1.0;
	lastSampleC = 0.0;
	gainC = 1.0;
	lastSampleD = 0.0;
	gainD = 1.0;
	lastSampleE = 0.0;
	gainE = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
