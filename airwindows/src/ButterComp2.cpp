#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ButterComp2"
#define AIRWINDOWS_DESCRIPTION "Improved ButterComp with an output control and sound upgrades."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','u','u' )
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
{ .name = "Compress", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		double controlApos;
		double controlAneg;
		double controlBpos;
		double controlBneg;
		double targetpos;
		double targetneg;
		double lastOutput;
		bool flip;
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
	Float64 overallscale = 2.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();

	Float64 inputgain = pow(10.0,(GetParameter( kParam_One )*14.0)/20.0);
	Float64 compfactor = 0.012 * (GetParameter( kParam_One ) / 135.0);
	Float64 output = GetParameter( kParam_Two );
	Float64 wet = GetParameter( kParam_Three );
	//removed unnecessary dry variable
	Float64 outputgain = inputgain;
	outputgain -= 1.0;
	outputgain /= 1.5;
	outputgain += 1.0;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		inputSample *= inputgain;
		
		double divisor = compfactor / (1.0+fabs(lastOutput));
		//this is slowing compressor recovery while output waveforms were high
		divisor /= overallscale;
		double remainder = divisor;
		divisor = 1.0 - divisor;
		//recalculate divisor every sample
		
		double inputpos = inputSample + 1.0;
		if (inputpos < 0.0) inputpos = 0.0;
		double outputpos = inputpos / 2.0;
		if (outputpos > 1.0) outputpos = 1.0;		
		inputpos *= inputpos;
		targetpos *= divisor;
		targetpos += (inputpos * remainder);
		double calcpos = pow((1.0/targetpos),2);
		
		double inputneg = (-inputSample) + 1.0;
		if (inputneg < 0.0) inputneg = 0.0;
		double outputneg = inputneg / 2.0;
		if (outputneg > 1.0) outputneg = 1.0;		
		inputneg *= inputneg;
		targetneg *= divisor;
		targetneg += (inputneg * remainder);
		double calcneg = pow((1.0/targetneg),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		
		if (inputSample > 0)
		{ //working on pos
			if (flip)
			{
				controlApos *= divisor;
				controlApos += (calcpos*remainder);
				
			}
			else
			{
				controlBpos *= divisor;
				controlBpos += (calcpos*remainder);
			}
		}
		else
		{ //working on neg
			if (flip)
			{
				controlAneg *= divisor;
				controlAneg += (calcneg*remainder);
			}
			else
			{
				controlBneg *= divisor;
				controlBneg += (calcneg*remainder);
			}
		}
		//this causes each of the four to update only when active and in the correct 'flip'
		
		double totalmultiplier;
		if (flip)
		{totalmultiplier = (controlApos * outputpos) + (controlAneg * outputneg);}
		else
		{totalmultiplier = (controlBpos * outputpos) + (controlBneg * outputneg);}
		//this combines the sides according to flip, blending relative to the input value
		
		inputSample *= totalmultiplier;
		inputSample /= outputgain;
		
		if (output != 1.0) {
			inputSample *= output;
		}
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		lastOutput = inputSample;
		//we will make this factor respond to use of dry/wet
		
		flip = !flip;

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
	controlApos = 1.0;
	controlAneg = 1.0;
	controlBpos = 1.0;
	controlBneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;
	flip = false;
	lastOutput = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
