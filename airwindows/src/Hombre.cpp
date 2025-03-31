#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hombre"
#define AIRWINDOWS_DESCRIPTION "Atmosphere and texture (through very short delays)."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','o','n' )
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
{ .name = "Voicing", .min = 0, .max = 1000, .def = 421, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Intensity", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 p[4001];
		Float64 slide;
		int gcount;
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
	Float64 target = GetParameter( kParam_One );
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 offsetA;
	Float64 offsetB;
	int widthA = (int)(1.0*overallscale);
	int widthB = (int)(7.0*overallscale); //max 364 at 44.1, 792 at 96K
	Float64 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	double inputSample;
	Float64 drySample;
	Float64 total;
	int count;	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;

		slide = (slide * 0.9997)+(target*0.0003);

		offsetA = ((pow(slide,2)) * 77)+3.2;
		offsetB = (3.85 * offsetA)+41;
		offsetA *= overallscale;
		offsetB *= overallscale;
		//adjust for sample rate

		if (gcount < 1 || gcount > 2000) {gcount = 2000;}
		count = gcount;

		p[count+2000] = p[count] = inputSample;
		//double buffer
		
		count = (int)(gcount+floor(offsetA));

		total = p[count] * 0.391; //less as value moves away from .0
		total += p[count+widthA]; //we can assume always using this in one way or another?
		total += p[count+widthA+widthA] * 0.391; //greater as value moves away from .0

		inputSample += ((total * 0.274));

		count = (int)(gcount+floor(offsetB));

		total = p[count] * 0.918; //less as value moves away from .0
		total += p[count+widthB]; //we can assume always using this in one way or another?
		total += p[count+widthB+widthB] * 0.918; //greater as value moves away from .0

		inputSample -= ((total * 0.629));

		inputSample /= 4;
		
		gcount--;
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
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 4000; count++) {p[count] = 0.0;}
	gcount = 0;
	slide = 0.421;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
