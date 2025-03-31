#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Gatelope"
#define AIRWINDOWS_DESCRIPTION "A special gate that applies filters."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','u' )
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
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble Sustain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Sustain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack Speed", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		Float64 iirLowpassA;
		Float64 iirLowpassB;
		Float64 iirHighpassA;
		Float64 iirHighpassB;
		Float64 treblefreq;
		Float64 bassfreq;
		bool flip;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
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
	//speed settings around release
	Float64 threshold = pow(GetParameter( kParam_One ),2);
	//gain settings around threshold
	Float64 trebledecay = pow(1.0-GetParameter( kParam_Two ),2)/4196.0;
	Float64 bassdecay =  pow(1.0-GetParameter( kParam_Three ),2)/8192.0;
	Float64 slowAttack = (pow(GetParameter( kParam_Four ),3)*3)+0.003;
	Float64 wet = GetParameter( kParam_Five );
	slowAttack /= overallscale;
	trebledecay /= overallscale;
	bassdecay /= overallscale;
	trebledecay += 1.0;
	bassdecay += 1.0;
	Float64 attackSpeed;

	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		attackSpeed = slowAttack - (fabs(inputSample)*slowAttack*0.5);
		
		if (attackSpeed < 0.0) attackSpeed = 0.0;
		//softening onset click depending on how hard we're getting it
		
		if (flip)
		{
			if (fabs(inputSample) > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0) treblefreq = 2.0;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0) bassfreq = 0.0;
				iirLowpassA = inputSample;
				iirLowpassB = inputSample;
				iirHighpassA = 0.0;
				iirHighpassB = 0.0;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0) iirLowpassA = inputSample;
			else iirLowpassA = (iirLowpassA * (1.0 - treblefreq)) + (inputSample * treblefreq);
			
			if (bassfreq > 1.0) bassfreq = 1.0;
			
			if (bassfreq > 0.0) iirHighpassA = (iirHighpassA * (1.0 - bassfreq)) + (inputSample * bassfreq);
			else iirHighpassA = 0.0;
			
			if (treblefreq > bassfreq) inputSample = (iirLowpassA - iirHighpassA);
			else inputSample = 0.0;
		}
		else
		{
			if (fabs(inputSample) > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0) treblefreq = 2.0;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0) bassfreq = 0.0;
				iirLowpassA = inputSample;
				iirLowpassB = inputSample;
				iirHighpassA = 0.0;
				iirHighpassB = 0.0;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0) iirLowpassB = inputSample;
			else iirLowpassB = (iirLowpassB * (1.0 - treblefreq)) + (inputSample * treblefreq);
			
			if (bassfreq > 1.0) bassfreq = 1.0;

			if (bassfreq > 0.0) iirHighpassB = (iirHighpassB * (1.0 - bassfreq)) + (inputSample * bassfreq);
			else iirHighpassB = 0.0;
			
			if (treblefreq > bassfreq) inputSample = (iirLowpassB - iirHighpassB);
			else inputSample = 0.0;
		}
		//done full gated envelope filtered effect
		inputSample  = ((1-wet)*drySample)+(wet*inputSample);
		//we're going to set up a dry/wet control instead of a min. threshold
		
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
	iirLowpassA = 0.0;
	iirLowpassB = 0.0;
	iirHighpassA = 0.0;
	iirHighpassB = 0.0;
	treblefreq = 1.0;
	bassfreq = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
