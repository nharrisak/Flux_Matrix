#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console8LiteChannel"
#define AIRWINDOWS_DESCRIPTION "Simplified Console8, working with just a single mix buss."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','*' )
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

		double iirA;
		double iirB;		
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
		double soft[11];
		int cycleEnd;
		//from undersampling code, used as a way to space out HF taps

		double inTrimA;
		double inTrimB;
		double fixB[fix_total];
		
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
	
	double iirAmountA = 12.66/GetSampleRate();
	//this is our distributed unusual highpass, which is
	//adding subtle harmonics to the really deep stuff to define it
	if (fabs(iirA)<1.18e-37) iirA = 0.0;
	if (fabs(iirB)<1.18e-37) iirB = 0.0;
	//catch denormals early and only check once per buffer
	if (GetSampleRate() > 49000.0) hsr = true; else hsr = false;
	fix[fix_freq] = 24000.0 / GetSampleRate();
	fix[fix_reso] = 2.24697960;
	double K = tan(M_PI * fix[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / fix[fix_reso] + K * K);
	fix[fix_a0] = K * K * norm;
	fix[fix_a1] = 2.0 * fix[fix_a0];
	fix[fix_a2] = fix[fix_a0];
	fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fix[fix_b2] = (1.0 - K / fix[fix_reso] + K * K) * norm;
	//this is the fixed biquad distributed anti-aliasing filter
	
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_One )*2.0;
	//0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
	//into softclipping overdrive.
	fixB[fix_freq] = 24000.0 / GetSampleRate();
	fixB[fix_reso] = 0.80193774;
	K = tan(M_PI * fixB[fix_freq]); //lowpass
	norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
	fixB[fix_a0] = K * K * norm;
	fixB[fix_a1] = 2.0 * fixB[fix_a0];
	fixB[fix_a2] = fixB[fix_a0];
	fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
	//this is the fixed biquad distributed anti-aliasing filter
	
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd == 3) cycleEnd = 4;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		double position = (double)nSampleFrames/inFramesToProcess;
		double inTrim = (inTrimA*position)+(inTrimB*(1.0-position));
		//input trim smoothed to cut out zipper noise
		
		iirA = (iirA * (1.0 - iirAmountA)) + (inputSample * iirAmountA);
		double iirAmountB = fabs(iirA)+0.00001;
		iirB = (iirB * (1.0 - iirAmountB)) + (iirA * iirAmountB);
		inputSample -= iirB;
		//Console8 highpass
		if (cycleEnd == 4) {
			soft[8] = soft[7]; soft[7] = soft[6]; soft[6] = soft[5]; soft[5] = soft[4];
			soft[4] = soft[3]; soft[3] = soft[2]; soft[2] = soft[1]; soft[1] = soft[0];
			soft[0] = inputSample;
		}
		if (cycleEnd == 2) {
			soft[8] = soft[6]; soft[6] = soft[4];
			soft[4] = soft[2]; soft[2] = soft[0];
			soft[0] = inputSample;
		}
		if (cycleEnd == 1) {
			soft[8] = soft[4];
			soft[4] = soft[0];
			soft[0] = inputSample;
		}
		soft[9] = ((soft[0]-soft[4])-(soft[4]-soft[8]));
		if (soft[9] > 0.91416342) inputSample = soft[4]+(fabs(soft[4])*sin(soft[9]-0.91416342)*0.08583658);
		if (-soft[9] > 0.91416342) inputSample = soft[4]-(fabs(soft[4])*sin(-soft[9]-0.91416342)*0.08583658);
		if (inputSample > 1.57079633) inputSample = 1.57079633; if (inputSample < -1.57079633) inputSample = -1.57079633;
		//Console8 slew soften: must be clipped or it can generate NAN out of the full system
		if (hsr){
			double outSample = (inputSample * fix[fix_a0]) + fix[fix_sL1];
			fix[fix_sL1] = (inputSample * fix[fix_a1]) - (outSample * fix[fix_b1]) + fix[fix_sL2];
			fix[fix_sL2] = (inputSample * fix[fix_a2]) - (outSample * fix[fix_b2]);
			inputSample = outSample;
		} //fixed biquad filtering ultrasonics
		//we can go directly into the first distortion stage of ChannelOut
		//with a filtered signal, so its biquad is between stages
		//on the input channel we have direct signal, not Console8 decode
		
		inputSample *= inTrim;
		if (inputSample > 1.57079633) inputSample = 1.57079633; if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		//Console8 gain stage clips at exactly 1.0 post-sin()
		if (hsr){
			double outSample = (inputSample * fixB[fix_a0]) + fixB[fix_sL1];
			fixB[fix_sL1] = (inputSample * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
			fixB[fix_sL2] = (inputSample * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
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
	iirA = 0.0; iirB = 0.0;
	for (int x = 0; x < fix_total; x++) fix[x] = 0.0;
	for (int x = 0; x < 10; x++) soft[x] = 0.0;
	
	inTrimA = 0.5; inTrimB = 0.5;
	for (int x = 0; x < fix_total; x++) fixB[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
