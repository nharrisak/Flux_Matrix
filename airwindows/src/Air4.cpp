#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Air4"
#define AIRWINDOWS_DESCRIPTION "Extends Air3 with controllable high frequency limiting."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','i','u' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
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
{ .name = "Gnd", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DarkF", .min = 0, .max = 1000, .def = 520, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
			lastSL,
			air_total
		};
		double air[air_total];
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
	
	double airGain = GetParameter( kParam_A )*2.0;
	if (airGain > 1.0) airGain = pow(airGain,3.0+sqrt(overallscale));
	double gndGain = GetParameter( kParam_B )*2.0;
	double threshSinew = pow(GetParameter( kParam_C ),2)/overallscale;
	double depthSinew = GetParameter( kParam_D );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		air[pvSL4] = air[pvAL4] - air[pvAL3];
		air[pvSL3] = air[pvAL3] - air[pvAL2];
		air[pvSL2] = air[pvAL2] - air[pvAL1];
		air[pvSL1] = air[pvAL1] - inputSample;
		
		air[accSL3] = air[pvSL4] - air[pvSL3];
		air[accSL2] = air[pvSL3] - air[pvSL2];
		air[accSL1] = air[pvSL2] - air[pvSL1];
		
		air[acc2SL2] = air[accSL3] - air[accSL2];
		air[acc2SL1] = air[accSL2] - air[accSL1];		
		
		air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1])*0.5));
		
		air[gainAL] *= 0.5; 
		air[gainAL] += fabs(drySample-air[outAL])*0.5;
		if (air[gainAL] > 0.3*sqrt(overallscale)) air[gainAL] = 0.3*sqrt(overallscale);
		air[pvAL4] = air[pvAL3];
		air[pvAL3] = air[pvAL2];
		air[pvAL2] = air[pvAL1];		
		air[pvAL1] = (air[gainAL] * air[outAL]) + drySample;
		
		double gnd = drySample - ((air[outAL]*0.5)+(drySample*(0.457-(0.017*overallscale))));
		double temp = (gnd + air[gndavgL])*0.5; air[gndavgL] = gnd; gnd = temp;
		inputSample = ((drySample-gnd)*airGain)+(gnd*gndGain);
		
		temp = inputSample; if (temp > 1.0) temp = 1.0; if (temp < -1.0) temp = -1.0;
		double sinew = threshSinew * cos(air[lastSL]*air[lastSL]);
		if (temp - air[lastSL] > sinew) temp = air[lastSL] + sinew;
		if (-(temp - air[lastSL]) > sinew) temp = air[lastSL] - sinew;
		air[lastSL] = temp;
		if (air[lastSL] > 1.0) air[lastSL] = 1.0; 
		if (air[lastSL] < -1.0) air[lastSL] = -1.0; 
		inputSample = (inputSample * (1.0-depthSinew))+(air[lastSL]*depthSinew);
		//run Sinew to stop excess slews, but run a dry/wet to allow a range of brights		
		
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
	for (int x = 0; x < air_total; x++) air[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
