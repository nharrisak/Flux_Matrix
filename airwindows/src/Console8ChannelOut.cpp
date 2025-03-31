#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console8ChannelOut"
#define AIRWINDOWS_DESCRIPTION "Moves to a channel/submix/buss topology and adds analog simulation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','(' )
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
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		bool hsr;
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
		double fix[fix_total];
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
	
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_One )*2.0;
	//0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
	//into softclipping overdrive.
	if (GetSampleRate() > 49000.0) hsr = true; else hsr = false;
	fix[fix_freq] = 24000.0 / GetSampleRate();
	fix[fix_reso] = 3.51333709;
	double K = tan(M_PI * fix[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / fix[fix_reso] + K * K);
	fix[fix_a0] = K * K * norm;
	fix[fix_a1] = 2.0 * fix[fix_a0];
	fix[fix_a2] = fix[fix_a0];
	fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fix[fix_b2] = (1.0 - K / fix[fix_reso] + K * K) * norm;
	//this is the fixed biquad distributed anti-aliasing filter
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		
		double position = (double)nSampleFrames/inFramesToProcess;
		double inTrim = (inTrimA*position)+(inTrimB*(1.0-position));
		//input trim smoothed to cut out zipper noise
		inputSample *= inTrim;
		if (inputSample > 1.57079633) inputSample = 1.57079633; if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//Console8 gain stage clips at exactly 1.0 post-sin()
		if (hsr){
			double outSample = (inputSample * fix[fix_a0]) + fix[fix_sL1];
			fix[fix_sL1] = (inputSample * fix[fix_a1]) - (outSample * fix[fix_b1]) + fix[fix_sL2];
			fix[fix_sL2] = (inputSample * fix[fix_a2]) - (outSample * fix[fix_b2]);
			inputSample = outSample;
		} //fixed biquad filtering ultrasonics
		inputSample *= inTrim;
		if (inputSample > 1.57079633) inputSample = 1.57079633; if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//Console8 gain stage clips at exactly 1.0 post-sin()
		
		
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
	inTrimA = 0.5; inTrimB = 0.5;
	for (int x = 0; x < fix_total; x++) fix[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
