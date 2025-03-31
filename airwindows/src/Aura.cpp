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
	struct _dram* dram;
 
		double lastSample;
		Float64 previousVelocity;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 b[21];
		Float64 f[21];		
};
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
	if (gain > 1.0) {dram->f[0] = 1.0; gain -= 1.0;} else {dram->f[0] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[1] = 1.0; gain -= 1.0;} else {dram->f[1] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[2] = 1.0; gain -= 1.0;} else {dram->f[2] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[3] = 1.0; gain -= 1.0;} else {dram->f[3] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[4] = 1.0; gain -= 1.0;} else {dram->f[4] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[5] = 1.0; gain -= 1.0;} else {dram->f[5] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[6] = 1.0; gain -= 1.0;} else {dram->f[6] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[7] = 1.0; gain -= 1.0;} else {dram->f[7] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[8] = 1.0; gain -= 1.0;} else {dram->f[8] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[9] = 1.0; gain -= 1.0;} else {dram->f[9] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[10] = 1.0; gain -= 1.0;} else {dram->f[10] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[11] = 1.0; gain -= 1.0;} else {dram->f[11] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[12] = 1.0; gain -= 1.0;} else {dram->f[12] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[13] = 1.0; gain -= 1.0;} else {dram->f[13] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[14] = 1.0; gain -= 1.0;} else {dram->f[14] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[15] = 1.0; gain -= 1.0;} else {dram->f[15] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[16] = 1.0; gain -= 1.0;} else {dram->f[16] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[17] = 1.0; gain -= 1.0;} else {dram->f[17] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[18] = 1.0; gain -= 1.0;} else {dram->f[18] = gain; gain = 0.0;}
	if (gain > 1.0) {dram->f[19] = 1.0; gain -= 1.0;} else {dram->f[19] = gain; gain = 0.0;}
	
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0) overallscale = 1.0;
	dram->f[0] /= overallscale;
	dram->f[1] /= overallscale;
	dram->f[2] /= overallscale;
	dram->f[3] /= overallscale;
	dram->f[4] /= overallscale;
	dram->f[5] /= overallscale;
	dram->f[6] /= overallscale;
	dram->f[7] /= overallscale;
	dram->f[8] /= overallscale;
	dram->f[9] /= overallscale;
	dram->f[10] /= overallscale;
	dram->f[11] /= overallscale;
	dram->f[12] /= overallscale;
	dram->f[13] /= overallscale;
	dram->f[14] /= overallscale;
	dram->f[15] /= overallscale;
	dram->f[16] /= overallscale;
	dram->f[17] /= overallscale;
	dram->f[18] /= overallscale;
	dram->f[19] /= overallscale;
	//and now it's neatly scaled, too
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		velocity = lastSample - inputSample;
		correction = previousVelocity - velocity;
		
		dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15];
		dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11];
		dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9];
		dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5];
		dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1];
		dram->b[1] = dram->b[0]; dram->b[0] = accumulatorSample = correction;
		
		//we are accumulating rates of change of the rate of change
		
		accumulatorSample *= dram->f[0];
		accumulatorSample += (dram->b[1] * dram->f[1]);
		accumulatorSample += (dram->b[2] * dram->f[2]);
		accumulatorSample += (dram->b[3] * dram->f[3]);
		accumulatorSample += (dram->b[4] * dram->f[4]);
		accumulatorSample += (dram->b[5] * dram->f[5]);
		accumulatorSample += (dram->b[6] * dram->f[6]);
		accumulatorSample += (dram->b[7] * dram->f[7]);
		accumulatorSample += (dram->b[8] * dram->f[8]);
		accumulatorSample += (dram->b[9] * dram->f[9]);
		accumulatorSample += (dram->b[10] * dram->f[10]);
		accumulatorSample += (dram->b[11] * dram->f[11]);
		accumulatorSample += (dram->b[12] * dram->f[12]);
		accumulatorSample += (dram->b[13] * dram->f[13]);
		accumulatorSample += (dram->b[14] * dram->f[14]);
		accumulatorSample += (dram->b[15] * dram->f[15]);
		accumulatorSample += (dram->b[16] * dram->f[16]);
		accumulatorSample += (dram->b[17] * dram->f[17]);
		accumulatorSample += (dram->b[18] * dram->f[18]);
		accumulatorSample += (dram->b[19] * dram->f[19]);
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
	for(int count = 0; count < 21; count++) {dram->b[count] = 0.0; dram->f[count] = 0.0;}
	lastSample = 0.0;
	previousVelocity = 0.0;	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
