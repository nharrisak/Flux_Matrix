#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hypersoft"
#define AIRWINDOWS_DESCRIPTION "A more extreme form of soft-clipper."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','y','q' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bright", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		double lastSample;
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

	double inputGain = GetParameter( kParam_A )*2.0;
	if (inputGain > 1.0) inputGain *= inputGain; else inputGain = 1.0-pow(1.0-inputGain,2);
	//this is the fader curve from ConsoleX with 0.5 being unity gain
	int stages = (int)(GetParameter( kParam_B )*12.0)+2;
	//each stage brings in an additional layer of harmonics on the waveshaping
	double bright = (1.0-GetParameter( kParam_C ))*0.15;
	//higher slews suppress these higher harmonics when they are sure to just alias
	double outputGain = GetParameter( kParam_D )*2.0;
	if (outputGain > 1.0) outputGain *= outputGain; else outputGain = 1.0-pow(1.0-outputGain,2);
	outputGain *= 0.68;
	//this is the fader curve from ConsoleX, rescaled to work with Hypersoft

	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		inputSample *= inputGain;
		
		inputSample = sin(inputSample); inputSample += (sin(inputSample*2.0)/2.0);
		for (int count = 2; count<stages; count++){
			inputSample += ((sin(inputSample*(double)count)/(double)pow(count,3))*fmax(0.0,1.0-fabs((inputSample-lastSample)*bright*(double)(count*count))));
		}
		lastSample = inputSample;
		
		inputSample *= outputGain;
		
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
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
