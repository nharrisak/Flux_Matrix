#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Stonefire"
#define AIRWINDOWS_DESCRIPTION "The non-EQ EQ designed for ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','p' )
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
{ .name = "Air", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fire", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
			pvAL1,
			pvSL1,
			accSL1,
			acc2SL1,
			pvAL2,
			pvSL2,
			accSL2,
			acc2SL2,
			pvAL3,
			pvSL3,
			accSL3,
			pvAL4,
			pvSL4,
			gndavgL,
			outAL,
			gainAL,
			air_total
		};
		double air[air_total];
		
		enum {
			prevSampL1,
			prevSlewL1,
			accSlewL1,
			prevSampL2,
			prevSlewL2,
			accSlewL2,
			prevSampL3,
			prevSlewL3,
			accSlewL3,
			kalGainL,
			kalOutL,
			kal_total
		};
		double kal[kal_total];
				
		double trebleGainA;
		double trebleGainB;
		double midGainA;
		double midGainB;
		double bassGainA;
		double bassGainB;
		
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
		
	trebleGainA = trebleGainB; trebleGainB = GetParameter( kParam_One )*2.0;
	midGainA = midGainB; midGainB = GetParameter( kParam_Two )*2.0;
	bassGainA = bassGainB; bassGainB = GetParameter( kParam_Three )*2.0;
	//simple three band to adjust
	double kalman = 1.0-pow(GetParameter( kParam_Four ),2);
	//crossover frequency between mid/bass
			
	while (nSampleFrames-- > 0) {
		double inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpd * 1.18e-17;
		double drySampleL = inputSampleL;
			
		double temp = (double)nSampleFrames/inFramesToProcess;
		double trebleGain = (trebleGainA*temp)+(trebleGainB*(1.0-temp));
		if (trebleGain > 1.0) trebleGain = pow(trebleGain,3.0+sqrt(overallscale));
		if (trebleGain < 1.0) trebleGain = 1.0-pow(1.0-trebleGain,2);
		
		double midGain = (midGainA*temp)+(midGainB*(1.0-temp));
		if (midGain > 1.0) midGain *= midGain;
		if (midGain < 1.0) midGain = 1.0-pow(1.0-midGain,2);
		
		double bassGain = (bassGainA*temp)+(bassGainB*(1.0-temp));
		if (bassGain > 1.0) bassGain *= bassGain;
		if (bassGain < 1.0) bassGain = 1.0-pow(1.0-bassGain,2);
				
		//begin Air3L
		air[pvSL4] = air[pvAL4] - air[pvAL3]; air[pvSL3] = air[pvAL3] - air[pvAL2];
		air[pvSL2] = air[pvAL2] - air[pvAL1]; air[pvSL1] = air[pvAL1] - inputSampleL;
		air[accSL3] = air[pvSL4] - air[pvSL3]; air[accSL2] = air[pvSL3] - air[pvSL2];
		air[accSL1] = air[pvSL2] - air[pvSL1];
		air[acc2SL2] = air[accSL3] - air[accSL2]; air[acc2SL1] = air[accSL2] - air[accSL1];		
		air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1])*0.5));
		air[gainAL] *= 0.5; air[gainAL] += fabs(drySampleL-air[outAL])*0.5;
		if (air[gainAL] > 0.3*sqrt(overallscale)) air[gainAL] = 0.3*sqrt(overallscale);
		air[pvAL4] = air[pvAL3]; air[pvAL3] = air[pvAL2];
		air[pvAL2] = air[pvAL1]; air[pvAL1] = (air[gainAL] * air[outAL]) + drySampleL;
		double midL = drySampleL - ((air[outAL]*0.5)+(drySampleL*(0.457-(0.017*overallscale))));
		temp = (midL + air[gndavgL])*0.5; air[gndavgL] = midL; midL = temp;
		double trebleL = drySampleL-midL;
		inputSampleL = midL;
		//end Air3L
				
		//begin KalmanL
		temp = inputSampleL = inputSampleL*(1.0-kalman)*0.777;
		inputSampleL *= (1.0-kalman);
		//set up gain levels to control the beast
		kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2]; kal[prevSlewL3] *= 0.5;
		kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1]; kal[prevSlewL2] *= 0.5;
		kal[prevSlewL1] += kal[prevSampL1] - inputSampleL; kal[prevSlewL1] *= 0.5;
		//make slews from each set of samples used
		kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2]; kal[accSlewL2] *= 0.5;
		kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1]; kal[accSlewL1] *= 0.5;
		//differences between slews: rate of change of rate of change
		kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]); kal[accSlewL3] *= 0.5;
		//entering the abyss, what even is this
		kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3]; kal[kalOutL] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		kal[kalGainL] += fabs(temp-kal[kalOutL])*kalman*8.0; kal[kalGainL] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (kal[kalGainL] > kalman*0.5) kal[kalGainL] = kalman*0.5;
		//attempts to avoid explosions
		kal[kalOutL] += (temp*(1.0-(0.68+(kalman*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		kal[prevSampL3] = kal[prevSampL2]; kal[prevSampL2] = kal[prevSampL1];
		kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0-kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (kal[prevSampL1] > 1.0) kal[prevSampL1] = 1.0; if (kal[prevSampL1] < -1.0) kal[prevSampL1] = -1.0;
		double bassL = kal[kalOutL]*0.777;
		midL -= bassL;
		//end KalmanL
						
		inputSampleL = (bassL*bassGain) + (midL*midGain) + (trebleL*trebleGain);
				
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSampleL += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < air_total; x++) air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) kal[x] = 0.0;
	trebleGainA = 1.0; trebleGainB = 1.0;
	midGainA = 1.0; midGainB = 1.0;
	bassGainA = 1.0; bassGainB = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
