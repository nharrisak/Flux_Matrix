#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BlockParty"
#define AIRWINDOWS_DESCRIPTION "Like a moderately saturated analog limiter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','l','o' )
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
{ .name = "Pound", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 muVary;
		Float64 muAttack;
		Float64 muNewSpeed;
		Float64 muSpeedA;
		Float64 muSpeedB;
		Float64 muSpeedC;
		Float64 muSpeedD;
		Float64 muSpeedE;
		Float64 muCoefficientA;
		Float64 muCoefficientB;
		Float64 muCoefficientC;
		Float64 muCoefficientD;
		Float64 muCoefficientE;
		Float64 lastCoefficientA;
		Float64 lastCoefficientB;
		Float64 lastCoefficientC;
		Float64 lastCoefficientD;
		Float64 mergedCoefficients;
		Float64 threshold;
		Float64 thresholdB;
		int count;
		bool fpFlip;
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	Float64 targetthreshold = 1.01 - (1.0-pow(1.0-(GetParameter( kParam_One )*0.5),4));
	Float64 wet = GetParameter( kParam_Two );
	Float64 voicing = 0.618033988749894848204586;
	if (overallscale > 0.0) voicing /= overallscale;
	//translate to desired sample rate, 44.1K is the base
	if (voicing < 0.0) voicing = 0.0;
	if (voicing > 1.0) voicing = 1.0;
	//some insanity checking
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		Float64 muMakeupGain = 1.0 / threshold;
		Float64 outMakeupGain = sqrt(muMakeupGain);
		muMakeupGain += outMakeupGain;
		muMakeupGain *= 0.5;
		//gain settings around threshold
		Float64 release = mergedCoefficients * 32768.0;
		release /= overallscale;
		Float64 fastest = sqrt(release);
		//speed settings around release
		Float64 lastCorrection = mergedCoefficients;
		// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
		
		if (muMakeupGain != 1.0) inputSample = inputSample * muMakeupGain;
				
		if (count < 1 || count > 3) count = 1;
		switch (count)
		{
			case 1:
				if (fabs(inputSample) > threshold)
				{
					if (inputSample > 0.0) {
						inputSample = (inputSample * voicing) + (targetthreshold * (1.0-voicing));
						threshold = fabs(inputSample);
					} else {
						inputSample = (inputSample * voicing) - (targetthreshold * (1.0-voicing));
						threshold = fabs(inputSample);
					}
					muVary = targetthreshold / fabs(inputSample);
					muAttack = sqrt(fabs(muSpeedA));
					muCoefficientA = muCoefficientA * (muAttack-1.0);
					if (muVary < threshold)
					{
						muCoefficientA = muCoefficientA + targetthreshold;
					}
					else
					{
						muCoefficientA = muCoefficientA + muVary;
					}
					muCoefficientA = muCoefficientA / muAttack;
				}
				else
				{
					threshold = targetthreshold;
					muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA)-1.0);
					muCoefficientA = muCoefficientA + 1.0;
					muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
				}
				muNewSpeed = muSpeedA * (muSpeedA-1);
				muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
				muSpeedA = muNewSpeed / muSpeedA;
				lastCoefficientA = pow(muCoefficientA,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0-lastCorrection);
				lastCoefficientA += (muCoefficientA * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
			case 2:
				if (fabs(inputSample) > threshold)
				{
					if (inputSample > 0.0) {
						inputSample = (inputSample * voicing) + (targetthreshold * (1.0-voicing));
						threshold = fabs(inputSample);
					} else {
						inputSample = (inputSample * voicing) - (targetthreshold * (1.0-voicing));
						threshold = fabs(inputSample);
					}
					muVary = targetthreshold / fabs(inputSample);
					muAttack = sqrt(fabs(muSpeedB));
					muCoefficientB = muCoefficientB * (muAttack-1);
					if (muVary < threshold)
					{
						muCoefficientB = muCoefficientB + targetthreshold;
					}
					else
					{
						muCoefficientB = muCoefficientB + muVary;
					}
					muCoefficientB = muCoefficientB / muAttack;
				}
				else
				{
					threshold = targetthreshold;
					muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB)-1.0);
					muCoefficientB = muCoefficientB + 1.0;
					muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
				}
				muNewSpeed = muSpeedB * (muSpeedB-1);
				muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
				muSpeedB = muNewSpeed / muSpeedB;
				lastCoefficientA = pow(muCoefficientB,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0-lastCorrection);
				lastCoefficientA += (muCoefficientB * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
			case 3:
				if (fabs(inputSample) > threshold)
				{
					if (inputSample > 0.0) {
						inputSample = (inputSample * voicing) + (targetthreshold * (1.0-voicing));
						threshold = fabs(inputSample);
					} else {
						inputSample = (inputSample * voicing) - (targetthreshold * (1.0-voicing));
						threshold = fabs(inputSample);
					}
					muVary = targetthreshold / fabs(inputSample);
					muAttack = sqrt(fabs(muSpeedC));
					muCoefficientC = muCoefficientC * (muAttack-1);
					if (muVary < threshold)
					{
						muCoefficientC = muCoefficientC + targetthreshold;
					}
					else
					{
						muCoefficientC = muCoefficientC + muVary;
					}
					muCoefficientC = muCoefficientC / muAttack;
				}
				else
				{
					threshold = targetthreshold;
					muCoefficientC = muCoefficientC * ((muSpeedC * muSpeedC)-1.0);
					muCoefficientC = muCoefficientC + 1.0;
					muCoefficientC = muCoefficientC / (muSpeedC * muSpeedC);
				}
				muNewSpeed = muSpeedC * (muSpeedC-1);
				muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
				muSpeedC = muNewSpeed / muSpeedC;
				lastCoefficientA = pow(muCoefficientC,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0-lastCorrection);
				lastCoefficientA += (muCoefficientC * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
		}		
		count++;
		
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (fpFlip) {
			if (fabs(inputSample) > thresholdB)
			{
				if (inputSample > 0.0) {
					inputSample = (inputSample * voicing) + (targetthreshold * (1.0-voicing));
					thresholdB = fabs(inputSample);
				} else {
					inputSample = (inputSample * voicing) - (targetthreshold * (1.0-voicing));
					thresholdB = fabs(inputSample);
				}
				muVary = targetthreshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedD));
				muCoefficientD = muCoefficientD * (muAttack-1.0);
				if (muVary < thresholdB)
				{
					muCoefficientD = muCoefficientD + targetthreshold;
				}
				else
				{
					muCoefficientD = muCoefficientD + muVary;
				}
				muCoefficientD = muCoefficientD / muAttack;
			}
			else
			{
				thresholdB = targetthreshold;
				muCoefficientD = muCoefficientD * ((muSpeedD * muSpeedD)-1.0);
				muCoefficientD = muCoefficientD + 1.0;
				muCoefficientD = muCoefficientD / (muSpeedD * muSpeedD);
			}
			muNewSpeed = muSpeedD * (muSpeedD-1);
			muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
			muSpeedD = muNewSpeed / muSpeedD;
			lastCoefficientC = pow(muCoefficientE,2);
			mergedCoefficients += lastCoefficientD;
			mergedCoefficients += lastCoefficientC;
			lastCoefficientC *= (1.0-lastCorrection);
			lastCoefficientC += (muCoefficientD * lastCorrection);
			lastCoefficientD = lastCoefficientC;
		} else {
			if (fabs(inputSample) > thresholdB)
			{
				if (inputSample > 0.0) {
					inputSample = (inputSample * voicing) + (targetthreshold * (1.0-voicing));
					thresholdB = fabs(inputSample);
				} else {
					inputSample = (inputSample * voicing) - (targetthreshold * (1.0-voicing));
					thresholdB = fabs(inputSample);
				}
				muVary = targetthreshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedE));
				muCoefficientE = muCoefficientE * (muAttack-1.0);
				if (muVary < thresholdB)
				{
					muCoefficientE = muCoefficientE + targetthreshold;
				}
				else
				{
					muCoefficientE = muCoefficientE + muVary;
				}
				muCoefficientE = muCoefficientE / muAttack;
			}
			else
			{
				thresholdB = targetthreshold;
				muCoefficientE = muCoefficientE * ((muSpeedE * muSpeedE)-1.0);
				muCoefficientE = muCoefficientE + 1.0;
				muCoefficientE = muCoefficientE / (muSpeedE * muSpeedE);
			}
			muNewSpeed = muSpeedE * (muSpeedE-1);
			muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
			muSpeedE = muNewSpeed / muSpeedE;
			lastCoefficientC = pow(muCoefficientE,2);
			mergedCoefficients += lastCoefficientD;
			mergedCoefficients += lastCoefficientC;
			lastCoefficientC *= (1.0-lastCorrection);
			lastCoefficientC += (muCoefficientE * lastCorrection);
			lastCoefficientD = lastCoefficientC;
		}
		mergedCoefficients *= 0.25;
		inputSample *= mergedCoefficients;
		
		if (outMakeupGain != 1.0) inputSample = inputSample * outMakeupGain;
		
		fpFlip = !fpFlip;
		
		if (wet < 1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		
		if (inputSample > 0.999) inputSample = 0.999;
		if (inputSample < -0.999) inputSample = -0.999;
		//iron bar clip comes after the dry/wet: alternate way to clean things up
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += static_cast<int32_t>(fpd) * 5.960464655174751e-36L * pow(2,expon+62);
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	muSpeedA = 10000;
	muSpeedB = 10000;
	muSpeedC = 10000;
	muSpeedD = 10000;
	muSpeedE = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muCoefficientC = 1;
	muCoefficientD = 1;
	muCoefficientE = 1;
	lastCoefficientA = 1;
	lastCoefficientB = 1;
	lastCoefficientC = 1;
	lastCoefficientD = 1;
	mergedCoefficients = 1;
	threshold = 1.0;
	thresholdB = 1.0;
	muVary = 1;
	count = 1;
	fpFlip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
