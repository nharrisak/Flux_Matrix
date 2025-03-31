#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pop2"
#define AIRWINDOWS_DESCRIPTION "Adds control and punch to Pop."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','q' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Compress", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		double muVary;
		double muAttack;
		double muNewSpeed;
		double muSpeedA;
		double muSpeedB;
		double muCoefficientA;
		double muCoefficientB;
		bool flip;
		
		double lastSample;
		double intermediate[16];
		bool wasPosClip;
		bool wasNegClip;
		
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16; //for ClipOnly2
	
	Float64 threshold = 1.0 - ((1.0-pow(1.0-GetParameter( kParam_One ),2))*0.9);
	Float64 attack = ((pow(GetParameter( kParam_Two ),4)*100000.0)+10.0)*overallscale;
	Float64 release = ((pow(GetParameter( kParam_Three ),5)*2000000.0)+20.0)*overallscale;
	Float64 maxRelease = release * 4.0;
	Float64 muPreGain = 1.0/threshold;
	Float64 muMakeupGain = sqrt(1.0 / threshold)*GetParameter( kParam_Four );
	Float64 wet = GetParameter( kParam_Five );
	//compressor section
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		//begin compressor section
		inputSample *= muPreGain;
		if (flip) {
			if (fabs(inputSample) > threshold) {
				muVary = threshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedA));
				muCoefficientA = muCoefficientA * (muAttack-1.0);
				if (muVary < threshold) muCoefficientA = muCoefficientA + threshold;
				else  muCoefficientA = muCoefficientA + muVary;
				muCoefficientA = muCoefficientA / muAttack;
				muNewSpeed = muSpeedA * (muSpeedA-1.0);
				muNewSpeed = muNewSpeed + release;
				muSpeedA = muNewSpeed / muSpeedA;
				if (muSpeedA > maxRelease) muSpeedA = maxRelease;
			} else {
				muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA)-1.0);
				muCoefficientA = muCoefficientA + 1.0;
				muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
				muNewSpeed = muSpeedA * (muSpeedA-1.0);
				muNewSpeed = muNewSpeed + attack;
				muSpeedA = muNewSpeed / muSpeedA;}
		} else {
			if (fabs(inputSample) > threshold) {
				muVary = threshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedB));
				muCoefficientB = muCoefficientB * (muAttack-1.0);
				if (muVary < threshold) muCoefficientB = muCoefficientB + threshold;
				else muCoefficientB = muCoefficientB + muVary;
				muCoefficientB = muCoefficientB / muAttack;
				muNewSpeed = muSpeedB * (muSpeedB-1.0);
				muNewSpeed = muNewSpeed + release;
				muSpeedB = muNewSpeed / muSpeedB;
				if (muSpeedB > maxRelease) muSpeedB = maxRelease;
			} else {
				muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB)-1.0);
				muCoefficientB = muCoefficientB + 1.0;
				muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
				muNewSpeed = muSpeedB * (muSpeedB-1.0);
				muNewSpeed = muNewSpeed + attack;
				muSpeedB = muNewSpeed / muSpeedB;}
		}
		//got coefficients, adjusted speeds
		if (flip) inputSample *= pow(muCoefficientA,2);
		else inputSample *= pow(muCoefficientB,2);
		inputSample *= muMakeupGain;
		flip = !flip;
		//end compressor section
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (inputSample > 4.0) inputSample = 4.0; if (inputSample < -4.0) inputSample = -4.0;
		if (wasPosClip == true) { //current will be over
			if (inputSample<lastSample) lastSample=0.7058208+(inputSample*0.2609148);
			else lastSample = 0.2491717+(lastSample*0.7390851);
		} wasPosClip = false;
		if (inputSample>0.9549925859) {wasPosClip=true;inputSample=0.7058208+(lastSample*0.2609148);}
		if (wasNegClip == true) { //current will be -over
			if (inputSample > lastSample) lastSample=-0.7058208+(inputSample*0.2609148);
			else lastSample=-0.2491717+(lastSample*0.7390851);
		} wasNegClip = false;
		if (inputSample<-0.9549925859) {wasNegClip=true;inputSample=-0.7058208+(lastSample*0.2609148);}
		intermediate[spacing] = inputSample;
        inputSample = lastSample; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediate[x-1] = intermediate[x];
		lastSample = intermediate[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider

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
	muVary = 0.0;
	muAttack = 0.0;
	muNewSpeed = 1000.0;
	muSpeedA = 1000.0;
	muSpeedB = 1000.0;
	muCoefficientA = 1.0;
	muCoefficientB = 1.0;
	flip = false;
	
	lastSample = 0.0;
	wasPosClip = false;
	wasNegClip = false;
	for (int x = 0; x < 16; x++) intermediate[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
