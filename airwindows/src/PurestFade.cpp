#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestFade"
#define AIRWINDOWS_DESCRIPTION "Just like PurestGain, but for LONG fades."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','!' )
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
{ .name = "Gain", .min = -4000, .max = 4000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Fade", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 gainchase;
		Float64 settingchase;
		Float64 gainBchase;
		Float64 chasespeed;
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
	
	Float64 inputgain = GetParameter( kParam_One );
	
	if (settingchase != inputgain) {
		chasespeed *= 2.0;
		settingchase = inputgain;
		//increment the slowness for each fader movement
		//continuous alteration makes it react smoother
		//sudden jump to setting, not so much
	}
	if (chasespeed > 2500.0) chasespeed = 2500.0;
	//bail out if it's too extreme
	if (gainchase < -60.0) {
		gainchase = pow(10.0,inputgain/20.0);
		//shouldn't even be a negative number
		//this is about starting at whatever's set, when
		//plugin is instantiated.
		//Otherwise it's the target, in dB.
	}
	double targetgain;	
	//done with top controller
	
	double targetBgain = GetParameter( kParam_Two );
	if (gainBchase < 0.0) gainBchase = targetBgain;
	//this one is not a dB value, but straight multiplication
	//done with slow fade controller
	
	double fadeSpeed = targetBgain;
	if (fadeSpeed < 0.0027) fadeSpeed = 0.0027; //minimum fade speed
	fadeSpeed = ((GetSampleRate()*0.004) / fadeSpeed);
	//this will tend to be much slower than PurestGain, and also adapt to sample rates
	
	double outputgain;
	
	double inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		targetgain = pow(10.0,settingchase/20.0);
		//now we have the target in our temp variable
		
		chasespeed *= 0.9999;
		chasespeed -= 0.01;
		if (chasespeed < 350.0) chasespeed = 350.0;
		//we have our chase speed compensated for recent fader activity
		
		gainchase = (((gainchase*chasespeed)+targetgain)/(chasespeed+1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		
		gainBchase = (((gainBchase*fadeSpeed)+targetBgain)/(fadeSpeed+1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		
		outputgain = gainchase * gainBchase;
		//directly multiply the dB gain by the straight multiply gain
		
		if (1.0 == outputgain) *destP = *sourceP;
		else
		{
			inputSample *= outputgain;
			
			//begin 32 bit floating point dither
			int expon; frexpf((float)inputSample, &expon);
			fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
			inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
			//end 32 bit floating point dither
			*destP = inputSample;
		}		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	gainchase = -90.0;
	settingchase = -90.0;
	gainBchase = -90.0;
	chasespeed = 350.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
