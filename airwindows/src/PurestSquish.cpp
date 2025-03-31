#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestSquish"
#define AIRWINDOWS_DESCRIPTION "An open-sounding compressor with bass bloom."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','$' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Squish", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Bloom", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 lastCoefficientA;
		Float64 lastCoefficientB;
		Float64 lastCoefficientC;
		Float64 lastCoefficientD;
		Float64 mergedCoefficients;
		int count;
		bool fpFlip;
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
	
	double highpassSample;
	double highpassSampleB;
	Float64 threshold = 1.01 - (1.0-pow(1.0-(GetParameter( kParam_One )*0.5),4));
	Float64 iirAmount = pow(GetParameter( kParam_Two ),4)/overallscale;
	Float64 output = GetParameter( kParam_Three );
	Float64 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		Float64 muMakeupGain = 1.0 / threshold;
		Float64 outMakeupGain = sqrt(muMakeupGain);
		muMakeupGain += outMakeupGain;
		muMakeupGain *= 0.5;
		outMakeupGain *= 0.5;
		//gain settings around threshold
		Float64 release = mergedCoefficients * 32768.0;
		release /= overallscale;
		Float64 fastest = sqrt(release);
		//speed settings around release
		Float64 lastCorrection = mergedCoefficients;
		// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
		
		if (muMakeupGain != 1.0) inputSample = inputSample * muMakeupGain;
		
		highpassSampleB = highpassSample = inputSample;
		
		if (count < 1 || count > 3) count = 1;
		switch (count)
		{
			case 1:
				iirSampleA = (iirSampleA * (1 - iirAmount)) + (highpassSample * iirAmount);
				highpassSample -= iirSampleA;
				if (fabs(highpassSample) > threshold)
				{
					muVary = threshold / fabs(highpassSample);
					muAttack = sqrt(fabs(muSpeedA));
					muCoefficientA = muCoefficientA * (muAttack-1.0);
					if (muVary < threshold)
					{
						muCoefficientA = muCoefficientA + threshold;
					}
					else
					{
						muCoefficientA = muCoefficientA + muVary;
					}
					muCoefficientA = muCoefficientA / muAttack;
				}
				else
				{
					muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA)-1.0);
					muCoefficientA = muCoefficientA + 1.0;
					muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
				}
				muNewSpeed = muSpeedA * (muSpeedA-1);
				muNewSpeed = muNewSpeed + fabs(highpassSample*release)+fastest;
				muSpeedA = muNewSpeed / muSpeedA;
				lastCoefficientA = pow(muCoefficientA,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0 - lastCorrection);
				lastCoefficientA += (muCoefficientA * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
			case 2:
				iirSampleB = (iirSampleB * (1 - iirAmount)) + (highpassSample * iirAmount);
				highpassSample -= iirSampleB;
				if (fabs(highpassSample) > threshold)
				{
					muVary = threshold / fabs(highpassSample);
					muAttack = sqrt(fabs(muSpeedB));
					muCoefficientB = muCoefficientB * (muAttack-1);
					if (muVary < threshold)
					{
						muCoefficientB = muCoefficientB + threshold;
					}
					else
					{
						muCoefficientB = muCoefficientB + muVary;
					}
					muCoefficientB = muCoefficientB / muAttack;
				}
				else
				{
					muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB)-1.0);
					muCoefficientB = muCoefficientB + 1.0;
					muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
				}
				muNewSpeed = muSpeedB * (muSpeedB-1);
				muNewSpeed = muNewSpeed + fabs(highpassSample*release)+fastest;
				muSpeedB = muNewSpeed / muSpeedB;
				lastCoefficientA = pow(muCoefficientB,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0 - lastCorrection);
				lastCoefficientA += (muCoefficientB * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
			case 3:
				iirSampleC = (iirSampleC * (1 - iirAmount)) + (highpassSample * iirAmount);
				highpassSample -= iirSampleC;
				if (fabs(highpassSample) > threshold)
				{
					muVary = threshold / fabs(highpassSample);
					muAttack = sqrt(fabs(muSpeedC));
					muCoefficientC = muCoefficientC * (muAttack-1);
					if (muVary < threshold)
					{
						muCoefficientC = muCoefficientC + threshold;
					}
					else
					{
						muCoefficientC = muCoefficientC + muVary;
					}
					muCoefficientC = muCoefficientC / muAttack;
				}
				else
				{
					muCoefficientC = muCoefficientC * ((muSpeedC * muSpeedC)-1.0);
					muCoefficientC = muCoefficientC + 1.0;
					muCoefficientC = muCoefficientC / (muSpeedC * muSpeedC);
				}
				muNewSpeed = muSpeedC * (muSpeedC-1);
				muNewSpeed = muNewSpeed + fabs(highpassSample*release)+fastest;
				muSpeedC = muNewSpeed / muSpeedC;
				lastCoefficientA = pow(muCoefficientC,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0 - lastCorrection);
				lastCoefficientA += (muCoefficientC * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
		}		
		count++;
		
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (fpFlip) {
			iirSampleD = (iirSampleD * (1 - iirAmount)) + (highpassSampleB * iirAmount);
			highpassSampleB -= iirSampleD;
			if (fabs(highpassSampleB) > threshold)
			{
				muVary = threshold / fabs(highpassSampleB);
				muAttack = sqrt(fabs(muSpeedD));
				muCoefficientD = muCoefficientD * (muAttack-1.0);
				if (muVary < threshold)
				{
					muCoefficientD = muCoefficientD + threshold;
				}
				else
				{
					muCoefficientD = muCoefficientD + muVary;
				}
				muCoefficientD = muCoefficientD / muAttack;
			}
			else
			{
				muCoefficientD = muCoefficientD * ((muSpeedD * muSpeedD)-1.0);
				muCoefficientD = muCoefficientD + 1.0;
				muCoefficientD = muCoefficientD / (muSpeedD * muSpeedD);
			}
			muNewSpeed = muSpeedD * (muSpeedD-1);
			muNewSpeed = muNewSpeed + fabs(highpassSampleB*release)+fastest;
			muSpeedD = muNewSpeed / muSpeedD;
			lastCoefficientC = pow(muCoefficientE,2);
			mergedCoefficients += lastCoefficientD;
			mergedCoefficients += lastCoefficientC;
			lastCoefficientC *= (1.0 - lastCorrection);
			lastCoefficientC += (muCoefficientD * lastCorrection);
			lastCoefficientD = lastCoefficientC;
		} else {
			iirSampleE = (iirSampleE * (1 - iirAmount)) + (highpassSampleB * iirAmount);
			highpassSampleB -= iirSampleE;
			if (fabs(highpassSampleB) > threshold)
			{
				muVary = threshold / fabs(highpassSampleB);
				muAttack = sqrt(fabs(muSpeedE));
				muCoefficientE = muCoefficientE * (muAttack-1.0);
				if (muVary < threshold)
				{
					muCoefficientE = muCoefficientE + threshold;
				}
				else
				{
					muCoefficientE = muCoefficientE + muVary;
				}
				muCoefficientE = muCoefficientE / muAttack;
			}
			else
			{
				muCoefficientE = muCoefficientE * ((muSpeedE * muSpeedE)-1.0);
				muCoefficientE = muCoefficientE + 1.0;
				muCoefficientE = muCoefficientE / (muSpeedE * muSpeedE);
			}
			muNewSpeed = muSpeedE * (muSpeedE-1);
			muNewSpeed = muNewSpeed + fabs(highpassSampleB*release)+fastest;
			muSpeedE = muNewSpeed / muSpeedE;
			lastCoefficientC = pow(muCoefficientE,2);
			mergedCoefficients += lastCoefficientD;
			mergedCoefficients += lastCoefficientC;
			lastCoefficientC *= (1.0 - lastCorrection);
			lastCoefficientC += (muCoefficientE * lastCorrection);
			lastCoefficientD = lastCoefficientC;
		}
		mergedCoefficients *= 0.25;
		inputSample *= mergedCoefficients;
		
		if (outMakeupGain != 1.0) inputSample = inputSample * outMakeupGain;
		
		fpFlip = !fpFlip;
		
		if (output < 1.0) {
			inputSample *= output;
		}
		
		if (wet < 1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		
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
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	lastCoefficientA = 1;
	lastCoefficientB = 1;
	lastCoefficientC = 1;
	lastCoefficientD = 1;
	mergedCoefficients = 1;
	muVary = 1;
	count = 1;
	fpFlip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
