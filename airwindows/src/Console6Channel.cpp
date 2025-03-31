#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console6Channel"
#define AIRWINDOWS_DESCRIPTION "A new more intense Console that peaks at lower dB."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','v' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		double inTrimA;
		double inTrimB;
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
	inTrimA = inTrimB;
	inTrimB = GetParameter( kParam_One );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double inTrim = (inTrimA*temp)+(inTrimB*(1.0-temp));
		
		if (inTrim != 1.0) {
			inputSample *= inTrim;
		}
		
		//encode/decode courtesy of torridgristle under the MIT license
		//Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5
		//Reformulated using 'Herbie' for better accuracy near zero
		
		if (inputSample > 1.0) inputSample = 1.0;
		else if (inputSample > 0.0) inputSample = inputSample * (2.0 - inputSample);
		
		if (inputSample < -1.0) inputSample = -1.0;
		else if (inputSample < 0.0) inputSample = inputSample * (inputSample + 2.0);
		
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
	inTrimA = 1.0; inTrimB = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
