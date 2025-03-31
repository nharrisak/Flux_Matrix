#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dirt"
#define AIRWINDOWS_DESCRIPTION "A soft-clip distortion in the spirit of Edge."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','r' )
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
{ .name = "Gain", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		double fixC[fix_total];
		double fixD[fix_total];
		double fixE[fix_total];
		double fixF[fix_total];
		double fixG[fix_total];
		double iirSample;
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
	
	double inTrim = ((1.0-pow(1.0-GetParameter( kParam_One ),2))*1.0)+1.0;
	double cutoff = (GetParameter( kParam_Two )*25000.0) / GetSampleRate();
	if (cutoff > 0.49) cutoff = 0.49; //don't crash if run at 44.1k
	if (cutoff < 0.0001) cutoff = 0.0001; //or if cutoff's too low
	double iirAmount = pow(GetParameter( kParam_Three ),3)*0.5;
	if (iirAmount < 0.00000001) iirAmount = 0.00000001; //or if cutoff's too low
	iirAmount /= overallscale; //highpass is very gentle
	double outPad = GetParameter( kParam_Four );
	double wet = GetParameter( kParam_Five );
	
	fixG[fix_freq] = fixF[fix_freq] = fixE[fix_freq] = fixD[fix_freq] = fixC[fix_freq] = fixB[fix_freq] = fixA[fix_freq] = cutoff;
	
    fixA[fix_reso] = 4.46570214;
	fixB[fix_reso] = 1.51387132;
	fixC[fix_reso] = 0.93979296;
	fixD[fix_reso] = 0.70710678;
	fixE[fix_reso] = 0.59051105;
	fixF[fix_reso] = 0.52972649;
	fixG[fix_reso] = 0.50316379;
	
	double K = tan(M_PI * fixA[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
	fixA[fix_a0] = K * K * norm;
	fixA[fix_a1] = 2.0 * fixA[fix_a0];
	fixA[fix_a2] = fixA[fix_a0];
	fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
	fixB[fix_a0] = K * K * norm;
	fixB[fix_a1] = 2.0 * fixB[fix_a0];
	fixB[fix_a2] = fixB[fix_a0];
	fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / fixC[fix_reso] + K * K);
	fixC[fix_a0] = K * K * norm;
	fixC[fix_a1] = 2.0 * fixC[fix_a0];
	fixC[fix_a2] = fixC[fix_a0];
	fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixC[fix_b2] = (1.0 - K / fixC[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / fixD[fix_reso] + K * K);
	fixD[fix_a0] = K * K * norm;
	fixD[fix_a1] = 2.0 * fixD[fix_a0];
	fixD[fix_a2] = fixD[fix_a0];
	fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixD[fix_b2] = (1.0 - K / fixD[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / fixE[fix_reso] + K * K);
	fixE[fix_a0] = K * K * norm;
	fixE[fix_a1] = 2.0 * fixE[fix_a0];
	fixE[fix_a2] = fixE[fix_a0];
	fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixE[fix_b2] = (1.0 - K / fixE[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / fixF[fix_reso] + K * K);
	fixF[fix_a0] = K * K * norm;
	fixF[fix_a1] = 2.0 * fixF[fix_a0];
	fixF[fix_a2] = fixF[fix_a0];
	fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixF[fix_b2] = (1.0 - K / fixF[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixG[fix_freq]);
	norm = 1.0 / (1.0 + K / fixG[fix_reso] + K * K);
	fixG[fix_a0] = K * K * norm;
	fixG[fix_a1] = 2.0 * fixG[fix_a0];
	fixG[fix_a2] = fixG[fix_a0];
	fixG[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixG[fix_b2] = (1.0 - K / fixG[fix_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		double drySample = inputSample;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (fabs(iirSample)<1.18e-33) iirSample = 0.0;
		iirSample = (iirSample * (1.0 - iirAmount)) + (inputSample * iirAmount);
		inputSample = inputSample - iirSample;
		
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;

		double outSample = (inputSample * fixA[fix_a0]) + fixA[fix_sL1];
		fixA[fix_sL1] = (inputSample * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
		fixA[fix_sL2] = (inputSample * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*inTrim;
		
		outSample = (inputSample * fixB[fix_a0]) + fixB[fix_sL1];
		fixB[fix_sL1] = (inputSample * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
		fixB[fix_sL2] = (inputSample * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*inTrim;
		
		outSample = (inputSample * fixC[fix_a0]) + fixC[fix_sL1];
		fixC[fix_sL1] = (inputSample * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sL2];
		fixC[fix_sL2] = (inputSample * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*inTrim;
		
		outSample = (inputSample * fixD[fix_a0]) + fixD[fix_sL1];
		fixD[fix_sL1] = (inputSample * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sL2];
		fixD[fix_sL2] = (inputSample * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*inTrim;
		
		outSample = (inputSample * fixE[fix_a0]) + fixE[fix_sL1];
		fixE[fix_sL1] = (inputSample * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sL2];
		fixE[fix_sL2] = (inputSample * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*inTrim;
		
		outSample = (inputSample * fixF[fix_a0]) + fixF[fix_sL1];
		fixF[fix_sL1] = (inputSample * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sL2];
		fixF[fix_sL2] = (inputSample * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*inTrim;
		
		outSample = (inputSample * fixG[fix_a0]) + fixG[fix_sL1];
		fixG[fix_sL1] = (inputSample * fixG[fix_a1]) - (outSample * fixG[fix_b1]) + fixG[fix_sL2];
		fixG[fix_sL2] = (inputSample * fixG[fix_a2]) - (outSample * fixG[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		inputSample = (inputSample * wet * outPad) + (drySample * (1.0-wet));
				
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
	for (int x = 0; x < fix_total; x++) {
		fixA[x] = 0.0;
		fixB[x] = 0.0;
		fixC[x] = 0.0;
		fixD[x] = 0.0;
		fixE[x] = 0.0;
		fixF[x] = 0.0;
		fixG[x] = 0.0;
	}
	iirSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
