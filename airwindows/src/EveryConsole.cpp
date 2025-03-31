#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "EveryConsole"
#define AIRWINDOWS_DESCRIPTION "Mix-and-match Airwindows Console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','v','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
static const int kRC = 1;
static const int kRB = 2;
static const int kSC = 3;
static const int kSB = 4;
static const int k6C = 5;
static const int k6B = 6;
static const int k7C = 7;
static const int k7B = 8;
static const int kBC = 9;
static const int kBB = 10;
static const int kZC = 11;
static const int kZB = 12;
static const int kDefaultValue_ParamOne = kSC;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static char const * const enumStrings0[] = { "", "Retro Channel", "Retro Buss", "Sin() Channel", "aSin() Buss", "C6 Channel", "C6 Buss", "C7 Channel", "C7 Buss", "BShifty Channel", "BShifty Buss", "CZero Channel", "CZero Buss", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Console Type", .min = 1, .max = 12, .def = 3, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "In Trim", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Out Trim", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	
	double half = 0.0;
	double falf = 0.0;
	int console = (int) GetParameter( kParam_One );
	Float64 inTrim = GetParameter( kParam_Two ); //0-2
	Float64 outTrim = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (inTrim != 1.0) {
			inputSample *= inTrim;
		}

		switch (console)
		{
			case kRC:
				half = inputSample * 0.83;
				falf = fabs(half);
				half *= falf;
				half *= falf;
				inputSample -= half;
				break; //original ConsoleChannel, before sin/asin
				
			case kRB:
				half = inputSample * 0.885;
				falf = fabs(half);
				half *= falf;
				half *= falf;
				inputSample += half;
				break; //original ConsoleBuss, before sin/asin
				
			case kSC:
				if (inputSample > M_PI_2) inputSample = M_PI_2;
				if (inputSample < -M_PI_2) inputSample = -M_PI_2;
				//clip to max sine without any wavefolding
				inputSample = sin(inputSample);
				break; //sin() function ConsoleChannel
				
			case kSB:
				if (inputSample > 1.0) inputSample = 1.0;
				if (inputSample < -1.0) inputSample = -1.0;
				//without this, you can get a NaN condition where it spits out DC offset at full blast!
				inputSample = asin(inputSample);
				break; //sin() function ConsoleBuss
				
			case k6C:
				//encode/decode courtesy of torridgristle under the MIT license
				if (inputSample > 1.0) inputSample= 1.0;
				else if (inputSample > 0.0) inputSample = 1.0 - pow(1.0-inputSample,2.0);
				if (inputSample < -1.0) inputSample = -1.0;
				else if (inputSample < 0.0) inputSample = -1.0 + pow(1.0+inputSample,2.0);
				//Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5 for Console6Channel
				break; //crude sine. Note that because modern processors love math more than extra variables, this is optimized
				
			case k6B:
				//encode/decode courtesy of torridgristle under the MIT license
				if (inputSample > 1.0) inputSample= 1.0;
				else if (inputSample > 0.0) inputSample = 1.0 - pow(1.0-inputSample,0.5);
				if (inputSample < -1.0) inputSample = -1.0;
				else if (inputSample < 0.0) inputSample = -1.0 + pow(1.0+inputSample,0.5);
				//Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5 for Console6Buss
				break; //crude arcsine. Note that because modern processors love math more than extra variables, this is optimized
				
			case k7C:
				if (inputSample > 1.097) inputSample = 1.097;
				if (inputSample < -1.097) inputSample = -1.097;
				inputSample = ((sin(inputSample*fabs(inputSample))/((fabs(inputSample) == 0.0) ?1:fabs(inputSample)))*0.8)+(sin(inputSample)*0.2);
				//this is a version of Spiral blended 80/20 with regular Density.
				//It's blending between two different harmonics in the overtones of the algorithm
				break; //Console7Channel
				
			case k7B:
				if (inputSample > 1.0) inputSample = 1.0;
				if (inputSample < -1.0) inputSample = -1.0;
				inputSample = ((asin(inputSample*fabs(inputSample))/((fabs(inputSample) == 0.0) ?1:fabs(inputSample)))*0.618033988749894848204586)+(asin(inputSample)*0.381966011250105);
				//this is an asin version of Spiral blended with regular asin ConsoleBuss.
				//It's blending between two different harmonics in the overtones of the algorithm.
				break; //Console7Buss
				
			case kBC:
				inputSample += ((pow(inputSample,5)/128.0) + (pow(inputSample,9)/262144.0)) - ((pow(inputSample,3)/8.0) + (pow(inputSample,7)/4096.0));
				break; //crude sine. Note that because modern processors love math more than extra variables, this is optimized
				
			case kBB:
				inputSample += (pow(inputSample,3)/4.0)+(pow(inputSample,5)/8.0)+(pow(inputSample,7)/16.0)+(pow(inputSample,9)/32.0);
				break; //crude arcsine. Note that because modern processors love math more than extra variables, this is optimized
				
			case kZC:
				if (inputSample > 1.4137166941154) inputSample = 1.4137166941154;
				if (inputSample < -1.4137166941154) inputSample = -1.4137166941154;
				if (inputSample > 0.0) inputSample = (inputSample/2.0)*(2.8274333882308-inputSample);
				else inputSample = -(inputSample/-2.0)*(2.8274333882308+inputSample);
				break; //ConsoleZeroChannel
				
			case kZB:
				if (inputSample > 2.8) inputSample = 2.8;
				if (inputSample < -2.8) inputSample = -2.8;
				if (inputSample > 0.0) inputSample = (inputSample*2.0)/(3.0-inputSample);
				else inputSample = -(inputSample*-2.0)/(3.0+inputSample);
				break; //ConsoleZeroBuss
		}
		
		if (outTrim != 1.0) {
			inputSample *= outTrim;
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
