#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TapeDust"
#define AIRWINDOWS_DESCRIPTION "Just a special treble-erode noise, a ‘slew noise’ plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','a','u' )
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
{ .name = "Dust", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 b[11];
		Float64 f[11];		
		uint32_t fpd;
		bool fpFlip;
	
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
	double inputSample;
	Float64 drySample;
	Float64 rRange = pow(GetParameter( kParam_One ),2)*5.0;
	Float64 xfuzz = rRange * 0.002;
	Float64 rOffset = (rRange*0.4) + 1.0;
	Float64 rDepth; //the randomly fluctuating value
	Float64 gain;
	Float64 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
		
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		for(int count = 9; count < 0; count--) {
			b[count+1] = b[count];
		}
		
		b[0] = inputSample;
		inputSample = rand() / (double)RAND_MAX;
		gain = rDepth = (inputSample * rRange) + rOffset;
		inputSample *= ((1.0-fabs(b[0]-b[1]))*xfuzz);
		if (fpFlip) inputSample = -inputSample;
		
		for(int count = 0; count < 9; count++) {			
			if (gain > 1.0) {
				f[count] = 1.0;
				gain -= 1.0;
			} else {
				f[count] = gain;
				gain = 0.0;
			}
			f[count] /= rDepth;
			inputSample += (b[count] * f[count]);
		}
				
		if (wet < 1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		fpFlip = !fpFlip;
		
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
	for(int count = 0; count < 11; count++) {b[count] = 0.0; f[count] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = false;
}
};
