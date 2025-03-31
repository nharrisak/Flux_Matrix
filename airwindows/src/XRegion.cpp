#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "XRegion"
#define AIRWINDOWS_DESCRIPTION "Distorted staggered bandpasses, for extreme soundmangling."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','X','R','e' )
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
{ .name = "Gain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "First", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Last", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Nuke", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double biquad[11];
		double biquadA[11];
		double biquadB[11];
		double biquadC[11];
		double biquadD[11];
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
	
	Float64 gain = pow(GetParameter( kParam_One )+0.5,4);
	
	Float64 high = GetParameter( kParam_Two );
	Float64 low = GetParameter( kParam_Three );
	Float64 mid = (high+low)*0.5;
	Float64 spread = 1.001-fabs(high-low);
	
	biquad[0] = (pow(high,3)*20000.0)/GetSampleRate();
	if (biquad[0] < 0.00009) biquad[0] = 0.00009;
	Float64 compensation = sqrt(biquad[0])*6.4*spread;
	Float64 clipFactor = 0.75+(biquad[0]*GetParameter( kParam_Four )*37.0);
	
	biquadA[0] = (pow((high+mid)*0.5,3)*20000.0)/GetSampleRate();
	if (biquadA[0] < 0.00009) biquadA[0] = 0.00009;
	Float64 compensationA = sqrt(biquadA[0])*6.4*spread;
	Float64 clipFactorA = 0.75+(biquadA[0]*GetParameter( kParam_Four )*37.0);
	
	biquadB[0] = (pow(mid,3)*20000.0)/GetSampleRate();
	if (biquadB[0] < 0.00009) biquadB[0] = 0.00009;
	Float64 compensationB = sqrt(biquadB[0])*6.4*spread;
	Float64 clipFactorB = 0.75+(biquadB[0]*GetParameter( kParam_Four )*37.0);
	
	biquadC[0] = (pow((mid+low)*0.5,3)*20000.0)/GetSampleRate();
	if (biquadC[0] < 0.00009) biquadC[0] = 0.00009;
	Float64 compensationC = sqrt(biquadC[0])*6.4*spread;
	Float64 clipFactorC = 0.75+(biquadC[0]*GetParameter( kParam_Four )*37.0);
	
	biquadD[0] = (pow(low,3)*20000.0)/GetSampleRate();
	if (biquadD[0] < 0.00009) biquadD[0] = 0.00009;
	Float64 compensationD = sqrt(biquadD[0])*6.4*spread;
	Float64 clipFactorD = 0.75+(biquadD[0]*GetParameter( kParam_Four )*37.0);
	
	double K = tan(M_PI * biquad[0]);
	double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquad[2] = K / 0.7071 * norm;
	biquad[4] = -biquad[2];
	biquad[5] = 2.0 * (K * K - 1.0) * norm;
	biquad[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * biquadA[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadA[2] = K / 0.7071 * norm;
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * biquadB[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadB[2] = K / 0.7071 * norm;
	biquadB[4] = -biquadB[2];
	biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	biquadB[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * biquadC[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadC[2] = K / 0.7071 * norm;
	biquadC[4] = -biquadC[2];
	biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	biquadC[6] = (1.0 - K / 0.7071 + K * K) * norm;

	K = tan(M_PI * biquadD[0]);
	norm = 1.0 / (1.0 + K / 0.7071 + K * K);
	biquadD[2] = K / 0.7071 * norm;
	biquadD[4] = -biquadD[2];
	biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	biquadD[6] = (1.0 - K / 0.7071 + K * K) * norm;	
	
	Float64 aWet = 1.0;
	Float64 bWet = 1.0;
	Float64 cWet = 1.0;
	Float64 dWet = GetParameter( kParam_Four )*4.0;
	Float64 wet = GetParameter( kParam_Five );
	
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
		
		inputSample *= clipFactor;
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		outSample = biquad[2]*inputSample+biquad[4]*biquad[8]-biquad[5]*biquad[9]-biquad[6]*biquad[10];
		biquad[8] = biquad[7]; biquad[7] = inputSample; biquad[10] = biquad[9];
		biquad[9] = outSample; //DF1
		inputSample = outSample / compensation; nukeLevel = inputSample;
		
		if (aWet > 0.0) {
			inputSample *= clipFactorA;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = biquadA[2]*inputSample+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; biquadA[10] = biquadA[9];
			biquadA[9] = outSample; //DF1
			inputSample = outSample / compensationA; inputSample = (inputSample * aWet) + (nukeLevel * (1.0-aWet));
			nukeLevel = inputSample;
		}
		if (bWet > 0.0) {
			inputSample *= clipFactorB;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = biquadB[2]*inputSample+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; biquadB[10] = biquadB[9]; 
			biquadB[9] = outSample; //DF1
			inputSample = outSample / compensationB; inputSample = (inputSample * bWet) + (nukeLevel * (1.0-bWet));
			nukeLevel = inputSample;
		}
		if (cWet > 0.0) {
			inputSample *= clipFactorC;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = biquadC[2]*inputSample+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSample; biquadC[10] = biquadC[9]; 
			biquadC[9] = outSample; //DF1
			inputSample = outSample / compensationC; inputSample = (inputSample * cWet) + (nukeLevel * (1.0-cWet));
			nukeLevel = inputSample;
		}
		if (dWet > 0.0) {
			inputSample *= clipFactorD;
			if (inputSample > 1.57079633) inputSample = 1.57079633;
			if (inputSample < -1.57079633) inputSample = -1.57079633;
			inputSample = sin(inputSample);
			outSample = biquadD[2]*inputSample+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSample; biquadD[10] = biquadD[9]; 
			biquadD[9] = outSample; //DF1
			inputSample = outSample / compensationD; inputSample = (inputSample * dWet) + (nukeLevel * (1.0-dWet));
		}
		
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
				
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
