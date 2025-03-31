#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "YNotLowpass"
#define AIRWINDOWS_DESCRIPTION "Soft and smooth to nasty, edgy texture-varying filtering, no control smoothing."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Y','N','q' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Gain", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reson8", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "ResEdge", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		enum {
			biq_freq,
			biq_reso,
			biq_a0,
			biq_a1,
			biq_a2,
			biq_b1,
			biq_b2,
			biq_sL1,
			biq_sL2,
			biq_sR1,
			biq_sR2,
			biq_total
		}; //coefficient interpolating biquad filter, stereo
		double biquad[biq_total];
		//double biquadC[biq_total];
		//double biquadD[biq_total];
		
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
		double fixB[fix_total];
		
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
	
	double inTrim = GetParameter( kParam_One )*10.0;
	
	biquad[biq_freq] = pow(GetParameter( kParam_Two ),3)*20000.0;
	if (biquad[biq_freq] < 15.0) biquad[biq_freq] = 15.0;
	biquad[biq_freq] /= GetSampleRate();
    biquad[biq_reso] = (pow(GetParameter( kParam_Three ),2)*15.0)+0.5571;
	double K = tan(M_PI * biquad[biq_freq]);
	double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
	biquad[biq_a0] = K * K * norm;
	biquad[biq_a1] = 2.0 * biquad[biq_a0];
	biquad[biq_a2] = biquad[biq_a0];
	biquad[biq_b1] = 2.0 * (K * K - 1.0) * norm;
	biquad[biq_b2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
	//for the coefficient-interpolated biquad filter
	
	double powFactor = pow(GetParameter( kParam_Four )+0.9,4);
	
	//1.0 == target neutral
	
	double outTrim = GetParameter( kParam_Five );
	
	double wet = GetParameter( kParam_Six );
	
	fixA[fix_freq] = fixB[fix_freq] = 20000.0 / GetSampleRate();
    fixA[fix_reso] = fixB[fix_reso] = 0.7071; //butterworth Q
	
	K = tan(M_PI * fixA[fix_freq]);
	norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
	fixA[fix_a0] = fixB[fix_a0] = K * K * norm;
	fixA[fix_a1] = fixB[fix_a1] = 2.0 * fixA[fix_a0];
	fixA[fix_a2] = fixB[fix_a2] = fixA[fix_a0];
	fixA[fix_b1] = fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = fixB[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
	//for the fixed-position biquad filter
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = *sourceP;
		
		inputSample *= inTrim;
		
		double temp = (inputSample * fixA[fix_a0]) + fixA[fix_sL1];
		fixA[fix_sL1] = (inputSample * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sL2];
		fixA[fix_sL2] = (inputSample * fixA[fix_a2]) - (temp * fixA[fix_b2]);
		inputSample = temp; //fixed biquad filtering ultrasonics
		
		//encode/decode courtesy of torridgristle under the MIT license
		if (inputSample > 1.0) inputSample = 1.0;
		else if (inputSample > 0.0) inputSample = 1.0 - pow(1.0-inputSample,powFactor);
		if (inputSample < -1.0) inputSample = -1.0;
		else if (inputSample < 0.0) inputSample = -1.0 + pow(1.0+inputSample,powFactor);
		
		temp = (inputSample * biquad[biq_a0]) + biquad[biq_sL1];
		biquad[biq_sL1] = (inputSample * biquad[biq_a1]) - (temp * biquad[biq_b1]) + biquad[biq_sL2];
		biquad[biq_sL2] = (inputSample * biquad[biq_a2]) - (temp * biquad[biq_b2]);
		inputSample = temp; //coefficient interpolating biquad filter
		
		//encode/decode courtesy of torridgristle under the MIT license
		if (inputSample > 1.0) inputSample = 1.0;
		else if (inputSample > 0.0) inputSample = 1.0 - pow(1.0-inputSample,(1.0/powFactor));
		if (inputSample < -1.0) inputSample = -1.0;
		else if (inputSample < 0.0) inputSample = -1.0 + pow(1.0+inputSample,(1.0/powFactor));
		
		inputSample *= outTrim;
		
		temp = (inputSample * fixB[fix_a0]) + fixB[fix_sL1];
		fixB[fix_sL1] = (inputSample * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sL2];
		fixB[fix_sL2] = (inputSample * fixB[fix_a2]) - (temp * fixB[fix_b2]);
		inputSample = temp; //fixed biquad filtering ultrasonics
		
		if (wet < 1.0) {
			inputSample = (inputSample*wet) + (drySample*(1.0-wet));
		}
		
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
	for (int x = 0; x < biq_total; x++) {biquad[x] = 0.0;}
	for (int x = 0; x < fix_total; x++) {fixA[x] = 0.0; fixB[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
