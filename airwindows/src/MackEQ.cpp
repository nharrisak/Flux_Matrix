#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MackEQ"
#define AIRWINDOWS_DESCRIPTION "The Mackie distortion but with treble and bass controls added."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','a','c' )
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
{ .name = "Trim", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hi", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lo", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		
		double iirSampleA;
		double iirSampleB;
		double iirSampleC;
		double iirSampleD;
		double iirSampleE;
		double iirSampleF;
		double biquadA[11];
		double biquadB[11];
		double biquadC[11];
		double biquadD[11];
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
	
	double inTrim = GetParameter( kParam_One )*10.0;
	inTrim *= inTrim;
	
	double gainHigh = pow(GetParameter( kParam_Two ),2)*4.0;
	double outHigh = sqrt(GetParameter( kParam_Two ));
	double gainBass = pow(GetParameter( kParam_Three ),2)*4.0;
	double outBass = sqrt(GetParameter( kParam_Three ));
	double outPad = GetParameter( kParam_Four );
	double wet = GetParameter( kParam_Five );
	
	double iirAmountA = 0.001860867/overallscale;
	double iirAmountB = 0.000287496/overallscale;
	double iirBassMid = 0.159/overallscale;
	double iirMidHigh = 0.236/overallscale;
	
	biquadD[0] = biquadC[0] = biquadB[0] = biquadA[0] = 19160.0 / GetSampleRate();
    biquadA[1] = 0.431684981684982;
	biquadB[1] = 1.1582298;
	biquadC[1] = 0.657027382751269;
	biquadD[1] = 1.076210852946577;
	
	double K = tan(M_PI * biquadA[0]); //lowpass
	double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0 * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]);
	norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0 * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
	
	K = tan(M_PI * biquadC[0]);
	norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
	biquadC[2] = K * K * norm;
	biquadC[3] = 2.0 * biquadC[2];
	biquadC[4] = biquadC[2];
	biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
	
	K = tan(M_PI * biquadD[0]);
	norm = 1.0 / (1.0 + K / biquadD[1] + K * K);
	biquadD[2] = K * K * norm;
	biquadD[3] = 2.0 * biquadD[2];
	biquadD[4] = biquadD[2];
	biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	biquadD[6] = (1.0 - K / biquadD[1] + K * K) * norm;
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		if (fabs(iirSampleA)<1.18e-37) iirSampleA = 0.0;
		iirSampleA = (iirSampleA * (1.0 - iirAmountA)) + (inputSample * iirAmountA);
		inputSample -= iirSampleA;
		
		if (inTrim != 1.0) inputSample *= inTrim;
		
		//begin Mackity input stage
		double outSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
		biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = outSample; biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1		
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		inputSample -= pow(inputSample,5)*0.1768;
		
		outSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
		biquadB[8] = biquadB[7]; biquadB[7] = inputSample; inputSample = outSample; biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
		
		if (fabs(iirSampleB)<1.18e-37) iirSampleB = 0.0;
		iirSampleB = (iirSampleB * (1.0 - iirAmountB)) + (inputSample * iirAmountB);
		inputSample -= iirSampleB;
		//end Mackity input stage
		
		//begin EQ section
		if (fabs(iirSampleC)<1.18e-37) iirSampleC = 0.0;
		iirSampleC = (iirSampleC * (1.0 - iirBassMid)) + (inputSample * iirBassMid);
		
		double bassSample = iirSampleC;
		double midSample = inputSample - bassSample;

		if (gainBass != 1.0) bassSample *= gainBass;
		if (bassSample > 1.0) bassSample = 1.0;
		if (bassSample < -1.0) bassSample = -1.0;
		bassSample -= pow(bassSample,5)*0.1768;
		
		if (fabs(iirSampleD)<1.18e-37) iirSampleD = 0.0;
		iirSampleD = (iirSampleD * (1.0 - iirBassMid)) + (bassSample * iirBassMid);
		bassSample = iirSampleD;
		//we've taken the bass sample, made the mids from it, distorted it
		//and hit it with another pole of darkening.
		//mid sample is still normal from undistorted bass
		
		if (fabs(iirSampleE)<1.18e-37) iirSampleE = 0.0;
		iirSampleE = (iirSampleE * (1.0 - iirMidHigh)) + (midSample * iirMidHigh);
		double highSample = midSample - iirSampleE;
		midSample = iirSampleE;
		//here is where we make the high sample out of the mid, and take highs
		//away from the mid.
		
		if (fabs(iirSampleF)<1.18e-37) iirSampleF = 0.0;
		iirSampleF = (iirSampleF * (1.0 - iirMidHigh)) + (highSample * iirMidHigh);
		highSample -= iirSampleF;
		
		if (gainHigh != 1.0) highSample *= gainHigh;
		if (highSample > 1.0) highSample = 1.0;
		if (highSample < -1.0) highSample = -1.0;
		highSample -= pow(highSample,5)*0.1768;
		//highpassing HighSample another stage, before distorting it
		
		inputSample = ((bassSample*outBass) + midSample + (highSample*outHigh))*4.0;
		//end EQ section
		
		outSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
		biquadC[8] = biquadC[7]; biquadC[7] = inputSample; inputSample = outSample; biquadC[10] = biquadC[9]; biquadC[9] = inputSample; //DF1
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		inputSample -= pow(inputSample,5)*0.1768;
		
		outSample = biquadD[2]*inputSample+biquadD[3]*biquadD[7]+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
		biquadD[8] = biquadD[7]; biquadD[7] = inputSample; inputSample = outSample; biquadD[10] = biquadD[9]; biquadD[9] = inputSample; //DF1
				
		if (outPad != 1.0) inputSample *= outPad;

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
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	for (int x = 0; x < 11; x++) {biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
