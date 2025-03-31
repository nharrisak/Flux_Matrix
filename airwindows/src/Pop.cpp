#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pop"
#define AIRWINDOWS_DESCRIPTION "A crazy overcompressor with a very exaggerated sound."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','p' )
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
{ .name = "Intensity", .min = 0, .max = 1000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		double muVary;
		double muAttack;
		double muNewSpeed;
		double muSpeedA;
		double muSpeedB;
		double muCoefficientA;
		double muCoefficientB;
		double thicken;
		double previous;
		double previous2;
		double previous3;
		double previous4;
		double previous5;
		int delay;
		bool flip;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 d[10001];
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	Float64 highGainOffset = pow(GetParameter( kParam_One ),2)*0.023;
	Float64 threshold = 1.001 - (1.0-pow(1.0-GetParameter( kParam_One ),5));
	Float64 muMakeupGain = sqrt(1.0 / threshold);
	//gain settings around threshold
	Float64 release = (GetParameter( kParam_One )*100000.0) + 300000.0;
	int maxdelay = (int)(1450.0 * overallscale);
	if (maxdelay > 9999) maxdelay = 9999;
	release /= overallscale;
	Float64 fastest = sqrt(release);
	//speed settings around release
	Float64 output = GetParameter( kParam_Two );
	Float64 wet = GetParameter( kParam_Three );
	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
	
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		dram->d[delay] = inputSample;
		delay--;
		if (delay < 0 || delay > maxdelay) {delay = maxdelay;}
		//yes this is a second bounds check. it's cheap, check EVERY time
		inputSample = (inputSample * thicken) + (dram->d[delay] * (1.0-thicken));
		
		double lowestSample = inputSample;
		if (fabs(inputSample) > fabs(previous)) lowestSample = previous;
		if (fabs(lowestSample) > fabs(previous2)) lowestSample = (lowestSample + previous2) / 1.99;
		if (fabs(lowestSample) > fabs(previous3)) lowestSample = (lowestSample + previous3) / 1.98;
		if (fabs(lowestSample) > fabs(previous4)) lowestSample = (lowestSample + previous4) / 1.97;
		if (fabs(lowestSample) > fabs(previous5)) lowestSample = (lowestSample + previous5) / 1.96;
		previous5 = previous4;
		previous4 = previous3;
		previous3 = previous2;
		previous2 = previous;
		previous = inputSample;
		inputSample *= muMakeupGain;
		Float64 punchiness = 0.95-fabs(inputSample*0.08);
		if (punchiness < 0.65) punchiness = 0.65;

		
		if (flip)
			{
				if (fabs(lowestSample) > threshold)
					{
						muVary = threshold / fabs(lowestSample);
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
				muNewSpeed = muNewSpeed + fabs(lowestSample*release)+fastest;
				muSpeedA = muNewSpeed / muSpeedA;
			}
		else
			{
				if (fabs(lowestSample) > threshold)
					{
						muVary = threshold / fabs(lowestSample);
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
				muNewSpeed = muNewSpeed + fabs(lowestSample*release)+fastest;
				muSpeedB = muNewSpeed / muSpeedB;
			}
		//got coefficients, adjusted speeds
		double coefficient = highGainOffset;
		if (flip) coefficient += pow(muCoefficientA,2);
		else coefficient += pow(muCoefficientB,2);
		inputSample *= coefficient;
		thicken = (coefficient/5)+punchiness;//0.80;
		thicken = (1.0-wet)+(wet*thicken);
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		double bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.2533141373155) bridgerectifier = 1.2533141373155;
		bridgerectifier = sin(bridgerectifier * fabs(bridgerectifier)) / ((fabs(bridgerectifier) == 0.0) ?1:fabs(bridgerectifier));
		//using Spiral instead of Density algorithm
		if (inputSample > 0) inputSample = (inputSample*coefficient)+(bridgerectifier*(1-coefficient));
		else inputSample = (inputSample*coefficient)-(bridgerectifier*(1-coefficient));
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		flip = !flip;
		
		if (output < 1.0) inputSample *= output;
		if (wet < 1.0) inputSample = (drySample*(1.0-wet))+(inputSample*wet);
		
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
	for(int count = 0; count < 10000; count++) {dram->d[count] = 0;}
	delay = 0;
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	thicken = 1;
	muVary = 1;
	flip = false;
	previous = 0.0;
	previous2 = 0.0;
	previous3 = 0.0;
	previous4 = 0.0;
	previous5 = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
