#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "VariMu"
#define AIRWINDOWS_DESCRIPTION "A more organic variation on Pressure (a compressor)"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','a','r' )
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
{ .name = "Intensity", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		Float64 muCoefficientA;
		Float64 muCoefficientB;
		Float64 previous;
		bool flip;
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
	Float64 overallscale = 2.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 threshold = 1.001 - (1.0-pow(1.0-GetParameter( kParam_One ),3));
	Float64 muMakeupGain = sqrt(1.0 / threshold);
	muMakeupGain = (muMakeupGain + sqrt(muMakeupGain))/2.0;
	muMakeupGain = sqrt(muMakeupGain);
	Float64 outGain = sqrt(muMakeupGain);
	//gain settings around threshold
	Float64 release = pow((1.15-GetParameter( kParam_Two )),5)*32768.0;
	release /= overallscale;
	Float64 fastest = sqrt(release);
	//speed settings around release
	Float64 coefficient;
	Float64 output = outGain * GetParameter( kParam_Three );
	Float64 wet = GetParameter( kParam_Four );
	double squaredSample;

	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		if (fabs(inputSample) > fabs(previous)) squaredSample = previous * previous;
		else squaredSample = inputSample * inputSample;
		previous = inputSample;
		inputSample *= muMakeupGain;
		
		if (flip)
			{
				if (fabs(squaredSample) > threshold)
					{
						muVary = threshold / fabs(squaredSample);
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
				muNewSpeed = muNewSpeed + fabs(squaredSample*release)+fastest;
				muSpeedA = muNewSpeed / muSpeedA;
			}
		else
			{
				if (fabs(squaredSample) > threshold)
					{
						muVary = threshold / fabs(squaredSample);
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
				muNewSpeed = muNewSpeed + fabs(squaredSample*release)+fastest;
				muSpeedB = muNewSpeed / muSpeedB;
			}
		//got coefficients, adjusted speeds

		if (flip)
			{
			coefficient = (muCoefficientA + pow(muCoefficientA,2))/2.0;
			inputSample *= coefficient;
			}
		else
			{
			coefficient = (muCoefficientB + pow(muCoefficientB,2))/2.0;
			inputSample *= coefficient;
			}
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		flip = !flip;		

		if (output < 1.0) {
			inputSample *= output;
		}
		if (wet < 1.0) {
			inputSample = (drySample * (1.0-wet)) + (inputSample * wet);
		}
		//nice little output stage template: if we have another scale of floating point
		//number, we really don't want to meaninglessly multiply that by 1.0.
		
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
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muVary = 1;
	flip = false;
	previous = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
