#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FathomFive"
#define AIRWINDOWS_DESCRIPTION "A way of supplementing extreme bass that’s not just EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','a','t' )
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
{ .name = "Root Note", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SubOctave", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Frequency", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		bool WasNegative;
		bool SubOctave;
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
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
	Float64 inputSample;
	Float64 rootnote = GetParameter( kParam_One );
	Float64 suboctave = GetParameter( kParam_Two );
	Float64 EQ = 0.01+((pow(GetParameter( kParam_Three ),4) / GetSampleRate())*32000.0);
	Float64 dcblock = EQ / 320.0;
	Float64 wet = GetParameter( kParam_Four )*2.0;
	Float64 dry = 2.0 - wet;
	Float64 bridgerectifier;
	Float64 result;
	Float64 basstrim = (0.01/EQ)+1.0;
	if (wet > 1.0) wet = 1.0;
	if (dry > 1.0) dry = 1.0;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		
		if (inputSample > 0)
			{if (WasNegative){SubOctave = !SubOctave;} WasNegative = false;}
		else {WasNegative = true;}
		
		iirSampleD = (iirSampleD * (1 - EQ)) + (inputSample *  EQ);
		bridgerectifier = fabs(iirSampleD);
		if (SubOctave) result = bridgerectifier*suboctave;
		else result = -bridgerectifier*suboctave;
		
		result += (inputSample*rootnote);
		
		iirSampleA += (result *  EQ);
		iirSampleA -= (iirSampleA * iirSampleA * iirSampleA * EQ);
		//this is the heart of FathomFive.
		
		
		if (iirSampleA > 0) iirSampleA -= dcblock;
		else iirSampleA += dcblock;
		result = iirSampleA*basstrim;
		
		iirSampleB = (iirSampleB * (1 - EQ)) + (result *  EQ);
		result = iirSampleB;
		
		iirSampleC = (iirSampleC * (1 - EQ)) + (result *  EQ);
		result = iirSampleC;
		
        Float64 outputSample = (inputSample*dry) + (result*wet);
		//this plugin produces DC offset very easily: it's presented in this primitive form
		//to make it easier to understand for when these are open sourced.
		//FathomFive is the original Airwindows deep bass booster algorithm:
		//further ones work to rein in that DC offset issue.
				
		//begin 32 bit floating point dither
		int expon; frexpf((float)outputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		outputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		sourceP += inNumChannels;
		*destP = outputSample;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	WasNegative = false;
	SubOctave = false;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
