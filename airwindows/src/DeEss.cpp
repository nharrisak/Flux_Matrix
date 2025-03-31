#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeEss"
#define AIRWINDOWS_DESCRIPTION "The best de-essing solution there is. A go-to utility plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','E' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Intensity", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Max DeEss", .min = -4800, .max = 0, .def = -2400, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Frequency", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 s1;
		Float64 s2;
		Float64 s3;
		Float64 s4;
		Float64 s5;
		Float64 s6;
		Float64 s7;
		Float64 m1;
		Float64 m2;
		Float64 m3;
		Float64 m4;
		Float64 m5;
		Float64 m6;
		Float64 c1;
		Float64 c2;
		Float64 c3;
		Float64 c4;
		Float64 c5;
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 intensity = pow(GetParameter( kParam_One ),5)*(8192/overallscale);
	Float64 maxdess = 1.0 / pow(10.0,GetParameter( kParam_Two )/20);
	Float64 iirAmount = pow(GetParameter( kParam_Three ),2)/overallscale;
	Float64 offset;
	Float64 sense;
	Float64 recovery;
	Float64 attackspeed;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		s3 = s2;
		s2 = s1;
		s1 = inputSample;
		m1 = (s1-s2)*((s1-s2)/1.3);
		m2 = (s2-s3)*((s1-s2)/1.3);
		sense = fabs((m1-m2)*((m1-m2)/1.3));
		//this will be 0 for smooth, high for SSS
		attackspeed = 7.0+(sense*1024);
		//this does not vary with intensity, but it does react to onset transients
		
		sense = 1.0+(intensity*intensity*sense);
		if (sense > intensity) {sense = intensity;}
		//this will be 1 for smooth, 'intensity' for SSS
		recovery = 1.0+(0.01/sense);
		//this will be 1.1 for smooth, 1.0000000...1 for SSS
		
		offset = 1.0-fabs(inputSample);
		
		if (flip) {
			iirSampleA = (iirSampleA * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
			if (ratioA < sense)
			{ratioA = ((ratioA*attackspeed)+sense)/(attackspeed+1.0);}
			else
			{ratioA = 1.0+((ratioA-1.0)/recovery);}
			//returny to 1/1 code
			if (ratioA > maxdess){ratioA = maxdess;}
			inputSample = iirSampleA+((inputSample-iirSampleA)/ratioA);
		}
		else {
			iirSampleB = (iirSampleB * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));	
			if (ratioB < sense)
			{ratioB = ((ratioB*attackspeed)+sense)/(attackspeed+1.0);}
			else
			{ratioB = 1.0+((ratioB-1.0)/recovery);}
			//returny to 1/1 code
			if (ratioB > maxdess){ratioB = maxdess;}
			inputSample = iirSampleB+((inputSample-iirSampleB)/ratioB);
		} //have the ratio chase Sense
		
		flip = !flip;

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
	s1 = s2 = s3 = s4 = s5 = s6 = s7 = 0.0;
	m1 = m2 = m3 = m4 = m5 = m6 = 0.0;
	c1 = c2 = c3 = c4 = c5 = 0.0;
	ratioA = ratioB = 1.0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
