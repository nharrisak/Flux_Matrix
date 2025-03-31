#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DrumSlam"
#define AIRWINDOWS_DESCRIPTION "A heavy-processing tape modeler. This entry contains personal details about what was happening in 2018 for me, and is included in full because there are things that have a place in AirwindowsPedia, as a sort of 'historical section'."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','r','u' )
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
{ .name = "Drive", .min = 1000, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 iirSampleF;
		Float64 iirSampleG;
		Float64 iirSampleH;
		Float64 lastSample;
		uint32_t fpd;
		bool fpFlip;
	};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{

	//This code will pass-thru the audio data.
	//This is where you want to process data to produce an effect.

	
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 iirAmountL = 0.0819;
	iirAmountL /= overallscale;
	Float64 iirAmountH = 0.377933067;
	iirAmountH /= overallscale;
	Float64 drive = GetParameter( kParam_One );
	Float64 out = GetParameter( kParam_Two );
	Float64 wet = GetParameter( kParam_Three );
	//removed unnecessary dry variable
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		double lowSample;
		double midSample;
		double highSample;
		
		inputSample *= drive;
		if (fpFlip)
			{
			iirSampleA = (iirSampleA * (1 - iirAmountL)) + (inputSample * iirAmountL);
			iirSampleB = (iirSampleB * (1 - iirAmountL)) + (iirSampleA * iirAmountL);
			lowSample = iirSampleB;
			iirSampleE = (iirSampleE * (1 - iirAmountH)) + (inputSample * iirAmountH);
			iirSampleF = (iirSampleF * (1 - iirAmountH)) + (iirSampleE * iirAmountH);
			midSample = iirSampleF - iirSampleB;
			highSample = inputSample - iirSampleF;
			}
		else
			{
			iirSampleC = (iirSampleC * (1 - iirAmountL)) + (inputSample * iirAmountL);
			iirSampleD = (iirSampleD * (1 - iirAmountL)) + (iirSampleC * iirAmountL);
			lowSample = iirSampleD;
			iirSampleG = (iirSampleG * (1 - iirAmountH)) + (inputSample * iirAmountH);
			iirSampleH = (iirSampleH * (1 - iirAmountH)) + (iirSampleG * iirAmountH);
			midSample = iirSampleH - iirSampleD;
			highSample = inputSample - iirSampleH;
			}
		//generate the tone bands we're using
		if (lowSample > 1.0) {lowSample = 1.0;}
		if (lowSample < -1.0) {lowSample = -1.0;}
		lowSample -= (lowSample * (fabs(lowSample) * 0.448) * (fabs(lowSample) * 0.448) );
		lowSample *= drive;

		if (highSample > 1.0) {highSample = 1.0;}
		if (highSample < -1.0) {highSample = -1.0;}
		highSample -= (highSample * (fabs(highSample) * 0.599) * (fabs(highSample) * 0.599) );
		highSample *= drive;

		midSample = midSample * drive;
		double skew = (midSample - lastSample);
		lastSample = midSample;
		//skew will be direction/angle
		double bridgerectifier = fabs(skew);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skew > 0) skew = bridgerectifier*3.1415926;
		else skew = -bridgerectifier*3.1415926;
		//skew is now sined and clamped and then re-amplified again
		skew *= midSample;
		//cools off sparkliness and crossover distortion
		skew *= 1.557079633;
		//crank up the gain on this so we can make it sing
		bridgerectifier = fabs(midSample);
		bridgerectifier += skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		bridgerectifier *= drive;
		bridgerectifier += skew;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (midSample > 0)
				{
				midSample = bridgerectifier; //(midSample*(1-1.557079633+skew))+((bridgerectifier)*(1.557079633+skew));
				}
			else
				{
				midSample = -bridgerectifier; //(midSample*(1-1.557079633+skew))-((bridgerectifier)*(1.557079633+skew));
				}
		//blend according to positive and negative controls
		
		inputSample = ((lowSample + midSample + highSample)/drive)*out;
		
		if (wet < 1.0) {
			inputSample = (drySample * (1.0-wet))+(inputSample*wet);
		}
		fpFlip = !fpFlip;
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = false;
}
};
