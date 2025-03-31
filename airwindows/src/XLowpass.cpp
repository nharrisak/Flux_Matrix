#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "XLowpass"
#define AIRWINDOWS_DESCRIPTION "A distorted digital EQ, inspired by retro sampler DSP."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','X','L','o' )
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
{ .name = "Gain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Nuke", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double biquad[11];
		double biquadA[11];
		double biquadB[11];
		double biquadC[11];
		double biquadD[11];
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

	Float64 gain = pow(GetParameter( kParam_One )+0.5,4);
	biquadA[0] = (pow(GetParameter( kParam_Two ),2)*20000.0)/GetSampleRate();
	if (biquadA[0] < 0.001) biquadA[0] = 0.001;
	
	Float64 compensation = sqrt(biquadA[0])*6.4;
	
	Float64 clipFactor = 1.0+(biquadA[0]*biquadA[0]*GetParameter( kParam_Three )*32.0);

	double K = tan(M_PI * biquadA[0]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0 * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;
	
	for (int x = 0; x < 7; x++) {biquad[x] = biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x];}
	
	Float64 aWet = 1.0;
	Float64 bWet = 1.0;
	Float64 cWet = 1.0;
	Float64 dWet = GetParameter( kParam_Three )*4.0;
	Float64 wet = GetParameter( kParam_Four );
	
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0) {aWet = dWet; bWet = 0.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 2.0) {bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 3.0) {cWet = dWet - 2.0; dWet = 0.0;}
	else {dWet -= 3.0;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	double outSample = 0.0;
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		if (gain != 1.0) {
			inputSample *= gain;
		}
		
		double nukeLevel = inputSample;
		
		outSample = biquad[2]*inputSample+biquad[3]*biquad[7]+biquad[4]*biquad[8]-biquad[5]*biquad[9]-biquad[6]*biquad[10];
		biquad[8] = biquad[7]; biquad[7] = inputSample; biquad[10] = biquad[9];
		outSample *= clipFactor;
		if (outSample > 1.57079633) outSample = 1.57079633;
		if (outSample < -1.57079633) outSample = -1.57079633;
		biquad[9] = sin(outSample); //DF1
		inputSample = outSample / compensation; nukeLevel = inputSample;
		
		if (aWet > 0.0) {
			outSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; biquadA[10] = biquadA[9];
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			biquadA[9] = sin(outSample); //DF1
			inputSample = outSample / compensation; inputSample = (inputSample * aWet) + (nukeLevel * (1.0-aWet));
			nukeLevel = inputSample;
		}
		if (bWet > 0.0) {
			outSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; biquadB[10] = biquadB[9]; 
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			biquadB[9] = sin(outSample); //DF1
			inputSample = outSample / compensation; inputSample = (inputSample * bWet) + (nukeLevel * (1.0-bWet));
			nukeLevel = inputSample;
		}
		if (cWet > 0.0) {
			outSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSample; biquadC[10] = biquadC[9]; 
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			biquadC[9] = sin(outSample); //DF1
			inputSample = outSample / compensation; inputSample = (inputSample * cWet) + (nukeLevel * (1.0-cWet));
			nukeLevel = inputSample;
		}
		if (dWet > 0.0) {
			outSample = biquadD[2]*inputSample+biquadD[3]*biquadD[7]+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSample; biquadD[10] = biquadD[9]; 
			outSample *= clipFactor;
			if (outSample > 1.57079633) outSample = 1.57079633;
			if (outSample < -1.57079633) outSample = -1.57079633;
			biquadD[9] = sin(outSample); //DF1
			inputSample = outSample / compensation; inputSample = (inputSample * dWet) + (nukeLevel * (1.0-dWet));
			nukeLevel = inputSample;
		}
		
		if (wet < 1.0) {
			inputSample = (drySample * (1.0-wet))+(inputSample * wet);
		}		
		
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
	for (int x = 0; x < 11; x++) {biquad[x] = 0.0; biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
