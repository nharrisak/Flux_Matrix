#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "HardVacuum"
#define AIRWINDOWS_DESCRIPTION "Tube style saturation effects."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','a','r' )
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
{ .name = "Drive", .min = 0, .max = 2000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Warmth", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Aura", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 lastSample;
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
	Float64 multistage = GetParameter( kParam_One );
	if (multistage > 1) multistage *= multistage;
	//WE MAKE LOUD NOISE! RAWWWK!
	Float64 countdown;
	Float64 warmth = GetParameter( kParam_Two );
	Float64 invwarmth = 1.0-warmth;
	warmth /= 1.57079633;
	Float64 aura = GetParameter( kParam_Three )*3.1415926;
	Float64 out = GetParameter( kParam_Four );
	Float64 wet = GetParameter( kParam_Five );
	Float64 dry = 1.0-wet;
	Float64 drive;
	Float64 positive;
	Float64 negative;
	Float64 bridgerectifier;
	Float64 skew;
	double inputSample;
	Float64 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;		
		
		skew = (inputSample - lastSample);
		lastSample = inputSample;
		//skew will be direction/angle
		bridgerectifier = fabs(skew);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skew > 0) skew = bridgerectifier*aura;
		else skew = -bridgerectifier*aura;
		//skew is now sined and clamped and then re-amplified again
		skew *= inputSample;
		//cools off sparkliness and crossover distortion
		skew *= 1.557079633;
		//crank up the gain on this so we can make it sing
		//We're doing all this here so skew isn't incremented by each stage

		countdown = multistage;
		//begin the torture

		while (countdown > 0)
			{
				if (countdown > 1.0) drive = 1.557079633;
				else drive = countdown * (1.0+(0.557079633*invwarmth));
				//full crank stages followed by the proportional one
				//whee. 1 at full warmth to 1.5570etc at no warmth
				positive = drive - warmth;
				negative = drive + warmth;
				//set up things so we can do repeated iterations, assuming that
				//wet is always going to be 0-1 as in the previous plug.
				bridgerectifier = fabs(inputSample);
				bridgerectifier += skew;
				//apply it here so we don't overload
				if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
				bridgerectifier = sin(bridgerectifier);
				//the distortion section.
				bridgerectifier *= drive;
				bridgerectifier += skew;
				//again
				if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
				bridgerectifier = sin(bridgerectifier);
				if (inputSample > 0)
					{
					inputSample = (inputSample*(1-positive+skew))+(bridgerectifier*(positive+skew));
					}
				else
					{
					inputSample = (inputSample*(1-negative+skew))-(bridgerectifier*(negative+skew));
					}
				//blend according to positive and negative controls
				countdown -= 1.0;
				//step down a notch and repeat.
			}
		
		if (out !=1.0) {
			inputSample *= out;
		}		

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
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
