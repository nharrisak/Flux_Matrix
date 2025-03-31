#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadHiLo"
#define AIRWINDOWS_DESCRIPTION "The highpass and lowpass filter in ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_HIP =0,
	kParam_LOP =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Highpas", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		enum {
			hilp_freq, hilp_temp,
			hilp_a0, hilp_a1, hilp_b1, hilp_b2,
			hilp_c0, hilp_c1, hilp_d1, hilp_d2,
			hilp_e0, hilp_e1, hilp_f1, hilp_f2,
			hilp_aL1, hilp_aL2, hilp_aR1, hilp_aR2,
			hilp_cL1, hilp_cL2, hilp_cR1, hilp_cR2,
			hilp_eL1, hilp_eL2, hilp_eR1, hilp_eR2,
			hilp_total
		};
		double highpass[hilp_total];
		double lowpass[hilp_total];
		
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
	
	highpass[hilp_freq] = ((GetParameter( kParam_HIP )*330.0)+20.0)/GetSampleRate();
	bool highpassEngage = true; if (GetParameter( kParam_HIP ) == 0.0) highpassEngage = false;
	
	lowpass[hilp_freq] = ((pow(1.0-GetParameter( kParam_LOP ),2)*17000.0)+3000.0)/GetSampleRate();
	bool lowpassEngage = true; if (GetParameter( kParam_LOP ) == 0.0) lowpassEngage = false;
	
	double K = tan(M_PI * highpass[hilp_freq]); //highpass
	double norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
	highpass[hilp_a0] = norm;
	highpass[hilp_a1] = -2.0 * highpass[hilp_a0];
	highpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
	highpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
	highpass[hilp_c0] = norm;
	highpass[hilp_c1] = -2.0 * highpass[hilp_c0];
	highpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
	highpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
	highpass[hilp_e0] = norm;
	highpass[hilp_e1] = -2.0 * highpass[hilp_e0];
	highpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
	highpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
		
	K = tan(M_PI * lowpass[hilp_freq]); //lowpass
	norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
	lowpass[hilp_a0] = K * K * norm;
	lowpass[hilp_a1] = 2.0 * lowpass[hilp_a0];
	lowpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
	lowpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
	lowpass[hilp_c0] = K * K * norm;
	lowpass[hilp_c1] = 2.0 * lowpass[hilp_c0];
	lowpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
	lowpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
	lowpass[hilp_e0] = K * K * norm;
	lowpass[hilp_e1] = 2.0 * lowpass[hilp_e0];
	lowpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
	lowpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (highpassEngage) { //begin Stacked Highpass
			highpass[hilp_temp] = (inputSample*highpass[hilp_a0])+highpass[hilp_aL1];
			highpass[hilp_aL1] = (inputSample*highpass[hilp_a1])-(highpass[hilp_temp]*highpass[hilp_b1])+highpass[hilp_aL2];
			highpass[hilp_aL2] = (inputSample*highpass[hilp_a0])-(highpass[hilp_temp]*highpass[hilp_b2]); inputSample = highpass[hilp_temp];
			highpass[hilp_temp] = (inputSample*highpass[hilp_c0])+highpass[hilp_cL1];
			highpass[hilp_cL1] = (inputSample*highpass[hilp_c1])-(highpass[hilp_temp]*highpass[hilp_d1])+highpass[hilp_cL2];
			highpass[hilp_cL2] = (inputSample*highpass[hilp_c0])-(highpass[hilp_temp]*highpass[hilp_d2]); inputSample = highpass[hilp_temp];
			highpass[hilp_temp] = (inputSample*highpass[hilp_e0])+highpass[hilp_eL1];
			highpass[hilp_eL1] = (inputSample*highpass[hilp_e1])-(highpass[hilp_temp]*highpass[hilp_f1])+highpass[hilp_eL2];
			highpass[hilp_eL2] = (inputSample*highpass[hilp_e0])-(highpass[hilp_temp]*highpass[hilp_f2]); inputSample = highpass[hilp_temp];
		} else {
			highpass[hilp_aL1] = highpass[hilp_aL2] = 0.0;
		} //end Stacked Highpass
		
		//rest of control strip goes here
		
		if (lowpassEngage) { //begin Stacked Lowpass
			lowpass[hilp_temp] = (inputSample*lowpass[hilp_a0])+lowpass[hilp_aL1];
			lowpass[hilp_aL1] = (inputSample*lowpass[hilp_a1])-(lowpass[hilp_temp]*lowpass[hilp_b1])+lowpass[hilp_aL2];
			lowpass[hilp_aL2] = (inputSample*lowpass[hilp_a0])-(lowpass[hilp_temp]*lowpass[hilp_b2]); inputSample = lowpass[hilp_temp];
			lowpass[hilp_temp] = (inputSample*lowpass[hilp_c0])+lowpass[hilp_cL1];
			lowpass[hilp_cL1] = (inputSample*lowpass[hilp_c1])-(lowpass[hilp_temp]*lowpass[hilp_d1])+lowpass[hilp_cL2];
			lowpass[hilp_cL2] = (inputSample*lowpass[hilp_c0])-(lowpass[hilp_temp]*lowpass[hilp_d2]); inputSample = lowpass[hilp_temp];
			lowpass[hilp_temp] = (inputSample*lowpass[hilp_e0])+lowpass[hilp_eL1];
			lowpass[hilp_eL1] = (inputSample*lowpass[hilp_e1])-(lowpass[hilp_temp]*lowpass[hilp_f1])+lowpass[hilp_eL2];
			lowpass[hilp_eL2] = (inputSample*lowpass[hilp_e0])-(lowpass[hilp_temp]*lowpass[hilp_f2]); inputSample = lowpass[hilp_temp];
		} //end Stacked Lowpass
		
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
	for (int x = 0; x < hilp_total; x++) {
		highpass[x] = 0.0;
		lowpass[x] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
