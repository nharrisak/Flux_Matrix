#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "RingModulatorMono"
#define AIRWINDOWS_DESCRIPTION "RingModulatorMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','i','o' )
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Soar", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		double sinePos;
		double incA;
		double incB;
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

	incA = incB; incB = pow(GetParameter( kParam_A ),5)/overallscale;
	double soar = 0.3-(GetParameter( kParam_B )*0.3);
	double wet = pow(GetParameter( kParam_C ),2);
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		double temp = (double)nSampleFrames/inFramesToProcess;
		double inc = (incA*temp)+(incB*(1.0-temp));

		sinePos += inc;
		if (sinePos > 6.283185307179586) sinePos -= 6.283185307179586;
		double sinResult = sin(sinePos);
		double out = 0.0;
		double snM = fabs(sinResult)+(soar*soar);
		double inM = fabs(inputSample);
		if (inM < snM) {
			inM = fabs(sinResult);
			snM = fabs(inputSample)+(soar*soar);
		}
		
		if (inputSample > 0.0 && sinResult > 0.0) out = fmax((sqrt((fabs(inM)/snM))*snM)-soar,0.0);
		if (inputSample < 0.0 && sinResult > 0.0) out = fmin((-sqrt((fabs(inM)/snM))*snM)+soar,0.0);
		if (inputSample > 0.0 && sinResult < 0.0) out = fmin((-sqrt((fabs(inM)/snM))*snM)+soar,0.0);
		if (inputSample < 0.0 && sinResult < 0.0) out = fmax((sqrt((fabs(inM)/snM))*snM)-soar,0.0);
		inputSample = out;

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
	sinePos = 0.0;
	incA = 0.0; incB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
