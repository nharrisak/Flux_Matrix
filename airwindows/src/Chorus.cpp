#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Chorus"
#define AIRWINDOWS_DESCRIPTION "A mono chorus, also works as a vibrato."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		const static int totalsamples = 16386;
		Float64 sweep;
		int gcount;
		Float64 airPrev;
		Float64 airEven;
		Float64 airOdd;
		Float64 airFactor;
		uint32_t fpd;
		bool fpFlip;
	
	struct _dram {
			Float32 d[totalsamples];
	};
	_dram* dram;
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();

	Float64 speed = pow(GetParameter( kParam_One ),4) * 0.001;
	speed *= overallscale;
	int loopLimit = (int)(totalsamples * 0.499);
	int count;
	Float64 range = pow(GetParameter( kParam_Two ),4) * loopLimit * 0.499;
	Float64 wet = GetParameter( kParam_Three );
	Float64 modulation = range*wet;
	//removed unnecessary dry variable
	Float64 tupi = 3.141592653589793238 * 2.0;
	Float64 offset;
	//this is a double buffer so we will be splitting it in two
	
	double inputSample;
	Float64 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		airFactor = airPrev - inputSample;
		if (fpFlip) {airEven += airFactor; airOdd -= airFactor; airFactor = airEven;}
		else {airOdd += airFactor; airEven -= airFactor; airFactor = airOdd;}
		airOdd = (airOdd - ((airOdd - airEven)/256.0)) / 1.0001;
		airEven = (airEven - ((airEven - airOdd)/256.0)) / 1.0001;
		airPrev = inputSample;
		inputSample += (airFactor*wet);
		//air, compensates for loss of highs in flanger's interpolation
		
		if (gcount < 1 || gcount > loopLimit) {gcount = loopLimit;}
		count = gcount;
		dram->d[count+loopLimit] = dram->d[count] = inputSample;
		gcount--;
		//double buffer
		
		offset = range + (modulation * sin(sweep));
		count += (int)floor(offset);
		inputSample = dram->d[count] * (1-(offset-floor(offset))); //less as value moves away from .0
		inputSample += dram->d[count+1]; //we can assume always using this in one way or another?
		inputSample += (dram->d[count+2] * (offset-floor(offset))); //greater as value moves away from .0
		inputSample -= (((dram->d[count]-dram->d[count+1])-(dram->d[count+1]-dram->d[count+2]))/50); //interpolation hacks 'r us
		inputSample *= 0.5; //to get a comparable level
		//sliding
		
		sweep += speed;
		if (sweep > tupi){sweep -= tupi;}
		//still scrolling through the samples, remember
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		destP += inNumChannels;
		sourceP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < totalsamples-1; count++) {dram->d[count] = 0;}
	sweep = 3.141592653589793238 / 2.0;
	gcount = 0;
	airPrev = 0.0;
	airEven = 0.0;
	airOdd = 0.0;
	airFactor = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = false;
}
};
