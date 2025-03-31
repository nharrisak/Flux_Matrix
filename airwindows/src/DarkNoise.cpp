#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DarkNoise"
#define AIRWINDOWS_DESCRIPTION "An alternative method for producing bassier noise directly. Sound design stuff."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','a','s' )
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dark", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float64 b[11][11];
		Float64 f[11];		
		int freq;
		Float64 lastRandy;
		Float64 outputNoise;
		uint32_t fpd;
	
	struct _dram {
			Float64 r[8193];
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

	int freqTarget = (int)(pow(GetParameter( kParam_One ),3)*8192.0);
	if (freqTarget < 2) freqTarget = 2;
	Float64 volumeScale = (1.0/freqTarget) * sqrt(freqTarget);
	
	Float64 overalltaps = (pow(GetParameter( kParam_One ),2)*8.0)+2.0;
	Float64 taps = overalltaps;
	//this is our averaging, which is not integer but continuous
	
	Float64 overallpoles = (GetParameter( kParam_One )*9.0)+1.0;
	//this is the poles of the filter, also not integer but continuous
	int yLimit = floor(overallpoles)+1;
	Float64 yPartial = overallpoles - floor(overallpoles);
	//now we can do a for loop, and also apply the final pole continuously
	int xLimit = 1;
	for(int x = 0; x < 11; x++) {
		if (taps > 1.0) {
			f[x] = 1.0;
			taps -= 1.0;
			xLimit++;
		} else {
			f[x] = taps;
			taps = 0.0;
		}
	} //there, now we have a neat little moving average with remainders
	if (xLimit > 9) xLimit = 9;
	
	if (overalltaps < 1.0) overalltaps = 1.0;
	for(int x = 0; x < xLimit; x++) {
		f[x] /= overalltaps;
	} //and now it's neatly scaled, too	
	
	Float64 dark  = GetParameter( kParam_Two );
	Float64 out  = GetParameter( kParam_Three )*0.5;
	Float64 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		if (freqTarget < freq) {
			outputNoise = ((outputNoise-0.5)*(1.0-(1.0/freq)))+0.5;
			dram->r[freq] = 0.5;
			freq--;
		}
		if (freqTarget > freq) freq++;
		//our tone control chases the input one bin at a time
		
		Float64 randy = (Float64)fpd / UINT32_MAX;
		// 0 to 1 random value
		
		int replacementBin = randy * freq;
		outputNoise -= dram->r[replacementBin];
		dram->r[replacementBin] = lastRandy;
		outputNoise += lastRandy;
		lastRandy = randy;
		//we update only one of the slots we're using
		
		inputSample = (-0.5 + outputNoise) * volumeScale;
		
		Float64 nondarkSample = inputSample;
		
		double previousPole = 0;		
		for (int y = 0; y < yLimit; y++) {
			for (int x = xLimit; x >= 0; x--) b[x+1][y] = b[x][y];
			b[0][y] = previousPole = inputSample;
			inputSample = 0.0;
			for (int x = 0; x < xLimit; x++) inputSample += (b[x][y] * f[x]);
		}
		inputSample = (previousPole * (1.0-yPartial)) + (inputSample * yPartial);
		//in this way we can blend in the final pole
		
		
		if (dark !=1.0) {
			inputSample = (inputSample * dark) + (nondarkSample * (1.0-dark));
		}
		if (out !=1.0) {
			inputSample *= out;
		}
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
	for(int count = 0; count < 8192; count++) dram->r[count] = 0.5;
	for(int x = 0; x < 11; x++) {
		f[x] = 0.0;
		for (int y = 0; y < 11; y++) b[x][y] = 0.0;
	}
	
	freq = 1;
	lastRandy = 0.0;
	outputNoise = 0.5;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
