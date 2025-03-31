#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Coils"
#define AIRWINDOWS_DESCRIPTION "Models the types of distortion you'll find in transformers."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','i' )
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
{ .name = "Saturate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Core DC", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double figure[9];
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
	
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	Float64 boost = 1.0-pow(GetParameter( kParam_One ),2);
	if (boost < 0.001) boost = 0.001; //there's a divide, we can't have this be zero
	figure[0] = 600.0/GetSampleRate(); //fixed frequency, 600hz
	figure[1] = 0.023; //resonance
	Float64 offset = GetParameter( kParam_Two );
	Float64 sinOffset = sin(offset); //we can cache this, it's expensive
	Float64 wet = GetParameter( kParam_Three );
	
	
	double K = tan(M_PI * figure[0]);
	double norm = 1.0 / (1.0 + K / figure[1] + K * K);
	figure[2] = K / figure[1] * norm;
	figure[4] = -figure[2];
	figure[5] = 2.0 * (K * K - 1.0) * norm;
	figure[6] = (1.0 - K / figure[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		//double tempSample = (inputSample * figure[2]) + figure[7];
		//figure[7] = -(tempSample * figure[5]) + figure[8];
		//figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
		//inputSample = tempSample + sin(drySample-tempSample);
		//or
		//inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
		//
		//given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
		//the full frequencies but distorts like a real transformer. Purest case, and since
		//we are not using a high Q we can remove the extra sin/asin on the biquad.
		
		double tempSample = (inputSample * figure[2]) + figure[7];
		figure[7] = -(tempSample * figure[5]) + figure[8];
		figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
		inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
		//given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
		//the full frequencies but distorts like a real transformer. Since
		//we are not using a high Q we can remove the extra sin/asin on the biquad.
		
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
	for (int x = 0; x < 9; x++) {figure[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
