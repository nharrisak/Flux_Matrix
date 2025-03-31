#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Thunder"
#define AIRWINDOWS_DESCRIPTION "A compressor that retains or exaggerates subsonic bass when you push it."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','h','u' )
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Thunder", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	Float64 muVary;
	Float64 muAttack;
	Float64 muNewSpeed;
	Float64 muSpeedA;
	Float64 muSpeedB;
	Float64 muCoefficientA;
	Float64 muCoefficientB;
	Float64 gateL;
	Float64 gateR;
	Float64 iirSampleAL;
	Float64 iirSampleBL;
	Float64 iirSampleAR;
	Float64 iirSampleBR;
	Float64 iirSampleAM;
	Float64 iirSampleBM;
	Float64 iirSampleCM;
	uint32_t fpdL;
	uint32_t fpdR;
	bool flip;
	
#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	Float64 thunder = GetParameter( kParam_One ) * 0.4;
	Float64 threshold = 1.0 - (thunder * 2.0);
	if (threshold < 0.01) threshold = 0.01;
	Float64 muMakeupGain = 1.0 / threshold;
	Float64 release = pow((1.28-thunder),5)*32768.0;
	release /= overallscale;
	Float64 fastest = sqrt(release);
	Float64 EQ = ((0.0275 / GetSampleRate())*32000.0);
	Float64 dcblock = EQ / 300.0;
	Float64 basstrim = (0.01/EQ)+1.0;
	//FF parameters also ride off Speed
	Float64 outputGain = GetParameter( kParam_Two );
	
	Float64 coefficient;
	Float64 inputSense;
	
	Float64 resultL;
	Float64 resultR;
	Float64 resultM;
	Float64 resultML;
	Float64 resultMR;
	
	double inputSampleL;
	double inputSampleR;
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		inputSampleL = inputSampleL * muMakeupGain;
		inputSampleR = inputSampleR * muMakeupGain;
		
		if (gateL < fabs(inputSampleL)) gateL = inputSampleL;
		else gateL -= dcblock;
		if (gateR < fabs(inputSampleR)) gateR = inputSampleR;
		else gateR -= dcblock;
		//setting up gated DC blocking to control the tendency for rumble and offset
		
		//begin three FathomFive stages
		iirSampleAL += (inputSampleL * EQ * thunder);
		iirSampleAL -= (iirSampleAL * iirSampleAL * iirSampleAL * EQ);
		if (iirSampleAL > gateL) iirSampleAL -= dcblock;
		if (iirSampleAL < -gateL) iirSampleAL += dcblock;
		resultL = iirSampleAL*basstrim;
		iirSampleBL = (iirSampleBL * (1 - EQ)) + (resultL * EQ);
		resultL = iirSampleBL;
		
		iirSampleAR += (inputSampleR * EQ * thunder);
		iirSampleAR -= (iirSampleAR * iirSampleAR * iirSampleAR * EQ);
		if (iirSampleAR > gateR) iirSampleAR -= dcblock;
		if (iirSampleAR < -gateR) iirSampleAR += dcblock;
		resultR = iirSampleAR*basstrim;
		iirSampleBR = (iirSampleBR * (1 - EQ)) + (resultR * EQ);
		resultR = iirSampleBR;
		
		iirSampleAM += ((inputSampleL + inputSampleR) * EQ * thunder);
		iirSampleAM -= (iirSampleAM * iirSampleAM * iirSampleAM * EQ);
		resultM = iirSampleAM*basstrim;
		iirSampleBM = (iirSampleBM * (1 - EQ)) + (resultM * EQ);
		resultM = iirSampleBM;
		iirSampleCM = (iirSampleCM * (1 - EQ)) + (resultM * EQ);
		
		resultM = fabs(iirSampleCM);
		resultML = fabs(resultL);
		resultMR = fabs(resultR);
		
		if (resultM > resultML) resultML = resultM;
		if (resultM > resultMR) resultMR = resultM;
		//trying to restrict the buzziness
		
		if (resultML > 1.0) resultML = 1.0;
		if (resultMR > 1.0) resultMR = 1.0;
		//now we have result L, R and M the trigger modulator which must be 0-1
		
		//begin compressor section
		inputSampleL -= (iirSampleBL * thunder);
		inputSampleR -= (iirSampleBR * thunder);
		//highpass the comp section by sneaking out what will be the reinforcement
		
		inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		//we will take the greater of either channel and just use that, then apply the result
		//to both stereo channels.

		if (flip)
		{
			if (inputSense > threshold)
			{
				muVary = threshold / inputSense;
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
			muNewSpeed = muNewSpeed + fabs(inputSense*release)+fastest;
			muSpeedA = muNewSpeed / muSpeedA;
		}
		else
		{
			if (inputSense > threshold)
			{
				muVary = threshold / inputSense;
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
			muNewSpeed = muNewSpeed + fabs(inputSense*release)+fastest;
			muSpeedB = muNewSpeed / muSpeedB;
		}
		//got coefficients, adjusted speeds
		
		if (flip)
		{
			coefficient = pow(muCoefficientA,2);
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		else
		{
			coefficient = pow(muCoefficientB,2);
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		inputSampleL += (resultL * resultM);
		inputSampleR += (resultR * resultM);
		//combine the two by adding the summed channnel of lows
		
		if (outputGain != 1.0) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}
		flip = !flip;
				
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//don't know why we're getting a volume boost, cursed thing
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muVary = 1;
	gateL = 0.0;
	gateR = 0.0;
	iirSampleAL = 0.0;
	iirSampleBL = 0.0;
	iirSampleAR = 0.0;
	iirSampleBR = 0.0;
	iirSampleAM = 0.0;
	iirSampleBM = 0.0;
	iirSampleCM = 0.0;
	flip = false;	
	return noErr;
}

};
