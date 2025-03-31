#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Lowpass2"
#define AIRWINDOWS_DESCRIPTION "An unusual-sounding variable-slope lowpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','o','x' )
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
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Soft/Hard", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 iirSampleF;
		Float64 iirSampleG;
		Float64 iirSampleH;
		uint32_t fpd;
		bool fpFlip;
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
	Float64 iirAmount = GetParameter( kParam_One );
	Float64 tight = GetParameter( kParam_Two );
	if (iirAmount < 0.0000001) iirAmount = 0.0000001;
	if (iirAmount > 1) iirAmount = 1;
	if (tight < 0.0) tight *= 0.5;
	Float64 offset;
	Float64 aWet = 1.0;
	Float64 bWet = 1.0;
	Float64 cWet = 1.0;
	Float64 dWet = GetParameter( kParam_Three );
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0) {aWet = dWet; bWet = 0.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 2.0) {bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 3.0) {cWet = dWet - 2.0; dWet = 0.0;}
	else {dWet -= 3.0;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	Float64 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0.0000001) offset = 0.0000001; if (offset > 1) offset = 1;

		if (fpFlip)
		{
			if (aWet > 0.0) {
				iirSampleA = (iirSampleA * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleA * aWet) + (inputSample * (1.0-aWet));
			}
			if (bWet > 0.0) {
				iirSampleC = (iirSampleC * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleC * bWet) + (inputSample * (1.0-bWet));
			}
			if (cWet > 0.0) {
				iirSampleE = (iirSampleE * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleE * cWet) + (inputSample * (1.0-cWet));
			}
			if (dWet > 0.0) {
				iirSampleG = (iirSampleG * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleG * dWet) + (inputSample * (1.0-dWet));
			}
		}
		else
		{
			if (aWet > 0.0) {
				iirSampleB = (iirSampleB * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleB * aWet) + (inputSample * (1.0-aWet));
			}
			if (bWet > 0.0) {
				iirSampleD = (iirSampleD * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleD * bWet) + (inputSample * (1.0-bWet));
			}
			if (cWet > 0.0) {
				iirSampleF = (iirSampleF * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleF * cWet) + (inputSample * (1.0-cWet));
			}
			if (dWet > 0.0) {
				iirSampleH = (iirSampleH * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
				inputSample = (iirSampleH * dWet) + (inputSample * (1.0-dWet));
			}
		}
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider
		fpFlip = !fpFlip;

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
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
