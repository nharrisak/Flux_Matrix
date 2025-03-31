#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SoftGateMono"
#define AIRWINDOWS_DESCRIPTION "SoftGateMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','o','h' )
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
{ .name = "Bright Threshold", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darken Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Base Silence", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 storedL[2];
		Float64 diffL;
		Float64 gate;
		uint32_t fpd;
	
	struct _dram {
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
	Float64 threshold = pow(GetParameter( kParam_One ),6);
	Float64 recovery = pow((GetParameter( kParam_Two )*0.5),6);
	recovery /= overallscale;
	Float64 baseline = pow(GetParameter( kParam_Three ),6);
	Float64 invrec = 1.0 - recovery;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		storedL[1] = storedL[0];
		storedL[0] = inputSample;
		diffL = storedL[0] - storedL[1];
		
		if (gate > 0) {gate = ((gate-baseline) * invrec) + baseline;}
		
		if (fabs(diffL) > threshold) {gate = 1.1;}
		else {gate = (gate * invrec); if (threshold > 0) {gate += ((fabs(inputSample)/threshold) * recovery);}}
		
		if (gate < 0) gate = 0;
		
		if (gate < 1.0)
		{
			storedL[0] = storedL[1] + (diffL * gate);		
		}
		
		if (gate < 1) inputSample = (inputSample * gate) + (storedL[0] * (1.0-gate));
		
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
	storedL[0] = storedL[1] = 0.0;
	diffL = 0.0;
	gate = 1.1;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
