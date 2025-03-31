#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "HypersonX"
#define AIRWINDOWS_DESCRIPTION "Like UltrasonX but with seven stages instead of five."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','y','r' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kA = 0;
static const int kB = 1;
static const int kC = 2;
static const int kD = 3;
static const int kE = 4;
static const int kF = 5;
static const int kG = 6;
static const int kDefaultValue_ParamOne = kD;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static char const * const enumStrings0[] = { "Reso A", "Reso B", "Reso C", "Reso D", "Reso E", "Reso F", "Reso G", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Q", .min = 0, .max = 6, .def = 3, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		double fixA[fix_total];
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
	
	double cutoff = 25000.0 / GetSampleRate();
	if (cutoff > 0.49) cutoff = 0.49; //don't crash if run at 44.1k
	
	fixA[fix_freq] = cutoff;
	
	switch ((int) GetParameter( kParam_One ))
	{
		case kA:
			fixA[fix_reso] = 4.46570214;
			break;
		case kB:
			fixA[fix_reso] = 1.51387132;;
			break;
		case kC:
			fixA[fix_reso] = 0.93979296;
			break;
		case kD:
			fixA[fix_reso] = 0.70710678; //butterworth Q
			break;
		case kE:
			fixA[fix_reso] = 0.59051105;
			break;
		case kF:
			fixA[fix_reso] = 0.52972649;
			break;
		case kG:
			fixA[fix_reso] = 0.50316379;
			break;
	}	
	
	double K = tan(M_PI * fixA[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
	fixA[fix_a0] = K * K * norm;
	fixA[fix_a1] = 2.0 * fixA[fix_a0];
	fixA[fix_a2] = fixA[fix_a0];
	fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		double outSample = (inputSample * fixA[fix_a0]) + fixA[fix_sL1];
		fixA[fix_sL1] = (inputSample * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
		fixA[fix_sL2] = (inputSample * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
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
	for (int x = 0; x < fix_total; x++) fixA[x] = 0.0;
	fixA[fix_reso] = 0.7071; //butterworth Q
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
