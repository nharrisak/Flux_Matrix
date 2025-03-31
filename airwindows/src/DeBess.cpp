#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeBess"
#define AIRWINDOWS_DESCRIPTION "An improved DeEss, with perfect rejection of non-ess audio."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','B' )
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
{ .name = "Intensity", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sharpness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sense Mon", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		double s[41], m[41], c[41];		
		Float64 ratioA;
		Float64 ratioB;
		Float64 iirSampleA;
		Float64 iirSampleB;
		bool flip;
		
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();

	Float64 intensity = pow(GetParameter( kParam_One ),5)*(8192/overallscale);
	Float64 sharpness = GetParameter( kParam_Two )*40.0;
	if (sharpness < 2) sharpness = 2;
	Float64 speed = 0.1 / sharpness;
	Float64 depth = 1.0 / (GetParameter( kParam_Three )+0.0001);
	Float64 iirAmount = GetParameter( kParam_Four );
	int monitoring = GetParameter( kParam_Five );
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-37) inputSample = fpd * 1.18e-37;		
		
		s[0] = inputSample; //set up so both [0] and [1] will be input sample
		//we only use the [1] so this is just where samples come in
		for (int x = sharpness; x > 0; x--) {
			s[x] = s[x-1];
		} //building up a set of slews
		
		m[1] = (s[1]-s[2])*((s[1]-s[2])/1.3);
		for (int x = sharpness-1; x > 1; x--) {
			m[x] = (s[x]-s[x+1])*((s[x-1]-s[x])/1.3);
		} //building up a set of slews of slews
		
		
		double sense = fabs(m[1] - m[2]) * sharpness * sharpness;
		for (int x = sharpness-1; x > 0; x--) {
			double mult = fabs(m[x] - m[x+1]) * sharpness * sharpness;
			if (mult < 1.0) sense *= mult;
		} //sense is slews of slews times each other
				
		sense = 1.0+(intensity*intensity*sense);
		if (sense > intensity) {sense = intensity;}
				
		if (flip) {
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			ratioA = (ratioA * (1.0-speed))+(sense * speed);
			if (ratioA > depth) ratioA = depth;
			if (ratioA > 1.0) inputSample = iirSampleA+((inputSample-iirSampleA)/ratioA);
		}
		else {
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);	
			ratioB = (ratioB * (1.0-speed))+(sense * speed);			
			if (ratioB > depth) ratioB = depth;
			if (ratioA > 1.0) inputSample = iirSampleB+((inputSample-iirSampleB)/ratioB);
		}
		flip = !flip;
		
		if (monitoring == 1) inputSample = *sourceP - inputSample;
		//sense monitoring
		
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
	for (int x = 0; x < 41; x++) {s[x] = 0.0; m[x] = 0.0; c[x] = 0.0;}
	ratioA = ratioB = 1.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
