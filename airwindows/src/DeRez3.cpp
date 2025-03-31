#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeRez3"
#define AIRWINDOWS_DESCRIPTION "Reinvents retro digital tones."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','T' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Rate", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		double rezA;
		double rezB;
		double bitA;
		double bitB;
		double wetA;
		double wetB;
		
		enum {
			bez_AL,
			bez_BL,
			bez_CL,	
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		double bez[bez_total];
		
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

	rezA = rezB;
	rezB = pow(GetParameter( kParam_A ),3.0)/overallscale;
	bitA = bitB;
	bitB = (GetParameter( kParam_B)*15.0)+1.0;
	wetA = wetB;
	wetB = GetParameter( kParam_C )*2.0;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpd * 1.18e-17;
		double drySampleL = inputSampleL;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double rez = (rezA*temp)+(rezB*(1.0-temp));
		double bit = (bitA*temp)+(bitB*(1.0-temp));
		double wet = (wetA*temp)+(wetB*(1.0-temp));
		if (rez < 0.0005) rez = 0.0005;
		double bitFactor = pow(2.0,bit);
		double dry = 2.0 - wet;
		if (wet > 1.0) wet = 1.0;
		if (wet < 0.0) wet = 0.0;
		if (dry > 1.0) dry = 1.0;
		if (dry < 0.0) dry = 0.0;
		//this bitcrush makes 50% full dry AND full wet, not crossfaded.
		//that's so it can be on tracks without cutting back dry channel when adjusted
		
		inputSampleL *= bitFactor;		
		inputSampleL = floor(inputSampleL+(0.5/bitFactor));
		inputSampleL /= bitFactor;
		
		bez[bez_cycle] += rez;
		bez[bez_SampL] += (inputSampleL * rez);
		if (bez[bez_cycle] > 1.0) {
			bez[bez_cycle] -= 1.0;
			bez[bez_CL] = bez[bez_BL];
			bez[bez_BL] = bez[bez_AL];
			bez[bez_AL] = inputSampleL;
			bez[bez_SampL] = 0.0;
		}
		double CBL = (bez[bez_CL]*(1.0-bez[bez_cycle]))+(bez[bez_BL]*bez[bez_cycle]);
		double BAL = (bez[bez_BL]*(1.0-bez[bez_cycle]))+(bez[bez_AL]*bez[bez_cycle]);
		double CBAL = (bez[bez_BL]+(CBL*(1.0-bez[bez_cycle]))+(BAL*bez[bez_cycle]))*0.5;
		
		inputSampleL = (wet*CBAL)+(dry*drySampleL);
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSampleL += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	rezA = 1.0; rezB = 1.0;
	bitA = 1.0; bitB = 1.0;
	wetA = 1.0; wetB = 1.0;
	for (int x = 0; x < bez_total; x++) bez[x] = 0.0;
	bez[bez_cycle] = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
