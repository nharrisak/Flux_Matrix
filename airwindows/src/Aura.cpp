#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Aura"
#define AIRWINDOWS_DESCRIPTION "A new kind of resonant lowpass EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','u','r' )
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
{ .name = "Voicing", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double lastSample;
		Float64 previousVelocity;
		Float64 b[21];
		Float64 f[21];		
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
	Float64 correction;
	Float64 accumulatorSample;
	Float64 velocity;
	Float64 trim = GetParameter( kParam_One );
	Float64 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	Float64 overallscale = trim * 10.0;
	Float64 gain = overallscale + (pow(wet,3) * 0.187859642462067);
	trim *= (1.0 - (pow(wet,3) * 0.187859642462067));
	double inputSample;
	Float64 drySample;
	
	if (gain < 1.0) gain = 1.0;
	if (gain > 1.0) {f[0] = 1.0; gain -= 1.0;} else {f[0] = gain; gain = 0.0;}
	if (gain > 1.0) {f[1] = 1.0; gain -= 1.0;} else {f[1] = gain; gain = 0.0;}
	if (gain > 1.0) {f[2] = 1.0; gain -= 1.0;} else {f[2] = gain; gain = 0.0;}
	if (gain > 1.0) {f[3] = 1.0; gain -= 1.0;} else {f[3] = gain; gain = 0.0;}
	if (gain > 1.0) {f[4] = 1.0; gain -= 1.0;} else {f[4] = gain; gain = 0.0;}
	if (gain > 1.0) {f[5] = 1.0; gain -= 1.0;} else {f[5] = gain; gain = 0.0;}
	if (gain > 1.0) {f[6] = 1.0; gain -= 1.0;} else {f[6] = gain; gain = 0.0;}
	if (gain > 1.0) {f[7] = 1.0; gain -= 1.0;} else {f[7] = gain; gain = 0.0;}
	if (gain > 1.0) {f[8] = 1.0; gain -= 1.0;} else {f[8] = gain; gain = 0.0;}
	if (gain > 1.0) {f[9] = 1.0; gain -= 1.0;} else {f[9] = gain; gain = 0.0;}
	if (gain > 1.0) {f[10] = 1.0; gain -= 1.0;} else {f[10] = gain; gain = 0.0;}
	if (gain > 1.0) {f[11] = 1.0; gain -= 1.0;} else {f[11] = gain; gain = 0.0;}
	if (gain > 1.0) {f[12] = 1.0; gain -= 1.0;} else {f[12] = gain; gain = 0.0;}
	if (gain > 1.0) {f[13] = 1.0; gain -= 1.0;} else {f[13] = gain; gain = 0.0;}
	if (gain > 1.0) {f[14] = 1.0; gain -= 1.0;} else {f[14] = gain; gain = 0.0;}
	if (gain > 1.0) {f[15] = 1.0; gain -= 1.0;} else {f[15] = gain; gain = 0.0;}
	if (gain > 1.0) {f[16] = 1.0; gain -= 1.0;} else {f[16] = gain; gain = 0.0;}
	if (gain > 1.0) {f[17] = 1.0; gain -= 1.0;} else {f[17] = gain; gain = 0.0;}
	if (gain > 1.0) {f[18] = 1.0; gain -= 1.0;} else {f[18] = gain; gain = 0.0;}
	if (gain > 1.0) {f[19] = 1.0; gain -= 1.0;} else {f[19] = gain; gain = 0.0;}
	
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0) overallscale = 1.0;
	f[0] /= overallscale;
	f[1] /= overallscale;
	f[2] /= overallscale;
	f[3] /= overallscale;
	f[4] /= overallscale;
	f[5] /= overallscale;
	f[6] /= overallscale;
	f[7] /= overallscale;
	f[8] /= overallscale;
	f[9] /= overallscale;
	f[10] /= overallscale;
	f[11] /= overallscale;
	f[12] /= overallscale;
	f[13] /= overallscale;
	f[14] /= overallscale;
	f[15] /= overallscale;
	f[16] /= overallscale;
	f[17] /= overallscale;
	f[18] /= overallscale;
	f[19] /= overallscale;
	//and now it's neatly scaled, too
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		velocity = lastSample - inputSample;
		correction = previousVelocity - velocity;
		
		b[19] = b[18]; b[18] = b[17]; b[17] = b[16]; b[16] = b[15];
		b[15] = b[14]; b[14] = b[13]; b[13] = b[12]; b[12] = b[11];
		b[11] = b[10]; b[10] = b[9];
		b[9] = b[8]; b[8] = b[7]; b[7] = b[6]; b[6] = b[5];
		b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1];
		b[1] = b[0]; b[0] = accumulatorSample = correction;
		
		//we are accumulating rates of change of the rate of change
		
		accumulatorSample *= f[0];
		accumulatorSample += (b[1] * f[1]);
		accumulatorSample += (b[2] * f[2]);
		accumulatorSample += (b[3] * f[3]);
		accumulatorSample += (b[4] * f[4]);
		accumulatorSample += (b[5] * f[5]);
		accumulatorSample += (b[6] * f[6]);
		accumulatorSample += (b[7] * f[7]);
		accumulatorSample += (b[8] * f[8]);
		accumulatorSample += (b[9] * f[9]);
		accumulatorSample += (b[10] * f[10]);
		accumulatorSample += (b[11] * f[11]);
		accumulatorSample += (b[12] * f[12]);
		accumulatorSample += (b[13] * f[13]);
		accumulatorSample += (b[14] * f[14]);
		accumulatorSample += (b[15] * f[15]);
		accumulatorSample += (b[16] * f[16]);
		accumulatorSample += (b[17] * f[17]);
		accumulatorSample += (b[18] * f[18]);
		accumulatorSample += (b[19] * f[19]);
		//we are doing our repetitive calculations on a separate value
		
		velocity = previousVelocity + accumulatorSample;		
		inputSample = lastSample + velocity;
		lastSample = inputSample;
		previousVelocity = -velocity * pow(trim,2);
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
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
	for(int count = 0; count < 21; count++) {b[count] = 0.0; f[count] = 0.0;}
	lastSample = 0.0;
	previousVelocity = 0.0;	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
