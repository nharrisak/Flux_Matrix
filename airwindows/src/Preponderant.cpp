#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Preponderant"
#define AIRWINDOWS_DESCRIPTION "Airwindows Anti-Soothe, a strange tone intensifier and balancer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','f' )
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
{ .name = "Narrow", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Medium", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wide", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Resonance", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double biquadA[9];
		double biquadB[9];
		double biquadC[9];
		double bassBalance;
		double midBalance;
		double highBalance;
		double bassTrack;
		double midTrack;
		double highTrack;
		double quickness;
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
	
	Float64 chase = 0.00005 / overallscale;
			
	biquadA[0] = pow(GetParameter( kParam_One ),(3.0*cbrt(overallscale)))*0.42;
	if (biquadA[0] < 0.0001) biquadA[0] = 0.0001;
	
	biquadB[0] = pow(GetParameter( kParam_Two ),(3.0*cbrt(overallscale)))*0.42;
	if (biquadB[0] < 0.0001) biquadB[0] = 0.0001;
	
	biquadC[0] = pow(GetParameter( kParam_Three ),(3.0*cbrt(overallscale)))*0.42;
	if (biquadC[0] < 0.0001) biquadC[0] = 0.0001;
	
    biquadA[1] = biquadB[1] = biquadC[1] = (pow(GetParameter( kParam_Four ),3)*8.0)+0.33;
	biquadB[1] /= 2.0; biquadC[1] /= 4.0;
	
	Float64 volumeCompensation = sqrt(biquadA[1]);

	Float64 wet = GetParameter( kParam_Five );
		
	double K = tan(M_PI * biquadA[0]);
	double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
	biquadA[2] = K / biquadA[1] * norm;
	biquadA[3] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadB[0]);
	norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
	biquadB[2] = K / biquadB[1] * norm;
	biquadB[3] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadB[4] = -biquadB[2];
	biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
	
	K = tan(M_PI * biquadC[0]);
	norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
	biquadC[2] = K / biquadC[1] * norm;
	biquadC[3] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
	biquadC[4] = -biquadC[2];
	biquadC[5] = 2.0 * (K * K - 1.0) * norm;
	biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = *sourceP;
				
		double tempSample = (inputSample * biquadA[2]) + biquadA[7];
		biquadA[7] = -(tempSample * biquadA[5]) + biquadA[8];
		biquadA[8] = (inputSample * biquadA[4]) - (tempSample * biquadA[6]);
		double bassSample = tempSample; //BANDPASS form
		
		tempSample = (inputSample * biquadB[2]) + biquadB[7];
		biquadB[7] = -(tempSample * biquadB[5]) + biquadB[8];
		biquadB[8] = (inputSample * biquadB[4]) - (tempSample * biquadB[6]);
		double midSample = tempSample; //BANDPASS form
		
		tempSample = (inputSample * biquadC[2]) + biquadC[7];
		biquadC[7] = -(tempSample * biquadC[5]) + biquadC[8];
		biquadC[8] = (inputSample * biquadC[4]) - (tempSample * biquadC[6]);
		double highSample = tempSample; //BANDPASS form
		
		if (bassBalance < 0.00125) bassBalance = 0.00125;
		if (bassBalance > 1.0) bassBalance = 1.0;
		
		if (midBalance < 0.00125) midBalance = 0.00125;
		if (midBalance > 1.0) midBalance = 1.0;
		
		if (highBalance < 0.00125) highBalance = 0.00125;
		if (highBalance > 1.0) highBalance = 1.0;
				
		bassSample *= bassBalance;
		midSample *= midBalance;
		highSample *= highBalance;
		
		if (bassTrack < fabs(bassSample)) bassTrack = fabs(bassSample);
		if (midTrack < fabs(midSample)) midTrack = fabs(midSample);
		if (highTrack < fabs(highSample)) highTrack = fabs(highSample);
		if (bassTrack > quickness) bassTrack -= quickness;
		if (midTrack > quickness) midTrack -= quickness;
		if (highTrack > quickness) highTrack -= quickness;
		
		
		Float64 disparity = 0.0;
		if (disparity < fabs(bassTrack - midTrack)) disparity = fabs(bassTrack - midTrack);
		if (disparity < fabs(bassTrack - highTrack)) disparity = fabs(bassTrack - highTrack);
		if (disparity < fabs(midTrack - highTrack)) disparity = fabs(midTrack - highTrack);
		
		quickness *= (1.0-chase);
		quickness += ((1.0+disparity) * 0.0001 * chase);
		
		
		if (bassTrack > midTrack) {bassBalance -= quickness; midBalance += quickness;}
		else {bassBalance += quickness; midBalance -= quickness;}
		if (midTrack > highTrack) {midBalance -= quickness; highBalance += quickness;}
		else {midBalance += quickness; highBalance -= quickness;}
		if (highTrack > bassTrack) {highBalance -= quickness; bassBalance += quickness;}
		else {highBalance += quickness; bassBalance -= quickness;}
		
		if (highBalance > 0.618033988749894848204586) highBalance -= (quickness*0.001); else highBalance += (quickness*0.001);
		if (midBalance > 0.618033988749894848204586) midBalance -= (quickness*0.001); else midBalance += (quickness*0.001);
		if (bassBalance > 0.618033988749894848204586) bassBalance -= (quickness*0.001); else bassBalance += (quickness*0.001);
		
		inputSample = (bassSample + midSample + highSample) * volumeCompensation;
		
		if (wet < 1.0) {
			inputSample = (inputSample*wet) + (drySample*(1.0-wet));
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
	for (int x = 0; x < 9; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;biquadC[x] = 0.0;}
	bassBalance = midBalance = highBalance = 1.0;
	bassTrack = midTrack = highTrack = 0.0;
	quickness = 0.0001;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
