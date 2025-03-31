#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Biquad"
#define AIRWINDOWS_DESCRIPTION "The Airwindows implementation of a biquad filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','q' )
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
{ .name = "Type", .min = 1000, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 10, .max = 30000, .def = 707, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double biquad[9];
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
	
	int type = GetParameter( kParam_One);
	
	biquad[0] = GetParameter( kParam_Two )*0.499;
	if (biquad[0] < 0.0001) biquad[0] = 0.0001;
	
    biquad[1] = GetParameter( kParam_Three );
	if (biquad[1] < 0.0001) biquad[1] = 0.0001;
	
	Float64 wet = GetParameter( kParam_Four );
	
	//biquad contains these values:
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	//[2] is a0 but you need distinct ones for additional biquad instances so it's here
	//[3] is a1 but you need distinct ones for additional biquad instances so it's here
	//[4] is a2 but you need distinct ones for additional biquad instances so it's here
	//[5] is b1 but you need distinct ones for additional biquad instances so it's here
	//[6] is b2 but you need distinct ones for additional biquad instances so it's here
	//[7] is a stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[8] is a stored delayed sample (you have to include the coefficient making code if you do that)
	
	//to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
	//put this code either within the sample buffer (for smoothly modulating freq or res)
	//or in this 'read the controls' area (for letting you change freq and res with controls)
	//or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
	
	if (type == 1) { //lowpass
		double K = tan(M_PI * biquad[0]);
		double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
		biquad[2] = K * K * norm;
		biquad[3] = 2.0 * biquad[2];
		biquad[4] = biquad[2];
		biquad[5] = 2.0 * (K * K - 1.0) * norm;
		biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
	}
	
	if (type == 2) { //highpass
		double K = tan(M_PI * biquad[0]);
		double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
		biquad[2] = norm;
		biquad[3] = -2.0 * biquad[2];
		biquad[4] = biquad[2];
		biquad[5] = 2.0 * (K * K - 1.0) * norm;
		biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
	}
	
	if (type == 3) { //bandpass
		double K = tan(M_PI * biquad[0]);
		double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
		biquad[2] = K / biquad[1] * norm;
		biquad[3] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
		biquad[4] = -biquad[2];
		biquad[5] = 2.0 * (K * K - 1.0) * norm;
		biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
	}
	
	if (type == 4) { //notch
		double K = tan(M_PI * biquad[0]);
		double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
		biquad[2] = (1.0 + K * K) * norm;
		biquad[3] = 2.0 * (K * K - 1) * norm;
		biquad[4] = biquad[2];
		biquad[5] = biquad[3];
		biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
	}
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = *sourceP;
		
		
		inputSample = sin(inputSample);
		//encode Console5: good cleanness
		
		double tempSample = (inputSample * biquad[2]) + biquad[7];
		biquad[7] = (inputSample * biquad[3]) - (tempSample * biquad[5]) + biquad[8];
		biquad[8] = (inputSample * biquad[4]) - (tempSample * biquad[6]);
		inputSample = tempSample;
				
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
		if (wet < 1.0) {
			inputSample = (inputSample*wet) + (drySample*(1.0-fabs(wet)));
			//inv/dry/wet lets us turn LP into HP and band into notch
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
	for (int x = 0; x < 9; x++) {biquad[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
