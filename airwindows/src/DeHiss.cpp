#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeHiss"
#define AIRWINDOWS_DESCRIPTION "Tries to suppress background hiss, like a hiss gate."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','H' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 stored[2];
		Float64 diff[6];
		Float64 gate;
		Float64 raw;
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

	Float64 meanA;
	Float64 meanB;
	Float64 meanOut = 0;
	Float64 meanLast;
	Float64 average[5];
	Float64 threshold = pow(GetParameter( kParam_One ),9);
	Float64 wet = GetParameter( kParam_Two );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		stored[1] = stored[0];
		stored[0] = inputSample;
		diff[5] = diff[4];
		diff[4] = diff[3];
		diff[3] = diff[2];
		diff[2] = diff[1];
		diff[1] = diff[0];
		diff[0] = stored[0] - stored[1];
		
		average[4] = (diff[0] + diff[1] + diff[2] + diff[3] + diff[4] + diff[5])/6.0;
		average[3] = (diff[0] + diff[1] + diff[2] + diff[3] + diff[4])/5.0;
		average[2] = (diff[0] + diff[1] + diff[2] + diff[3])/4.0;
		average[1] = (diff[0] + diff[1] + diff[2])/3.0;
		average[0] = (diff[0] + diff[1])/2.0;
		
		meanA = diff[0];
		meanB = diff[0];
		if (fabs(average[4]) < fabs(meanB)) {meanA = meanB; meanB = average[4];}
		if (fabs(average[3]) < fabs(meanB)) {meanA = meanB; meanB = average[3];}
		if (fabs(average[2]) < fabs(meanB)) {meanA = meanB; meanB = average[2];}
		if (fabs(average[1]) < fabs(meanB)) {meanA = meanB; meanB = average[1];}
		if (fabs(average[0]) < fabs(meanB)) {meanA = meanB; meanB = average[0];}
		meanLast = meanOut;
		meanOut = ((meanA+meanB)/2.0);
		
		if (raw > 0) raw -= 0.001;
		if (fabs(inputSample) > threshold) {gate = 1.0; raw = 2.0;}
		else {gate = (gate * 0.999); if (threshold > 0) gate += ((fabs(inputSample)/threshold) * 0.001);}
		
		if ((fabs(meanOut) > threshold) || (fabs(meanLast) > threshold)){}
		else stored[0] = stored[1] + (meanOut * gate);
		
		if (raw < 1) inputSample = (inputSample * raw) + (stored[0] * (1-raw));
		
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
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	stored[0] = stored[1] = 0.0;
	diff[0] = diff[1] = diff[2] = diff[3] = diff[4] = diff[5] = 0.0;
	gate = 1.0;
	raw = 2.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
