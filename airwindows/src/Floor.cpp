#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Floor"
#define AIRWINDOWS_DESCRIPTION "Fake bottom octave for fun and profit!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','l','o' )
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
{ .name = "Floor", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		bool flip; //end defining of antialiasing variables
		Float64 iirSample1A;
		Float64 iirSample1B;
		Float64 iirSample1C;
		Float64 iirSample1D;
		Float64 iirSample1E;
		Float64 iirSample2A;
		Float64 iirSample2B;
		Float64 iirSample2C;
		Float64 iirSample2D;
		Float64 iirSample2E;
		uint32_t fpd;
	
	struct _dram {
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

	Float64 setting = pow(GetParameter( kParam_One ),2);
	Float64 iirAmount = (setting/4.0)/overallscale;
	Float64 tight = -1.0;
	Float64 gaintrim = 1.0 + (setting/4.0);
	Float64 offset;
	Float64 lows;
	Float64 density = GetParameter( kParam_Two );
	Float64 bridgerectifier;
	Float64 temp;
	iirAmount += (iirAmount * tight * tight);
	tight /=  3.0;
	if (iirAmount <= 0.0) iirAmount = 0.0;
	if (iirAmount > 1.0) iirAmount = 1.0;
	Float64 wet = GetParameter( kParam_Three );
	Float64 dry = 1.0-wet;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		
		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		iirSample1A = (iirSample1A * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
		lows = iirSample1A;
		
		inputSample -= lows;
		
		temp = lows;
		if (lows < 0) {lows = -sin(-lows*1.5707963267949);}
		if (lows > 0) {lows = sin(lows*1.5707963267949);}
		lows -= temp;
		
		inputSample += lows;
		inputSample *= gaintrim;
		
		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		
		iirSample1B = (iirSample1B * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
		lows = iirSample1B;
		
		inputSample -= lows;
		
		temp = lows;
		if (lows < 0) {lows = -sin(-lows*1.5707963267949);}
		if (lows > 0) {lows = sin(lows*1.5707963267949);}
		lows -= temp;
		
		inputSample += lows;
		inputSample *= gaintrim;
		
		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		
		iirSample1C = (iirSample1C * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
		lows = iirSample1C;
		
		inputSample -= lows;
		
		temp = lows;
		if (lows < 0) {lows = -sin(-lows*1.5707963267949);}
		if (lows > 0) {lows = sin(lows*1.5707963267949);}
		lows -= temp;
		
		inputSample += lows;
		inputSample *= gaintrim;
		
		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		
		iirSample1D = (iirSample1D * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
		lows = iirSample1D;
		
		inputSample -= lows;
		
		temp = lows;
		if (lows < 0) {lows = -sin(-lows*1.5707963267949);}
		if (lows > 0) {lows = sin(lows*1.5707963267949);}
		lows -= temp;
		
		inputSample += lows;
		inputSample *= gaintrim;

		if (tight > 0) offset = (1 - tight) + (fabs(inputSample)*tight);
		else offset = (1 + tight) + ((1-fabs(inputSample))*tight);
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		
		iirSample1E = (iirSample1E * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
		lows = iirSample1E;
		
		inputSample -= lows;
		
		temp = lows;
		if (lows < 0) {lows = -sin(-lows*1.5707963267949);}
		if (lows > 0) {lows = sin(lows*1.5707963267949);}
		lows -= temp;
		
		inputSample += lows;
		inputSample *= gaintrim;

		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		bridgerectifier = fabs(inputSample)*1.57079633;
		bridgerectifier = sin(bridgerectifier)*1.57079633;
		bridgerectifier = (fabs(inputSample)*(1-density))+(bridgerectifier*density);
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = (inputSample*(1-density))+(bridgerectifier*density);
		else inputSample = (inputSample*(1-density))-(bridgerectifier*density);
		//drive section
		
		if (wet < 1.0) inputSample = (drySample * dry)+(inputSample*wet);

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
	flip = false;
	iirSample1A = 0.0;
	iirSample1B = 0.0;
	iirSample1C = 0.0;
	iirSample1D = 0.0;
	iirSample1E = 0.0;
	iirSample2A = 0.0;
	iirSample2B = 0.0;
	iirSample2C = 0.0;
	iirSample2D = 0.0;
	iirSample2E = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
