#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "NCSeventeen"
#define AIRWINDOWS_DESCRIPTION "Dirty Loud!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','C','S' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "LOUDER", .min = 0, .max = 24000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 lastSample;
		Float64 iirSampleA;
		Float64 iirSampleB;
		bool flip;
		Float64 basslev;
		Float64 treblev;
		Float64 cheblev;
		uint32_t fpd;
		//default stuff
		
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
	Float64 inP2;
	Float64 chebyshev;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 IIRscaleback = 0.0004716;
	Float64 bassScaleback = 0.0002364;
	Float64 trebleScaleback = 0.0005484;
	Float64 addBassBuss = 0.000243;
	Float64 addTrebBuss = 0.000407;
	Float64 addShortBuss = 0.000326;
	IIRscaleback /= overallscale;
	bassScaleback /= overallscale;
	trebleScaleback /= overallscale;
	addBassBuss /= overallscale;
	addTrebBuss /= overallscale;
	addShortBuss /= overallscale;
	Float64 limitingBass = 0.39;
	Float64 limitingTreb = 0.6;
	Float64 limiting = 0.36;
	Float64 maxfeedBass = 0.972;
	Float64 maxfeedTreb = 0.972;
	Float64 maxfeed = 0.975;
	Float64 bridgerectifier;
	double inputSample;
	Float64 lowSample;
	Float64 highSample;
	Float64 distSample;
	Float64 minusSample;
	Float64 plusSample;
	Float64 gain = pow(10.0,GetParameter( kParam_One )/20);
	Float64 outgain = GetParameter( kParam_Two );

	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		inputSample *= gain;

		if (flip)
			{
			iirSampleA = (iirSampleA * 0.9) + (inputSample * 0.1);
			lowSample = iirSampleA;
			}
		else
			{
			iirSampleB = (iirSampleB * 0.9) + (inputSample * 0.1);
			lowSample = iirSampleB;
			}
		highSample = inputSample - lowSample;
		flip = !flip;
		//we now have two bands and the original source

		//inputSample = lowSample;
		inP2 = lowSample * lowSample;
		if (inP2 > 1.0) inP2 = 1.0; if (inP2 < -1.0) inP2 = -1.0;
		chebyshev = (2 * inP2);
		chebyshev *= basslev;
		//second harmonic max +1
		if (basslev > 0) basslev -= bassScaleback;
		if (basslev < 0) basslev += bassScaleback;
		//this is ShortBuss, IIRscaleback is the decay speed. *2 for second harmonic, and so on
		bridgerectifier = fabs(lowSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (lowSample > 0.0) distSample = bridgerectifier;
		else distSample = -bridgerectifier;
		minusSample = lowSample - distSample;
		plusSample = lowSample + distSample;
		if (minusSample > maxfeedBass) minusSample = maxfeedBass;
		if (plusSample > maxfeedBass) plusSample = maxfeedBass;
		if (plusSample < -maxfeedBass) plusSample = -maxfeedBass;
		if (minusSample < -maxfeedBass) minusSample = -maxfeedBass;
		if (lowSample > distSample) basslev += (minusSample*addBassBuss);
		if (lowSample < -distSample) basslev -= (plusSample*addBassBuss);
		if (basslev > 1.0)  basslev = 1.0;
		if (basslev < -1.0) basslev = -1.0;
		bridgerectifier = fabs(lowSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (lowSample > 0.0) lowSample = bridgerectifier;
		else lowSample = -bridgerectifier;
		//apply the distortion transform for reals
		lowSample /= (1.0+fabs(basslev*limitingBass));
		lowSample += chebyshev;
		//apply the correction measures
		
		//inputSample = highSample;
		inP2 = highSample * highSample;
		if (inP2 > 1.0) inP2 = 1.0; if (inP2 < -1.0) inP2 = -1.0;
		chebyshev = (2 * inP2);
		chebyshev *= treblev;
		//second harmonic max +1
		if (treblev > 0) treblev -= trebleScaleback;
		if (treblev < 0) treblev += trebleScaleback;
		//this is ShortBuss, IIRscaleback is the decay speed. *2 for second harmonic, and so on
		bridgerectifier = fabs(highSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (highSample > 0.0) distSample = bridgerectifier;
		else distSample = -bridgerectifier;
		minusSample = highSample - distSample;
		plusSample = highSample + distSample;
		if (minusSample > maxfeedTreb) minusSample = maxfeedTreb;
		if (plusSample > maxfeedTreb) plusSample = maxfeedTreb;
		if (plusSample < -maxfeedTreb) plusSample = -maxfeedTreb;
		if (minusSample < -maxfeedTreb) minusSample = -maxfeedTreb;
		if (highSample > distSample) treblev += (minusSample*addTrebBuss);
		if (highSample < -distSample) treblev -= (plusSample*addTrebBuss);
		if (treblev > 1.0)  treblev = 1.0;
		if (treblev < -1.0) treblev = -1.0;
		bridgerectifier = fabs(highSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (highSample > 0.0) highSample = bridgerectifier;
		else highSample = -bridgerectifier;
		//apply the distortion transform for reals
		highSample /= (1.0+fabs(treblev*limitingTreb));
		highSample += chebyshev;
		//apply the correction measures
		
		inputSample = lowSample + highSample;

		inP2 = inputSample * inputSample;
		if (inP2 > 1.0) inP2 = 1.0; if (inP2 < -1.0) inP2 = -1.0;
		chebyshev = (2 * inP2);
		chebyshev *= cheblev;
		//third harmonic max -1
		if (cheblev > 0) cheblev -= IIRscaleback;
		if (cheblev < 0) cheblev += IIRscaleback;
		//this is ShortBuss, IIRscaleback is the decay speed. *2 for second harmonic, and so on
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0) distSample = bridgerectifier;
		else distSample = -bridgerectifier;
		minusSample = inputSample - distSample;
		plusSample = inputSample + distSample;
		if (minusSample > maxfeed) minusSample = maxfeed;
		if (plusSample > maxfeed) plusSample = maxfeed;
		if (plusSample < -maxfeed) plusSample = -maxfeed;
		if (minusSample < -maxfeed) minusSample = -maxfeed;
		if (inputSample > distSample) cheblev += (minusSample*addShortBuss);
		if (inputSample < -distSample) cheblev -= (plusSample*addShortBuss);
		if (cheblev > 1.0)  cheblev = 1.0;
		if (cheblev < -1.0) cheblev = -1.0;
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//apply the distortion transform for reals
		inputSample /= (1.0+fabs(cheblev*limiting));
		inputSample += chebyshev;
		//apply the correction measures
		
		if (outgain < 1.0) {
			inputSample *= outgain;
		}
		
		if (inputSample > 0.95) inputSample = 0.95;
		if (inputSample < -0.95) inputSample = -0.95;
		//iron bar
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		//built in output trim and dry/wet by default
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	basslev = 0.0;
	treblev = 0.0;
	cheblev = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
