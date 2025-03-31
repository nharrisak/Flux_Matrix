#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Isolator2"
#define AIRWINDOWS_DESCRIPTION "Isolator, but with control smoothing and a new resonance booster."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','s','p' )
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		enum {
			biq_freq,
			biq_reso,
			biq_a0,
			biq_a1,
			biq_a2,
			biq_b1,
			biq_b2,
			biq_aA0,
			biq_aA1,
			biq_aA2,
			biq_bA1,
			biq_bA2,
			biq_aB0,
			biq_aB1,
			biq_aB2,
			biq_bB1,
			biq_bB2,
			biq_sL1,
			biq_sL2,
			biq_sR1,
			biq_sR2,
			biq_total
		}; //coefficient interpolating biquad filter, stereo
		double biquadA[biq_total];
		double biquadB[biq_total];
		double biquadC[biq_total];
		double biquadD[biq_total];
		double biquadE[biq_total];
		double biquadF[biq_total];
		double biquadG[biq_total];
		double highA;
		double highB;
		double lowA;
		double lowB;
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
	
	biquadA[biq_freq] = pow(GetParameter( kParam_One ),(2.0*sqrt(overallscale)))*0.4999;
	if (biquadA[biq_freq] < 0.0005) biquadA[biq_freq] = 0.0005;
	biquadG[biq_freq] = biquadF[biq_freq] = biquadE[biq_freq] = biquadD[biq_freq] = biquadC[biq_freq] = biquadB[biq_freq] = biquadA[biq_freq];
	
	double reso = pow(GetParameter( kParam_Two ),2);
	double resoBoost = reso+1.0;
	reso = 1.0-reso;
	
	biquadA[biq_reso] = 4.46570214 * resoBoost;
	biquadB[biq_reso] = 1.51387132 * resoBoost;
	biquadC[biq_reso] = 0.93979296 * resoBoost;
	biquadD[biq_reso] = 0.70710678 * resoBoost;
	biquadE[biq_reso] = 0.59051105 * resoBoost;
	biquadF[biq_reso] = 0.52972649 * resoBoost;
	biquadG[biq_reso] = 0.50316379 * resoBoost;
	
	biquadA[biq_aA0] = biquadA[biq_aB0];
	biquadA[biq_aA1] = biquadA[biq_aB1];
	biquadA[biq_aA2] = biquadA[biq_aB2];
	biquadA[biq_bA1] = biquadA[biq_bB1];
	biquadA[biq_bA2] = biquadA[biq_bB2];
	
	biquadB[biq_aA0] = biquadB[biq_aB0];
	biquadB[biq_aA1] = biquadB[biq_aB1];
	biquadB[biq_aA2] = biquadB[biq_aB2];
	biquadB[biq_bA1] = biquadB[biq_bB1];
	biquadB[biq_bA2] = biquadB[biq_bB2];
	
	biquadC[biq_aA0] = biquadC[biq_aB0];
	biquadC[biq_aA1] = biquadC[biq_aB1];
	biquadC[biq_aA2] = biquadC[biq_aB2];
	biquadC[biq_bA1] = biquadC[biq_bB1];
	biquadC[biq_bA2] = biquadC[biq_bB2];
	
	biquadD[biq_aA0] = biquadD[biq_aB0];
	biquadD[biq_aA1] = biquadD[biq_aB1];
	biquadD[biq_aA2] = biquadD[biq_aB2];
	biquadD[biq_bA1] = biquadD[biq_bB1];
	biquadD[biq_bA2] = biquadD[biq_bB2];
	
	biquadE[biq_aA0] = biquadE[biq_aB0];
	biquadE[biq_aA1] = biquadE[biq_aB1];
	biquadE[biq_aA2] = biquadE[biq_aB2];
	biquadE[biq_bA1] = biquadE[biq_bB1];
	biquadE[biq_bA2] = biquadE[biq_bB2];
	
	biquadF[biq_aA0] = biquadF[biq_aB0];
	biquadF[biq_aA1] = biquadF[biq_aB1];
	biquadF[biq_aA2] = biquadF[biq_aB2];
	biquadF[biq_bA1] = biquadF[biq_bB1];
	biquadF[biq_bA2] = biquadF[biq_bB2];
	
	biquadG[biq_aA0] = biquadG[biq_aB0];
	biquadG[biq_aA1] = biquadG[biq_aB1];
	biquadG[biq_aA2] = biquadG[biq_aB2];
	biquadG[biq_bA1] = biquadG[biq_bB1];
	biquadG[biq_bA2] = biquadG[biq_bB2];
	
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.	
	double K = tan(M_PI * biquadA[biq_freq]);
	double norm = 1.0 / (1.0 + K / biquadA[biq_reso] + K * K);
	biquadA[biq_aB0] = K * K * norm;
	biquadA[biq_aB1] = 2.0 * biquadA[biq_aB0];
	biquadA[biq_aB2] = biquadA[biq_aB0];
	biquadA[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadA[biq_bB2] = (1.0 - K / biquadA[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * biquadB[biq_freq]);
	norm = 1.0 / (1.0 + K / biquadB[biq_reso] + K * K);
	biquadB[biq_aB0] = K * K * norm;
	biquadB[biq_aB1] = 2.0 * biquadB[biq_aB0];
	biquadB[biq_aB2] = biquadB[biq_aB0];
	biquadB[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadB[biq_bB2] = (1.0 - K / biquadB[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * biquadC[biq_freq]);
	norm = 1.0 / (1.0 + K / biquadC[biq_reso] + K * K);
	biquadC[biq_aB0] = K * K * norm;
	biquadC[biq_aB1] = 2.0 * biquadC[biq_aB0];
	biquadC[biq_aB2] = biquadC[biq_aB0];
	biquadC[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadC[biq_bB2] = (1.0 - K / biquadC[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * biquadD[biq_freq]);
	norm = 1.0 / (1.0 + K / biquadD[biq_reso] + K * K);
	biquadD[biq_aB0] = K * K * norm;
	biquadD[biq_aB1] = 2.0 * biquadD[biq_aB0];
	biquadD[biq_aB2] = biquadD[biq_aB0];
	biquadD[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadD[biq_bB2] = (1.0 - K / biquadD[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * biquadE[biq_freq]);
	norm = 1.0 / (1.0 + K / biquadE[biq_reso] + K * K);
	biquadE[biq_aB0] = K * K * norm;
	biquadE[biq_aB1] = 2.0 * biquadE[biq_aB0];
	biquadE[biq_aB2] = biquadE[biq_aB0];
	biquadE[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadE[biq_bB2] = (1.0 - K / biquadE[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * biquadF[biq_freq]);
	norm = 1.0 / (1.0 + K / biquadF[biq_reso] + K * K);
	biquadF[biq_aB0] = K * K * norm;
	biquadF[biq_aB1] = 2.0 * biquadF[biq_aB0];
	biquadF[biq_aB2] = biquadF[biq_aB0];
	biquadF[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadF[biq_bB2] = (1.0 - K / biquadF[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * biquadG[biq_freq]);
	norm = 1.0 / (1.0 + K / biquadG[biq_reso] + K * K);
	biquadG[biq_aB0] = K * K * norm;
	biquadG[biq_aB1] = 2.0 * biquadG[biq_aB0];
	biquadG[biq_aB2] = biquadG[biq_aB0];
	biquadG[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
	biquadG[biq_bB2] = (1.0 - K / biquadG[biq_reso] + K * K) * norm;
	
	bool bypass = (GetParameter( kParam_One ) == 1.0);
	highA = highB;
	highB = GetParameter( kParam_Three )*reso;
	if (highB > 0.0) bypass = false;
	lowA = lowB;
	lowB = GetParameter( kParam_Four )*reso;
	if (lowB < 1.0) bypass = false;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		
		biquadA[biq_a0] = (biquadA[biq_aA0]*temp)+(biquadA[biq_aB0]*(1.0-temp));
		biquadA[biq_a1] = (biquadA[biq_aA1]*temp)+(biquadA[biq_aB1]*(1.0-temp));
		biquadA[biq_a2] = (biquadA[biq_aA2]*temp)+(biquadA[biq_aB2]*(1.0-temp));
		biquadA[biq_b1] = (biquadA[biq_bA1]*temp)+(biquadA[biq_bB1]*(1.0-temp));
		biquadA[biq_b2] = (biquadA[biq_bA2]*temp)+(biquadA[biq_bB2]*(1.0-temp));
		
		biquadB[biq_a0] = (biquadB[biq_aA0]*temp)+(biquadB[biq_aB0]*(1.0-temp));
		biquadB[biq_a1] = (biquadB[biq_aA1]*temp)+(biquadB[biq_aB1]*(1.0-temp));
		biquadB[biq_a2] = (biquadB[biq_aA2]*temp)+(biquadB[biq_aB2]*(1.0-temp));
		biquadB[biq_b1] = (biquadB[biq_bA1]*temp)+(biquadB[biq_bB1]*(1.0-temp));
		biquadB[biq_b2] = (biquadB[biq_bA2]*temp)+(biquadB[biq_bB2]*(1.0-temp));
		
		biquadC[biq_a0] = (biquadC[biq_aA0]*temp)+(biquadC[biq_aB0]*(1.0-temp));
		biquadC[biq_a1] = (biquadC[biq_aA1]*temp)+(biquadC[biq_aB1]*(1.0-temp));
		biquadC[biq_a2] = (biquadC[biq_aA2]*temp)+(biquadC[biq_aB2]*(1.0-temp));
		biquadC[biq_b1] = (biquadC[biq_bA1]*temp)+(biquadC[biq_bB1]*(1.0-temp));
		biquadC[biq_b2] = (biquadC[biq_bA2]*temp)+(biquadC[biq_bB2]*(1.0-temp));
		
		biquadD[biq_a0] = (biquadD[biq_aA0]*temp)+(biquadD[biq_aB0]*(1.0-temp));
		biquadD[biq_a1] = (biquadD[biq_aA1]*temp)+(biquadD[biq_aB1]*(1.0-temp));
		biquadD[biq_a2] = (biquadD[biq_aA2]*temp)+(biquadD[biq_aB2]*(1.0-temp));
		biquadD[biq_b1] = (biquadD[biq_bA1]*temp)+(biquadD[biq_bB1]*(1.0-temp));
		biquadD[biq_b2] = (biquadD[biq_bA2]*temp)+(biquadD[biq_bB2]*(1.0-temp));
		
		biquadE[biq_a0] = (biquadE[biq_aA0]*temp)+(biquadE[biq_aB0]*(1.0-temp));
		biquadE[biq_a1] = (biquadE[biq_aA1]*temp)+(biquadE[biq_aB1]*(1.0-temp));
		biquadE[biq_a2] = (biquadE[biq_aA2]*temp)+(biquadE[biq_aB2]*(1.0-temp));
		biquadE[biq_b1] = (biquadE[biq_bA1]*temp)+(biquadE[biq_bB1]*(1.0-temp));
		biquadE[biq_b2] = (biquadE[biq_bA2]*temp)+(biquadE[biq_bB2]*(1.0-temp));
		
		biquadF[biq_a0] = (biquadF[biq_aA0]*temp)+(biquadF[biq_aB0]*(1.0-temp));
		biquadF[biq_a1] = (biquadF[biq_aA1]*temp)+(biquadF[biq_aB1]*(1.0-temp));
		biquadF[biq_a2] = (biquadF[biq_aA2]*temp)+(biquadF[biq_aB2]*(1.0-temp));
		biquadF[biq_b1] = (biquadF[biq_bA1]*temp)+(biquadF[biq_bB1]*(1.0-temp));
		biquadF[biq_b2] = (biquadF[biq_bA2]*temp)+(biquadF[biq_bB2]*(1.0-temp));
		
		biquadG[biq_a0] = (biquadG[biq_aA0]*temp)+(biquadG[biq_aB0]*(1.0-temp));
		biquadG[biq_a1] = (biquadG[biq_aA1]*temp)+(biquadG[biq_aB1]*(1.0-temp));
		biquadG[biq_a2] = (biquadG[biq_aA2]*temp)+(biquadG[biq_aB2]*(1.0-temp));
		biquadG[biq_b1] = (biquadG[biq_bA1]*temp)+(biquadG[biq_bB1]*(1.0-temp));
		biquadG[biq_b2] = (biquadG[biq_bA2]*temp)+(biquadG[biq_bB2]*(1.0-temp));
		
		//this is the interpolation code for the biquad
		double high = (highA*temp)+(highB*(1.0-temp));
		double low = (lowA*temp)+(lowB*(1.0-temp));
		
		double outSample = (inputSample * biquadA[biq_a0]) + biquadA[biq_sL1];
		biquadA[biq_sL1] = (inputSample * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sL2];
		biquadA[biq_sL2] = (inputSample * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * biquadB[biq_a0]) + biquadB[biq_sL1];
		biquadB[biq_sL1] = (inputSample * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sL2];
		biquadB[biq_sL2] = (inputSample * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * biquadC[biq_a0]) + biquadC[biq_sL1];
		biquadC[biq_sL1] = (inputSample * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sL2];
		biquadC[biq_sL2] = (inputSample * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * biquadD[biq_a0]) + biquadD[biq_sL1];
		biquadD[biq_sL1] = (inputSample * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sL2];
		biquadD[biq_sL2] = (inputSample * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * biquadE[biq_a0]) + biquadE[biq_sL1];
		biquadE[biq_sL1] = (inputSample * biquadE[biq_a1]) - (outSample * biquadE[biq_b1]) + biquadE[biq_sL2];
		biquadE[biq_sL2] = (inputSample * biquadE[biq_a2]) - (outSample * biquadE[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * biquadF[biq_a0]) + biquadF[biq_sL1];
		biquadF[biq_sL1] = (inputSample * biquadF[biq_a1]) - (outSample * biquadF[biq_b1]) + biquadF[biq_sL2];
		biquadF[biq_sL2] = (inputSample * biquadF[biq_a2]) - (outSample * biquadF[biq_b2]);
		inputSample = outSample;
		
		outSample = (inputSample * biquadG[biq_a0]) + biquadG[biq_sL1];
		biquadG[biq_sL1] = (inputSample * biquadG[biq_a1]) - (outSample * biquadG[biq_b1]) + biquadG[biq_sL2];
		biquadG[biq_sL2] = (inputSample * biquadG[biq_a2]) - (outSample * biquadG[biq_b2]);
		inputSample = outSample;
		
		if (bypass) inputSample = drySample;
		else inputSample = (inputSample * low) + ((drySample - inputSample)*high);
		
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
	for (int x = 0; x < biq_total; x++) {
		biquadA[x] = 0.0;
		biquadB[x] = 0.0;
		biquadC[x] = 0.0;
		biquadD[x] = 0.0;
		biquadE[x] = 0.0;
		biquadF[x] = 0.0;
		biquadG[x] = 0.0;
	}
	highA = 0.0; highB = 0.0;
	lowA = 1.0; lowB = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
