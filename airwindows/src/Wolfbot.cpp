#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Wolfbot"
#define AIRWINDOWS_DESCRIPTION "An aggressive Kalman bandpass with evil in its heart."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','W','o','l' )
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			prevSampL1,
			prevSlewL1,
			accSlewL1,
			prevSampL2,
			prevSlewL2,
			accSlewL2,
			prevSampL3,
			prevSlewL3,
			accSlewL3,
			kalGainL,
			kalOutL,
			kal_total
		};
		double kHP[kal_total];
		double kLP[kal_total];

		uint32_t fpd;
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
	
	double kalHP = 1.0-(0.004225/overallscale);
	double kalLP = 1.0-(0.954529/overallscale);
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		//begin Kalman Filter
		double dryKal = inputSample = inputSample*(1.0-kalHP)*0.777;
		inputSample *= (1.0-kalHP);
		//set up gain levels to control the beast
		kHP[prevSlewL3] += kHP[prevSampL3] - kHP[prevSampL2]; kHP[prevSlewL3] *= 0.5;
		kHP[prevSlewL2] += kHP[prevSampL2] - kHP[prevSampL1]; kHP[prevSlewL2] *= 0.5;
		kHP[prevSlewL1] += kHP[prevSampL1] - inputSample; kHP[prevSlewL1] *= 0.5;
		//make slews from each set of samples used
		kHP[accSlewL2] += kHP[prevSlewL3] - kHP[prevSlewL2]; kHP[accSlewL2] *= 0.5;
		kHP[accSlewL1] += kHP[prevSlewL2] - kHP[prevSlewL1]; kHP[accSlewL1] *= 0.5;
		//differences between slews: rate of change of rate of change
		kHP[accSlewL3] += (kHP[accSlewL2] - kHP[accSlewL1]); kHP[accSlewL3] *= 0.5;
		//entering the abyss, what even is this
		kHP[kalOutL] += kHP[prevSampL1] + kHP[prevSlewL2] + kHP[accSlewL3]; kHP[kalOutL] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		kHP[kalGainL] += fabs(dryKal-kHP[kalOutL])*kalHP*8.0; kHP[kalGainL] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (kHP[kalGainL] > kalHP*0.5) kHP[kalGainL] = kalHP*0.5;
		//attempts to avoid explosions
		kHP[kalOutL] += (dryKal*(1.0-(0.68+(kalHP*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		kHP[prevSampL3] = kHP[prevSampL2];
		kHP[prevSampL2] = kHP[prevSampL1];
		kHP[prevSampL1] = (kHP[kalGainL] * kHP[kalOutL]) + ((1.0-kHP[kalGainL])*dryKal);
		//feed the chain of previous samples
		if (kHP[prevSampL1] > 1.0) kHP[prevSampL1] = 1.0;
		if (kHP[prevSampL1] < -1.0) kHP[prevSampL1] = -1.0;
		//end Kalman Filter, except for trim on output		
		inputSample = drySample+(kHP[kalOutL]*-0.777); //highpass
		
		//begin Kalman Filter
		dryKal = inputSample = inputSample*(1.0-kalLP)*0.777;
		inputSample *= (1.0-kalLP);
		//set up gain levels to control the beast
		kLP[prevSlewL3] += kLP[prevSampL3] - kLP[prevSampL2]; kLP[prevSlewL3] *= 0.5;
		kLP[prevSlewL2] += kLP[prevSampL2] - kLP[prevSampL1]; kLP[prevSlewL2] *= 0.5;
		kLP[prevSlewL1] += kLP[prevSampL1] - inputSample; kLP[prevSlewL1] *= 0.5;
		//make slews from each set of samples used
		kLP[accSlewL2] += kLP[prevSlewL3] - kLP[prevSlewL2]; kLP[accSlewL2] *= 0.5;
		kLP[accSlewL1] += kLP[prevSlewL2] - kLP[prevSlewL1]; kLP[accSlewL1] *= 0.5;
		//differences between slews: rate of change of rate of change
		kLP[accSlewL3] += (kLP[accSlewL2] - kLP[accSlewL1]); kLP[accSlewL3] *= 0.5;
		//entering the abyss, what even is this
		kLP[kalOutL] += kLP[prevSampL1] + kLP[prevSlewL2] + kLP[accSlewL3]; kLP[kalOutL] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		kLP[kalGainL] += fabs(dryKal-kLP[kalOutL])*kalLP*8.0; kLP[kalGainL] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (kLP[kalGainL] > kalLP*0.5) kLP[kalGainL] = kalLP*0.5;
		//attempts to avoid explosions
		kLP[kalOutL] += (dryKal*(1.0-(0.68+(kalLP*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		kLP[prevSampL3] = kLP[prevSampL2];
		kLP[prevSampL2] = kLP[prevSampL1];
		kLP[prevSampL1] = (kLP[kalGainL] * kLP[kalOutL]) + ((1.0-kLP[kalGainL])*dryKal);
		//feed the chain of previous samples
		if (kLP[prevSampL1] > 1.0) kLP[prevSampL1] = 1.0;
		if (kLP[prevSampL1] < -1.0) kLP[prevSampL1] = -1.0;
		//end Kalman Filter, except for trim on output		
		inputSample = sin(kLP[kalOutL]*0.7943)*1.2589; //lowpass
		
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
	for (int x = 0; x < kal_total; x++) {kHP[x] = 0.0; kLP[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
