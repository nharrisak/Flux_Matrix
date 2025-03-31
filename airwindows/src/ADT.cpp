#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ADT"
#define AIRWINDOWS_DESCRIPTION "A double short delay tap with saturation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','D','T' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Headroom", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "A Delay Time", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "A Level", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "B Delay Time", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "B Level", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 p[10000];
		int gcount;
		Float64 offsetA;
		Float64 offsetB;
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
	
	Float64 gain = GetParameter( kParam_One ) * 0.636;
	Float64 targetA = pow(GetParameter( kParam_Two ),4) * 4790.0;
	Float64 fractionA;
	Float64 minusA;
	Float64 intensityA = GetParameter( kParam_Three ) / 2.0;
	//first delay
	Float64 targetB = (pow(GetParameter( kParam_Four ),4) * 4790.0);
	Float64 fractionB;
	Float64 minusB;
	Float64 intensityB = GetParameter( kParam_Five ) / 2.0;
	//second delay
	Float64 output = GetParameter( kParam_Six );
	Float64 total;
	int count;
	Float64 temp;
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		
		if (fabs(offsetA - targetA) > 1000) offsetA = targetA;
		offsetA = ((offsetA*999.0)+targetA)/1000.0;
		fractionA = offsetA - floor(offsetA);
		minusA = 1.0 - fractionA;
		
		if (fabs(offsetB - targetB) > 1000) offsetB = targetB;
		offsetB = ((offsetB*999.0)+targetB)/1000.0;
		fractionB = offsetB - floor(offsetB);
		minusB = 1.0 - fractionB;
		//chase delay taps for smooth action
		
		if (gain > 0) inputSample /= gain;
		
		if (inputSample > 1.2533141373155) inputSample = 1.2533141373155;
		if (inputSample < -1.2533141373155) inputSample = -1.2533141373155;
		inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0) ?1:fabs(inputSample));
		//Spiral: lean out the sound a little when decoded by ConsoleBuss
		
		if (gcount < 1 || gcount > 4800) {gcount = 4800;}
		count = gcount;
		total = 0.0;
		p[count+4800] = p[count] = inputSample;
		//double buffer
		
		if (intensityA != 0.0)
		{
			count = (int)(gcount+floor(offsetA));
			temp = (p[count] * minusA); //less as value moves away from .0
			temp += p[count+1]; //we can assume always using this in one way or another?
			temp += (p[count+2] * fractionA); //greater as value moves away from .0
			temp -= (((p[count]-p[count+1])-(p[count+1]-p[count+2]))/50); //interpolation hacks 'r us
			total += (temp * intensityA);
		}
		
		if (intensityB != 0.0)
		{
			count = (int)(gcount+floor(offsetB));
			temp = (p[count] * minusB); //less as value moves away from .0
			temp += p[count+1]; //we can assume always using this in one way or another?
			temp += (p[count+2] * fractionB); //greater as value moves away from .0
			temp -= (((p[count]-p[count+1])-(p[count+1]-p[count+2]))/50); //interpolation hacks 'r us
			total += (temp * intensityB);
		}
		
		gcount--;
		//still scrolling through the samples, remember
		
		inputSample += total;
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		
		inputSample = asin(inputSample);
		//amplitude aspect
		
		inputSample *= gain;
		
		if (output < 1.0) inputSample *= output;
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += static_cast<int32_t>(fpd) * 5.960464655174751e-36L * pow(2,expon+62);
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 9999; count++) {p[count] = 0;}
	offsetA = 9001;
	offsetB = 9001;  //  :D
	gcount = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
