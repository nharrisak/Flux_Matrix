#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SurgeMono"
#define AIRWINDOWS_DESCRIPTION "SurgeMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','u','s' )
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
{ .name = "Surge", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		uint32_t fpd;
		double chaseA;
		double chaseB;
		double chaseC;
		double chaseD;
		double chaseMax;
	
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
	double inputSample;
	double drySample;
	Float64 chaseMax = 0.0;
	Float64 intensity = (1.0-(pow((1.0-GetParameter( kParam_One )),2)))*0.7;
	Float64 attack = ((intensity+0.1)*0.0005)/overallscale;
	Float64 decay = ((intensity+0.001)*0.00005)/overallscale;
	Float64 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	Float64 inputSense;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		inputSample *= 8.0;
		inputSample *= intensity;
		inputSense = fabs(inputSample);
		
		if (chaseMax < inputSense) chaseA += attack;
		if (chaseMax > inputSense) chaseA -= decay;
		
		if (chaseA > decay) chaseA = decay;
		if (chaseA < -attack) chaseA = -attack;
		
		chaseB += (chaseA/overallscale);
		if (chaseB > decay) chaseB = decay;
		if (chaseB < -attack) chaseB = -attack;
		
		chaseC += (chaseB/overallscale);
		if (chaseC > decay) chaseC = decay;
		if (chaseC < -attack) chaseC = -attack;

		chaseD += (chaseC/overallscale);
		if (chaseD > 1.0) chaseD = 1.0;
		if (chaseD < 0.0) chaseD = 0.0;
		
		chaseMax = chaseA;
		if (chaseMax < chaseB) chaseMax = chaseB;
		if (chaseMax < chaseC) chaseMax = chaseC;
		if (chaseMax < chaseD) chaseMax = chaseD;
		inputSample *= chaseMax;
				
		inputSample = drySample - (inputSample * intensity);
		inputSample = (drySample * (1.0-wet)) + (inputSample * wet);
		
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	chaseA = 0.0;
	chaseB = 0.0;
	chaseC = 0.0;
	chaseD = 0.0;
	chaseMax = 0.0;
}
};
