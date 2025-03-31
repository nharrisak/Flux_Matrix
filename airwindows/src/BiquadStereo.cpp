#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadStereo"
#define AIRWINDOWS_DESCRIPTION "BiquadStereo"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','y' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Type", .min = 1000, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 10, .max = 30000, .def = 707, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double biquad[11];
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
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
	//[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
	//[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
	//[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)
	
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
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		
		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		//encode Console5: good cleanness
		
		/*
		double mid = inputSampleL + inputSampleR;
		double side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		
		double tempSampleM = (mid * biquad[2]) + biquad[7];
		biquad[7] = (mid * biquad[3]) - (tempSampleM * biquad[5]) + biquad[8];
		biquad[8] = (mid * biquad[4]) - (tempSampleM * biquad[6]);
		mid = tempSampleM; //like mono AU, 7 and 8 store mid channel
		
		double tempSampleS = (side * biquad[2]) + biquad[9];
		biquad[9] = (side * biquad[3]) - (tempSampleS * biquad[5]) + biquad[10];
		biquad[10] = (side * biquad[4]) - (tempSampleS * biquad[6]);
		inputSampleR = tempSampleS; //note: 9 and 10 store the side channel		
		
		inputSampleL = (mid+side)/2.0;
		inputSampleR = (mid-side)/2.0;
		//unassign mid and side
		*/
		
		double tempSampleL = (inputSampleL * biquad[2]) + biquad[7];
		biquad[7] = (inputSampleL * biquad[3]) - (tempSampleL * biquad[5]) + biquad[8];
		biquad[8] = (inputSampleL * biquad[4]) - (tempSampleL * biquad[6]);
		inputSampleL = tempSampleL; //like mono AU, 7 and 8 store L channel

		double tempSampleR = (inputSampleR * biquad[2]) + biquad[9];
		biquad[9] = (inputSampleR * biquad[3]) - (tempSampleR * biquad[5]) + biquad[10];
		biquad[10] = (inputSampleR * biquad[4]) - (tempSampleR * biquad[6]);
		inputSampleR = tempSampleR; //note: 9 and 10 store the R channel
		
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		//amplitude aspect
		
		if (wet < 1.0) {
			inputSampleL = (inputSampleL*wet) + (drySampleL*(1.0-fabs(wet)));
			inputSampleR = (inputSampleR*wet) + (drySampleR*(1.0-fabs(wet)));
			//inv/dry/wet lets us turn LP into HP and band into notch
		}
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	for (int x = 0; x < 11; x++) {biquad[x] = 0.0;}
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
