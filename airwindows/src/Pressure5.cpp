#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pressure5"
#define AIRWINDOWS_DESCRIPTION "Expands Pressure4 with after-boost and built-in ClipOnly2."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','i' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pressure", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mewiness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "PawsClaws", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	Float64 muVary;
	Float64 muAttack;
	Float64 muNewSpeed;
	Float64 muSpeedA;
	Float64 muSpeedB;
	Float64 muCoefficientA;
	Float64 muCoefficientB;
	bool flip; //Pressure
	
	enum {
		fix_freq,
		fix_reso,
		fix_a0,
		fix_a1,
		fix_a2,
		fix_b1,
		fix_b2,
		fix_sL1,
		fix_sL2,
		fix_sR1,
		fix_sR2,
		fix_lastSampleL,
		fix_lastSampleR,
		fix_total
	};
	double fixA[fix_total];
	double fixB[fix_total]; //fixed frequency biquad filter for ultrasonics, stereo
	
	double lastSampleL;
	double intermediateL[16];
	bool wasPosClipL;
	bool wasNegClipL;
	double lastSampleR;
	double intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly2
	
	double slewMax; //to adust mewiness
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
		
	double threshold = 1.0 - (GetParameter( kParam_One ) * 0.95);
	double muMakeupGain = 1.0 / threshold;
	//gain settings around threshold
	double release = pow((1.28-GetParameter( kParam_Two )),5)*32768.0;
	double fastest = sqrt(release);
	release /= overallscale;
	fastest /= overallscale;
	//speed settings around release
	double mewinessRef = GetParameter( kParam_Three );
	double pawsClaws = -(GetParameter( kParam_Four )-0.5)*1.618033988749894848204586;
	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
	double outputGain = pow(GetParameter( kParam_Five )*2.0,2); //max 4.0 gain
	double wet = GetParameter( kParam_Six );
	
	fixA[fix_freq] = 24000.0 / GetSampleRate();
    fixA[fix_reso] = 0.7071; //butterworth Q
	double K = tan(M_PI * fixA[fix_freq]);
	double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
	fixA[fix_a0] = K * K * norm;
	fixA[fix_a1] = 2.0 * fixA[fix_a0];
	fixA[fix_a2] = fixA[fix_a0];
	fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
	//for the fixed-position biquad filter
	for (int x = 0; x < fix_sL1; x++) fixB[x] = fixA[x];
	//make the second filter same as the first, don't use sample slots

	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		inputSampleL = inputSampleL * muMakeupGain;
		inputSampleR = inputSampleR * muMakeupGain;
		
		if (fixA[fix_freq] < 0.4999) {
			double temp = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
			fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sL2];
			fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (temp * fixA[fix_b2]);
			inputSampleL = temp;
			temp = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
			fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sR2];
			fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (temp * fixA[fix_b2]);
			inputSampleR = temp; //fixed biquad filtering ultrasonics before Pressure
		}
		
		double inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		//we will take the greater of either channel and just use that, then apply the result
		//to both stereo channels.
		
		double mewiness = sin(mewinessRef + (slewMax * pawsClaws));
		bool positivemu = true; if (mewiness < 0) {positivemu = false; mewiness = -mewiness;}
		
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
		
		double coefficient;
		if (flip) {
			if (positivemu) coefficient = pow(muCoefficientA,2);
			else coefficient = sqrt(muCoefficientA);
			coefficient = (coefficient*mewiness)+(muCoefficientA*(1.0-mewiness));
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		} else {
			if (positivemu) coefficient = pow(muCoefficientB,2);
			else coefficient = sqrt(muCoefficientB);
			coefficient = (coefficient*mewiness)+(muCoefficientB*(1.0-mewiness));
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (outputGain != 1.0) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}		
		
		flip = !flip;
				
		if (fixB[fix_freq] < 0.49999) {
			double temp = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
			fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sL2];
			fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (temp * fixB[fix_b2]);
			inputSampleL = temp;
			temp = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
			fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sR2];
			fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (temp * fixB[fix_b2]);
			inputSampleR = temp; //fixed biquad filtering ultrasonics between Pressure and ClipOnly
		}
		
		if (wet != 1.0) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0-wet));
		}
		//Dry/Wet control, BEFORE ClipOnly
		
		slewMax = fabs(inputSampleL - lastSampleL);
		if (slewMax < fabs(inputSampleR - lastSampleR)) slewMax = fabs(inputSampleR - lastSampleR);
		//set up for fiddling with mewiness. Largest amount of slew in any direction
		
		//begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0; if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=0.7058208+(inputSampleL*0.2609148);
			else lastSampleL = 0.2491717+(lastSampleL*0.7390851);
		} wasPosClipL = false;
		if (inputSampleL>0.9549925859) {wasPosClipL=true;inputSampleL=0.7058208+(lastSampleL*0.2609148);}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=-0.7058208+(inputSampleL*0.2609148);
			else lastSampleL=-0.2491717+(lastSampleL*0.7390851);
		} wasNegClipL = false;
		if (inputSampleL<-0.9549925859) {wasNegClipL=true;inputSampleL=-0.7058208+(lastSampleL*0.2609148);}
		intermediateL[spacing] = inputSampleL;
        inputSampleL = lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateL[x-1] = intermediateL[x];
		lastSampleL = intermediateL[0]; //run a little buffer to handle this
		
		if (inputSampleR > 4.0) inputSampleR = 4.0; if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (wasPosClipR == true) { //current will be over
			if (inputSampleR<lastSampleR) lastSampleR=0.7058208+(inputSampleR*0.2609148);
			else lastSampleR = 0.2491717+(lastSampleR*0.7390851);
		} wasPosClipR = false;
		if (inputSampleR>0.9549925859) {wasPosClipR=true;inputSampleR=0.7058208+(lastSampleR*0.2609148);}
		if (wasNegClipR == true) { //current will be -over
			if (inputSampleR > lastSampleR) lastSampleR=-0.7058208+(inputSampleR*0.2609148);
			else lastSampleR=-0.2491717+(lastSampleR*0.7390851);
		} wasNegClipR = false;
		if (inputSampleR<-0.9549925859) {wasNegClipR=true;inputSampleR=-0.7058208+(lastSampleR*0.2609148);}
		intermediateR[spacing] = inputSampleR;
        inputSampleR = lastSampleR; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateR[x-1] = intermediateR[x];
		lastSampleR = intermediateR[0]; //run a little buffer to handle this
		//end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		//final clip runs AFTER the Dry/Wet. It serves as a safety clip even if you're not full wet

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
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muVary = 1;
	flip = false;
	for (int x = 0; x < fix_total; x++) {fixA[x] = 0.0; fixB[x] = 0.0;}
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0; intermediateR[x] = 0.0;}
	slewMax = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
