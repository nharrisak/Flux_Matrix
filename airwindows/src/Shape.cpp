#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Shape"
#define AIRWINDOWS_DESCRIPTION "An asymmetrical waveshaper for peak manipulating."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','h','a' )
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
{ .name = "Shape", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Neg/Pos", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		uint32_t fpd;
	
	struct _dram {
			double fixA[fix_total];
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
	
	double shape = -((GetParameter( kParam_One )*2.0)-1.0);
	double gainstage = fabs(shape)+0.01; //no divide by zero
	double offset = (GetParameter( kParam_Two )*2.0)-1.0;
	double postOffset = 0.0;
	if (shape > 0) {
		gainstage += 0.99;
		postOffset = sin(offset);
	}
	if (shape < 0) postOffset = asin(offset);
	double cutoff = 25000.0 / GetSampleRate();
	if (cutoff > 0.49) cutoff = 0.49; //don't crash if run at 44.1k
	dram->fixA[fix_freq] = cutoff;
	dram->fixA[fix_reso] = 0.70710678; //butterworth Q
	double K = tan(M_PI * dram->fixA[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / dram->fixA[fix_reso] + K * K);
	dram->fixA[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = 2.0 * dram->fixA[fix_a0];
	dram->fixA[fix_a2] = dram->fixA[fix_a0];
	dram->fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->fixA[fix_b2] = (1.0 - K / dram->fixA[fix_reso] + K * K) * norm;
	dram->fixA[fix_sL1] = 0.0;
	dram->fixA[fix_sL2] = 0.0;
	dram->fixA[fix_sR1] = 0.0;
	dram->fixA[fix_sR2] = 0.0;
	//define filters here: on VST you can't define them in reset 'cos getSampleRate isn't returning good information yet
	
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		double outSample = (inputSample * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSample * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSample * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample *= gainstage;
		inputSample += offset;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		if (shape > 0) inputSample = sin(inputSample);
		if (shape < 0) inputSample = asin(inputSample);
		inputSample -= postOffset;
		inputSample = ((inputSample/gainstage)*fabs(shape))+(drySample*(1.0-fabs(shape)));

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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
