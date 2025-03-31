#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hull"
#define AIRWINDOWS_DESCRIPTION "An alternate form of highpass/lowpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','u','l' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Freq", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bright", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		int bPointer;
		int cPointer;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		double b[4005];	
		double c[105];
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 96000.0;
	overallscale *= GetSampleRate(); //this one's scaled to 96k for the deepest bass
	if (overallscale > 1.0) overallscale = 1.0; //and if you go for 192k, rather than crash
	//it just cuts out the maximum (2000) depth of averaging you can get
	Float64 hullSetting = pow(GetParameter( kParam_One ),3)*overallscale;
	int limitA = (hullSetting*2000.0)+1.0;
	double divisorA = 1.0/limitA;
	int limitB = (hullSetting*1000.0)+1.0;
	double divisorB = 1.0/limitB;
	int limitC = sqrt(hullSetting*2000.0)+1.0;
	double divisorC = 1.0/limitC;
	Float64 wet = -GetParameter( kParam_Two ); //functions as dark/bright
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		bPointer--; if (bPointer < 0) bPointer += 2000;
		dram->b[bPointer] = dram->b[bPointer+2000] = inputSample;

		int x = bPointer;
		double longAverage = 0.0;
		double shortAverage = 0.0;
		while (x < bPointer+limitB) {
			shortAverage += dram->b[x];
			longAverage += dram->b[x];
			x++;
		} //once shorter average is reached, continue to longer
		while (x < bPointer+limitA) {
			longAverage += dram->b[x];
			x++;
		}
		longAverage *= divisorA;
		shortAverage *= divisorB;
		
		
		cPointer--; if (cPointer < 0) cPointer += 50;
		dram->c[cPointer] = dram->c[cPointer+50] = shortAverage+(shortAverage-longAverage);
		
		x = cPointer;
		double shortestAverage = 0.0;
		while (x < cPointer+limitC) {
			shortestAverage += dram->c[x];
			x++;
		}
		shortestAverage *= divisorC;
				
		if (wet > 0.0) {
			inputSample = (shortestAverage * wet)+(drySample * (1.0-wet)); //dark
		} else {
			inputSample = ((inputSample-shortestAverage) * fabs(wet))+(drySample * (1.0+wet)); //bright
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
	for(int count = 0; count < 4002; count++) dram->b[count] = 0.0;
	for(int count = 0; count < 102; count++) dram->c[count] = 0.0;
	bPointer = 1; cPointer = 1;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
