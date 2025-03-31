#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Density2"
#define AIRWINDOWS_DESCRIPTION "A different color for Density, some old code I had that people wanted."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','o' )
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
{ .name = "Density", .min = -1000, .max = 4000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double last3Sample;
		double last2Sample;
		double last1Sample;
		double ataA;
		double ataB;
		double ataC;
		double lastDiffSample;
		double iirSampleA;
		double iirSampleB;
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
	Float64 density = GetParameter( kParam_One );
	Float64 out = fabs(density);
	while (out > 1.0) out = out - 1.0;
	density = density * fabs(density);
	Float64 iirAmount = pow(GetParameter( kParam_Two ),3)/overallscale;
	Float64 output = GetParameter( kParam_Three );
	Float64 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		double halfwaySample = (inputSample + last1Sample + ((-last2Sample + last3Sample) * 0.0414213562373095048801688)) / 2.0;
		double halfDrySample = halfwaySample;
		
		last3Sample = last2Sample; last2Sample = last1Sample; last1Sample = inputSample;

		iirSampleB = (iirSampleB * (1.0 - iirAmount)) + (halfwaySample * iirAmount); halfwaySample -= iirSampleB; //highpass section
		
		double count = density;
		double bridgerectifier;
		while (count > 1.0) {
			bridgerectifier = fabs(halfwaySample)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (halfwaySample > 0.0) halfwaySample = bridgerectifier;
			else halfwaySample = -bridgerectifier;
			count = count - 1.0;
		}
		bridgerectifier = fabs(halfwaySample)*1.57079633;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		if (density > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier); //produce either boosted or starved version
		if (halfwaySample > 0) halfwaySample = (halfwaySample*(1.0-out))+(bridgerectifier*out);
		else halfwaySample = (halfwaySample*(1.0-out))-(bridgerectifier*out); //blend according to density control
		
		ataC = halfwaySample - halfDrySample;
		ataA *= 0.915965594177219015; ataB *= 0.915965594177219015;
		ataB += ataC; ataA -= ataC; ataC = ataB;
		double halfDiffSample = ataC * 0.915965594177219015;
		
		iirSampleA = (iirSampleA * (1.0 - iirAmount)) + (inputSample * iirAmount); inputSample -= iirSampleA; //highpass section
		
		count = density;
		while (count > 1.0) {
			bridgerectifier = fabs(inputSample)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			bridgerectifier = sin(bridgerectifier);
			if (inputSample > 0.0) inputSample = bridgerectifier;
			else inputSample = -bridgerectifier;
			count = count - 1.0;
		}
		bridgerectifier = fabs(inputSample)*1.57079633;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		if (density > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier); //produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out); //blend according to density control
		
		ataC = inputSample - drySample;
		ataA *= 0.915965594177219015; ataB *= 0.915965594177219015;
		ataA += ataC; ataB -= ataC; ataC = ataA;
		double diffSample = ataC * 0.915965594177219015; 
		
		inputSample = drySample + ((diffSample + halfDiffSample + lastDiffSample) / 1.187);
		lastDiffSample = diffSample / 2.0;
		
		inputSample *= output;
		
		inputSample = (drySample*(1.0-wet))+(inputSample*wet);
				
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
	ataA = ataB = ataC = lastDiffSample = 0.0;
	iirSampleA = iirSampleB = last3Sample = last2Sample = last1Sample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
