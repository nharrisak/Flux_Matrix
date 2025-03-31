#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Srsly"
#define AIRWINDOWS_DESCRIPTION "A psychoacoustic stereo processor."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','r','s' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Center", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Space", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Q", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double biquadM2[11];
	double biquadM7[11];
	double biquadM10[11];
	
	double biquadL3[11];
	double biquadL7[11];
	double biquadR3[11];
	double biquadR7[11];

	double biquadS3[11];
	double biquadS5[11];

	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	Float64 sampleRate = GetSampleRate();
	if (sampleRate < 22000) sampleRate = 22000; //keep biquads in range
	double tempSample;
	
	biquadM2[0] = 2000 / sampleRate; //up
	biquadM7[0] = 7000 / sampleRate; //down
	biquadM10[0] = 10000 / sampleRate; //down
	
	biquadL3[0] = 3000 / sampleRate; //up
	biquadL7[0] = 7000 / sampleRate; //way up
	biquadR3[0] = 3000 / sampleRate; //up
	biquadR7[0] = 7000 / sampleRate; //way up
	
	biquadS3[0] = 3000 / sampleRate; //up
	biquadS5[0] = 5000 / sampleRate; //way down
		
	Float64 focusM = 15.0-(GetParameter( kParam_One )*10.0);
	Float64 focusS = 20.0-(GetParameter( kParam_Two )*15.0);
	Float64 Q = GetParameter( kParam_Four )+0.25; //add Q control: from half to double intensity

    biquadM2[1] = focusM*0.25*Q; //Q, mid 2K boost is much broader
    biquadM7[1] = focusM*Q; //Q
    biquadM10[1] = focusM*Q; //Q
    biquadS3[1] = focusM*Q; //Q
    biquadS5[1] = focusM*Q; //Q
	
    biquadL3[1] = focusS*Q; //Q
    biquadL7[1] = focusS*Q; //Q
    biquadR3[1] = focusS*Q; //Q
    biquadR7[1] = focusS*Q; //Q
	
	double K = tan(M_PI * biquadM2[0]);
	double norm = 1.0 / (1.0 + K / biquadM2[1] + K * K);
	biquadM2[2] = K / biquadM2[1] * norm;
	biquadM2[4] = -biquadM2[2];
	biquadM2[5] = 2.0 * (K * K - 1.0) * norm;
	biquadM2[6] = (1.0 - K / biquadM2[1] + K * K) * norm;
	
	K = tan(M_PI * biquadM7[0]);
	norm = 1.0 / (1.0 + K / biquadM7[1] + K * K);
	biquadM7[2] = K / biquadM7[1] * norm;
	biquadM7[4] = -biquadM7[2];
	biquadM7[5] = 2.0 * (K * K - 1.0) * norm;
	biquadM7[6] = (1.0 - K / biquadM7[1] + K * K) * norm;
	
	K = tan(M_PI * biquadM10[0]);
	norm = 1.0 / (1.0 + K / biquadM10[1] + K * K);
	biquadM10[2] = K / biquadM10[1] * norm;
	biquadM10[4] = -biquadM10[2];
	biquadM10[5] = 2.0 * (K * K - 1.0) * norm;
	biquadM10[6] = (1.0 - K / biquadM10[1] + K * K) * norm;
	
	K = tan(M_PI * biquadL3[0]);
	norm = 1.0 / (1.0 + K / biquadL3[1] + K * K);
	biquadL3[2] = K / biquadL3[1] * norm;
	biquadL3[4] = -biquadL3[2];
	biquadL3[5] = 2.0 * (K * K - 1.0) * norm;
	biquadL3[6] = (1.0 - K / biquadL3[1] + K * K) * norm;
	
	K = tan(M_PI * biquadL7[0]);
	norm = 1.0 / (1.0 + K / biquadL7[1] + K * K);
	biquadL7[2] = K / biquadL7[1] * norm;
	biquadL7[4] = -biquadL7[2];
	biquadL7[5] = 2.0 * (K * K - 1.0) * norm;
	biquadL7[6] = (1.0 - K / biquadL7[1] + K * K) * norm;
	
	K = tan(M_PI * biquadR3[0]);
	norm = 1.0 / (1.0 + K / biquadR3[1] + K * K);
	biquadR3[2] = K / biquadR3[1] * norm;
	biquadR3[4] = -biquadR3[2];
	biquadR3[5] = 2.0 * (K * K - 1.0) * norm;
	biquadR3[6] = (1.0 - K / biquadR3[1] + K * K) * norm;
	
	K = tan(M_PI * biquadR7[0]);
	norm = 1.0 / (1.0 + K / biquadR7[1] + K * K);
	biquadR7[2] = K / biquadR7[1] * norm;
	biquadR7[4] = -biquadR7[2];
	biquadR7[5] = 2.0 * (K * K - 1.0) * norm;
	biquadR7[6] = (1.0 - K / biquadR7[1] + K * K) * norm;
	
	K = tan(M_PI * biquadS3[0]);
	norm = 1.0 / (1.0 + K / biquadS3[1] + K * K);
	biquadS3[2] = K / biquadS3[1] * norm;
	biquadS3[4] = -biquadS3[2];
	biquadS3[5] = 2.0 * (K * K - 1.0) * norm;
	biquadS3[6] = (1.0 - K / biquadS3[1] + K * K) * norm;
	
	K = tan(M_PI * biquadS5[0]);
	norm = 1.0 / (1.0 + K / biquadS5[1] + K * K);
	biquadS5[2] = K / biquadS5[1] * norm;
	biquadS5[4] = -biquadS5[2];
	biquadS5[5] = 2.0 * (K * K - 1.0) * norm;
	biquadS5[6] = (1.0 - K / biquadS5[1] + K * K) * norm;
	
	Float64 depthM = pow(GetParameter( kParam_One ),2)*2.0; //proportion to mix in the filtered stuff
	Float64 depthS = pow(GetParameter( kParam_Two ),2)*2.0; //proportion to mix in the filtered stuff
	Float64 level = GetParameter( kParam_Three ); //output pad
	Float64 wet = GetParameter( kParam_Five ); //dry/wet
		
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
		
		double mid = inputSampleL + inputSampleR;
		double rawmid = mid * 0.5; //we'll use this to isolate L&R a little
		double side = inputSampleL - inputSampleR;
		double boostside = side * depthS;
		//assign mid and side.Between these sections, you can do mid/side processing
				
		tempSample = (mid * biquadM2[2]) + biquadM2[7];
		biquadM2[7] = (-tempSample * biquadM2[5]) + biquadM2[8];
		biquadM2[8] = (mid * biquadM2[4]) - (tempSample * biquadM2[6]);
		double M2Sample = tempSample; //like mono AU, 7 and 8 store L channel
		
		tempSample = (mid * biquadM7[2]) + biquadM7[7];
		biquadM7[7] = (-tempSample * biquadM7[5]) + biquadM7[8];
		biquadM7[8] = (mid * biquadM7[4]) - (tempSample * biquadM7[6]);
		double M7Sample = -tempSample*2.0; //like mono AU, 7 and 8 store L channel
		
		tempSample = (mid * biquadM10[2]) + biquadM10[7];
		biquadM10[7] = (-tempSample * biquadM10[5]) + biquadM10[8];
		biquadM10[8] = (mid * biquadM10[4]) - (tempSample * biquadM10[6]);
		double M10Sample = -tempSample*2.0; //like mono AU, 7 and 8 store L channel
		//mid
		
		tempSample = (side * biquadS3[2]) + biquadS3[7];
		biquadS3[7] = (-tempSample * biquadS3[5]) + biquadS3[8];
		biquadS3[8] = (side * biquadS3[4]) - (tempSample * biquadS3[6]);
		double S3Sample = tempSample*2.0; //like mono AU, 7 and 8 store L channel
		
		tempSample = (side * biquadS5[2]) + biquadS5[7];
		biquadS5[7] = (-tempSample * biquadS5[5]) + biquadS5[8];
		biquadS5[8] = (side * biquadS5[4]) - (tempSample * biquadS5[6]);
		double S5Sample = -tempSample*5.0; //like mono AU, 7 and 8 store L channel
		
		mid = (M2Sample + M7Sample + M10Sample)*depthM;
		side = (S3Sample + S5Sample + boostside)*depthS;
		
		double msOutSampleL = (mid+side)/2.0;
		double msOutSampleR = (mid-side)/2.0;
		//unassign mid and side
		
		double isoSampleL = inputSampleL-rawmid;
		double isoSampleR = inputSampleR-rawmid; //trying to isolate L and R a little
		
		tempSample = (isoSampleL * biquadL3[2]) + biquadL3[7];
		biquadL3[7] = (-tempSample * biquadL3[5]) + biquadL3[8];
		biquadL3[8] = (isoSampleL * biquadL3[4]) - (tempSample * biquadL3[6]);
		double L3Sample = tempSample; //like mono AU, 7 and 8 store L channel
		
		tempSample = (isoSampleR * biquadR3[2]) + biquadR3[9];
		biquadR3[9] = (-tempSample * biquadR3[5]) + biquadR3[10];
		biquadR3[10] = (isoSampleR * biquadR3[4]) - (tempSample * biquadR3[6]);
		double R3Sample = tempSample; //note: 9 and 10 store the R channel
		
		tempSample = (isoSampleL * biquadL7[2]) + biquadL7[7];
		biquadL7[7] = (-tempSample * biquadL7[5]) + biquadL7[8];
		biquadL7[8] = (isoSampleL * biquadL7[4]) - (tempSample * biquadL7[6]);
		double L7Sample = tempSample*3.0; //like mono AU, 7 and 8 store L channel
		
		tempSample = (isoSampleR * biquadR7[2]) + biquadR7[9];
		biquadR7[9] = (-tempSample * biquadR7[5]) + biquadR7[10];
		biquadR7[10] = (isoSampleR * biquadR7[4]) - (tempSample * biquadR7[6]);
		double R7Sample = tempSample*3.0; //note: 9 and 10 store the R channel
		
		double processingL = msOutSampleL + ((L3Sample + L7Sample)*depthS);
		double processingR = msOutSampleR + ((R3Sample + R7Sample)*depthS);
		//done with making filters, now we apply them
				
		inputSampleL += processingL;
		inputSampleR += processingR;
		
		if (level < 1.0) {
			inputSampleL *= level;
			inputSampleR *= level;
		}		
		
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSampleL = asin(inputSampleL);
		inputSampleR = asin(inputSampleR);
		//amplitude aspect
				
		if (wet < 1.0) {
			inputSampleL = (inputSampleL * wet)+(drySampleL * (1.0-wet));
			inputSampleR = (inputSampleR * wet)+(drySampleR * (1.0-wet));
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
	for (int x = 0; x < 11; x++) {
		biquadM2[x] = 0.0;
		biquadM7[x] = 0.0;
		biquadM10[x] = 0.0;
		biquadL3[x] = 0.0;
		biquadL7[x] = 0.0;
		biquadR3[x] = 0.0;
		biquadR7[x] = 0.0;
		biquadS3[x] = 0.0;
		biquadS5[x] = 0.0;
	}
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
