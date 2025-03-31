#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Creature"
#define AIRWINDOWS_DESCRIPTION "A soft slew saturator, a new class of noisechanger."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','r','f' )
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
{ .name = "Drive", .min = 0, .max = 1000, .def = 260, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 260, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		double slew[102]; //probably worth just using a number here
};
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
	
	double source = 1.0-pow(1.0-GetParameter( kParam_One ),5);
	int stages = (pow(GetParameter( kParam_Two ),2)*32.0*sqrt(overallscale))+1;
	double wet = (GetParameter( kParam_Three )*2.0)-1.0; //inv-dry-wet for highpass
	double dry = 2.0-(GetParameter( kParam_Three )*2.0);
	if (dry > 1.0) dry = 1.0; //full dry for use with inv, to 0.0 at full wet
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		for (int x = 0; x < stages; x++) {
			inputSample = (dram->slew[x]+(sin(dram->slew[x]-inputSample)*0.5))*source;
			dram->slew[x] = inputSample*0.5;
		}
		if (stages % 2 > 0) inputSample = -inputSample;
		
		inputSample *= wet;
		drySample *= dry;
		inputSample += drySample;
		
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
	for (int x = 0; x < 101; x++) dram->slew[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
