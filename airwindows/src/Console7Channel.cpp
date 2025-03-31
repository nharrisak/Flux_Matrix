#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console7Channel"
#define AIRWINDOWS_DESCRIPTION "Adds anti-alias filtering and special saturation curves tied to the slider position."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','y' )
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
{ .name = "Fader", .min = 0, .max = 1000, .def = 772, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 gainchase;
		Float64 chasespeed;
		double biquadA[11];
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
	double inputgain = GetParameter( kParam_One )*1.272019649514069;
	//which is, in fact, the square root of 1.618033988749894848204586...
	//this happens to give us a boost factor where the track continues to get louder even
	//as it saturates and loses a bit of peak energy. Console7Channel channels go to 12! (.272,etc)
	//Neutral gain through the whole system with a full scale sine ia 0.772 on the gain knob
	if (gainchase != inputgain) chasespeed *= 2.0;
	if (chasespeed > inFramesToProcess) chasespeed = inFramesToProcess;
	if (gainchase < 0.0) gainchase = inputgain;
	
	biquadA[0] = 20000.0 / GetSampleRate();
    biquadA[1] = 1.618033988749894848204586;
	
	double K = tan(M_PI * biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0 * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;	
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		double tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = tempSample; 
		biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1
		
		chasespeed *= 0.9999; chasespeed -= 0.01; if (chasespeed < 64.0) chasespeed = 64.0;
		//we have our chase speed compensated for recent fader activity
		gainchase = (((gainchase*chasespeed)+inputgain)/(chasespeed+1.0));
		//gainchase is chasing the target, as a simple multiply gain factor
		if (1.0 != gainchase) inputSample *= pow(gainchase,3);
		//this trim control cuts back extra hard because we will amplify after the distortion
		//that will shift the distortion/antidistortion curve, in order to make faded settings
		//slightly 'expanded' and fall back in the soundstage, subtly
		
		if (inputSample > 1.097) inputSample = 1.097;
		if (inputSample < -1.097) inputSample = -1.097;
		inputSample = ((sin(inputSample*fabs(inputSample))/((fabs(inputSample) == 0.0) ?1:fabs(inputSample)))*0.8)+(sin(inputSample)*0.2);
		//this is a version of Spiral blended 80/20 with regular Density ConsoleChannel.
		//It's blending between two different harmonics in the overtones of the algorithm
				
		if (1.0 != gainchase && 0.0 != gainchase) inputSample /= gainchase;
		//we re-amplify after the distortion relative to how much we cut back previously.
		
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
	gainchase = -1.0;
	chasespeed = 64.0;
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
