#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Tremolo"
#define AIRWINDOWS_DESCRIPTION "Fluctuating saturation curves for a tubey tremolo."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','r','f' )
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
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 speedChase;
		Float64 depthChase;
		Float64 speedAmount;
		Float64 depthAmount;
		Float64 lastSpeed;
		Float64 lastDepth;
		Float64 sweep;
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
	
	speedChase = pow(GetParameter( kParam_One ),4);
	depthChase = GetParameter( kParam_Two );
	Float64 speedSpeed = 300 / (fabs( lastSpeed - speedChase)+1.0);
	Float64 depthSpeed = 300 / (fabs( lastDepth - depthChase)+1.0);
	lastSpeed = speedChase;
	lastDepth = depthChase;

	Float64 speed;
	Float64 depth;
	Float64 skew;
	Float64 density;
	
	Float64 tupi = 3.141592653589793238;
	double inputSample;
	double drySample;
	Float64 control;
	Float64 tempcontrol;
	Float64 thickness;
	Float64 out;
	Float64 bridgerectifier;
	Float64 offset;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		speedAmount = (((speedAmount*speedSpeed)+speedChase)/(speedSpeed + 1.0));
		depthAmount = (((depthAmount*depthSpeed)+depthChase)/(depthSpeed + 1.0));
		speed = 0.0001+(speedAmount/1000.0);
		speed /= overallscale;
		depth = 1.0 - pow(1.0-depthAmount,5);
		skew = 1.0+pow(depthAmount,9);
		density = ((1.0-depthAmount)*2.0) - 1.0;		
		
		offset = sin(sweep);
		sweep += speed;
		if (sweep > tupi){sweep -= tupi;}
		control = fabs(offset);
		if (density > 0)
			{
			tempcontrol = sin(control);
			control = (control * (1.0-density))+(tempcontrol * density);
			}
		else
			{
			tempcontrol = 1-cos(control);
			control = (control * (1.0+density))+(tempcontrol * -density);
			}
		//produce either boosted or starved version of control signal
		//will go from 0 to 1
		
		thickness = ((control * 2.0) - 1.0)*skew;
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out);
		//blend according to density control
		inputSample *= (1.0 - control);
		inputSample *= 2.0;
		//apply tremolo, apply gain boost to compensate for volume loss
		inputSample = (drySample * (1-depth)) + (inputSample*depth);

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
	sweep = 3.141592653589793238 / 2.0;
	speedChase = 0.0;
	depthChase = 0.0;
	speedAmount = 1.0;
	depthAmount = 0.0;
	lastSpeed = 1000.0;
	lastDepth = 1000.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
