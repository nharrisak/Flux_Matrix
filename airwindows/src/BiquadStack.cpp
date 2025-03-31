#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadStack"
#define AIRWINDOWS_DESCRIPTION "A way of making a parametric EQ out of stacked biquad filters."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','x' )
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
{ .name = "Level", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;

		
		enum {
			biqs_freq, biqs_reso, biqs_level, biqs_levelA, biqs_levelB, biqs_nonlin, biqs_temp, biqs_dis,
			biqs_a0, biqs_aA0, biqs_aB0, biqs_b1, biqs_bA1, biqs_bB1, biqs_b2, biqs_bA2, biqs_bB2,
			biqs_c0, biqs_cA0, biqs_cB0, biqs_d1, biqs_dA1, biqs_dB1, biqs_d2, biqs_dA2, biqs_dB2,
			biqs_e0, biqs_eA0, biqs_eB0, biqs_f1, biqs_fA1, biqs_fB1, biqs_f2, biqs_fA2, biqs_fB2,
			biqs_aL1, biqs_aL2, biqs_aR1, biqs_aR2,
			biqs_cL1, biqs_cL2, biqs_cR1, biqs_cR2,
			biqs_eL1, biqs_eL2, biqs_eR1, biqs_eR2,
			biqs_outL, biqs_outR, biqs_total
		};
		double biqs[biqs_total];
		
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
	
	biqs[biqs_levelA] = biqs[biqs_levelB];
	biqs[biqs_aA0] = biqs[biqs_aB0];
	biqs[biqs_bA1] = biqs[biqs_bB1];
	biqs[biqs_bA2] = biqs[biqs_bB2];
	biqs[biqs_cA0] = biqs[biqs_cB0];
	biqs[biqs_dA1] = biqs[biqs_dB1];
	biqs[biqs_dA2] = biqs[biqs_dB2];
	biqs[biqs_eA0] = biqs[biqs_eB0];
	biqs[biqs_fA1] = biqs[biqs_fB1];
	biqs[biqs_fA2] = biqs[biqs_fB2];
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.
	
	biqs[biqs_freq] = (((pow(GetParameter( kParam_A ),4)*19980.0)+20.0)/GetSampleRate());	
	biqs[biqs_nonlin] = GetParameter( kParam_B );
	biqs[biqs_levelB] = (biqs[biqs_nonlin]*2.0)-1.0;
	if (biqs[biqs_levelB] > 0.0) biqs[biqs_levelB] *= 2.0;
	biqs[biqs_reso] = ((0.5+(biqs[biqs_nonlin]*0.5)+sqrt(biqs[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_C ),2.0)))+0.5+(biqs[biqs_nonlin]*0.5);

	double K = tan(M_PI * biqs[biqs_freq]);
	double norm = 1.0 / (1.0 + K / (biqs[biqs_reso]*1.93185165) + K * K);
	biqs[biqs_aB0] = K / (biqs[biqs_reso]*1.93185165) * norm;
	biqs[biqs_bB1] = 2.0 * (K * K - 1.0) * norm;
	biqs[biqs_bB2] = (1.0 - K / (biqs[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (biqs[biqs_reso]*0.70710678) + K * K);
	biqs[biqs_cB0] = K / (biqs[biqs_reso]*0.70710678) * norm;
	biqs[biqs_dB1] = 2.0 * (K * K - 1.0) * norm;
	biqs[biqs_dB2] = (1.0 - K / (biqs[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (biqs[biqs_reso]*0.51763809) + K * K);
	biqs[biqs_eB0] = K / (biqs[biqs_reso]*0.51763809) * norm;
	biqs[biqs_fB1] = 2.0 * (K * K - 1.0) * norm;
	biqs[biqs_fB2] = (1.0 - K / (biqs[biqs_reso]*0.51763809) + K * K) * norm;

	if (biqs[biqs_aA0] == 0.0) { // if we have just started, start directly with raw info
		biqs[biqs_levelA] = biqs[biqs_levelB];
		biqs[biqs_aA0] = biqs[biqs_aB0];
		biqs[biqs_bA1] = biqs[biqs_bB1];
		biqs[biqs_bA2] = biqs[biqs_bB2];
		biqs[biqs_cA0] = biqs[biqs_cB0];
		biqs[biqs_dA1] = biqs[biqs_dB1];
		biqs[biqs_dA2] = biqs[biqs_dB2];
		biqs[biqs_eA0] = biqs[biqs_eB0];
		biqs[biqs_fA1] = biqs[biqs_fB1];
		biqs[biqs_fA2] = biqs[biqs_fB2];
	}
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		double buf = (double)nSampleFrames/inFramesToProcess;
		biqs[biqs_level] = (biqs[biqs_levelA]*buf)+(biqs[biqs_levelB]*(1.0-buf));
		biqs[biqs_a0] = (biqs[biqs_aA0]*buf)+(biqs[biqs_aB0]*(1.0-buf));
		biqs[biqs_b1] = (biqs[biqs_bA1]*buf)+(biqs[biqs_bB1]*(1.0-buf));
		biqs[biqs_b2] = (biqs[biqs_bA2]*buf)+(biqs[biqs_bB2]*(1.0-buf));
		biqs[biqs_c0] = (biqs[biqs_cA0]*buf)+(biqs[biqs_cB0]*(1.0-buf));
		biqs[biqs_d1] = (biqs[biqs_dA1]*buf)+(biqs[biqs_dB1]*(1.0-buf));
		biqs[biqs_d2] = (biqs[biqs_dA2]*buf)+(biqs[biqs_dB2]*(1.0-buf));
		biqs[biqs_e0] = (biqs[biqs_eA0]*buf)+(biqs[biqs_eB0]*(1.0-buf));
		biqs[biqs_f1] = (biqs[biqs_fA1]*buf)+(biqs[biqs_fB1]*(1.0-buf));
		biqs[biqs_f2] = (biqs[biqs_fA2]*buf)+(biqs[biqs_fB2]*(1.0-buf));
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		biqs[biqs_outL] = inputSample * fabs(biqs[biqs_level]);
		biqs[biqs_dis] = fabs(biqs[biqs_a0] * (1.0+(biqs[biqs_outL]*biqs[biqs_nonlin])));
		if (biqs[biqs_dis] > 1.0) biqs[biqs_dis] = 1.0;
		biqs[biqs_temp] = (biqs[biqs_outL] * biqs[biqs_dis]) + biqs[biqs_aL1];
		biqs[biqs_aL1] = biqs[biqs_aL2] - (biqs[biqs_temp]*biqs[biqs_b1]);
		biqs[biqs_aL2] = (biqs[biqs_outL] * -biqs[biqs_dis]) - (biqs[biqs_temp]*biqs[biqs_b2]);
		biqs[biqs_outL] = biqs[biqs_temp];
		biqs[biqs_dis] = fabs(biqs[biqs_c0] * (1.0+(biqs[biqs_outL]*biqs[biqs_nonlin])));
		if (biqs[biqs_dis] > 1.0) biqs[biqs_dis] = 1.0;
		biqs[biqs_temp] = (biqs[biqs_outL] * biqs[biqs_dis]) + biqs[biqs_cL1];
		biqs[biqs_cL1] = biqs[biqs_cL2] - (biqs[biqs_temp]*biqs[biqs_d1]);
		biqs[biqs_cL2] = (biqs[biqs_outL] * -biqs[biqs_dis]) - (biqs[biqs_temp]*biqs[biqs_d2]);
		biqs[biqs_outL] = biqs[biqs_temp];
		biqs[biqs_dis] = fabs(biqs[biqs_e0] * (1.0+(biqs[biqs_outL]*biqs[biqs_nonlin])));
		if (biqs[biqs_dis] > 1.0) biqs[biqs_dis] = 1.0;
		biqs[biqs_temp] = (biqs[biqs_outL] * biqs[biqs_dis]) + biqs[biqs_eL1];
		biqs[biqs_eL1] = biqs[biqs_eL2] - (biqs[biqs_temp]*biqs[biqs_f1]);
		biqs[biqs_eL2] = (biqs[biqs_outL] * -biqs[biqs_dis]) - (biqs[biqs_temp]*biqs[biqs_f2]);
		biqs[biqs_outL] = biqs[biqs_temp];
		biqs[biqs_outL] *= biqs[biqs_level];
		if (biqs[biqs_level] > 1.0) biqs[biqs_outL] *= biqs[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		inputSample += biqs[biqs_outL]; //purely a parallel filter stage here
		
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
	for (int x = 0; x < biqs_total; x++) {biqs[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
