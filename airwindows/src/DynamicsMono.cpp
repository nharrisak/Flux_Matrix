#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DynamicsMono"
#define AIRWINDOWS_DESCRIPTION "DynamicsMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','o' )
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
{ .name = "Comp", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		//begin Gate
		bool WasNegative;
		int ZeroCross;
		double gateroller;
		double gate;
		//end Gate
		
		//begin ButterComp
		double controlApos;
		double controlAneg;
		double controlBpos;
		double controlBneg;
		double targetpos;
		double targetneg;
		double avgA;
		double avgB;
		double nvgA;
		double nvgB;
		
		bool flip;
		//end ButterComp
		
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	//begin ButterComp
	double inputgain = (pow(GetParameter( kParam_One ),5)*35)+1.0;
	double divisor = (pow(GetParameter( kParam_Two ),4) * 0.01)+0.0005;
	divisor /= overallscale;
	double remainder = divisor;
	divisor = 1.0 - divisor;
	//end ButterComp
	
	//begin Gate
	double onthreshold = (pow(GetParameter( kParam_Three ),3)/3)+0.00018;
	double offthreshold = onthreshold * 1.1;	
	double release = 0.028331119964586;
	double absmax = 220.9;
	//speed to be compensated w.r.t sample rate
	//end Gate
	
	double wet = GetParameter(kParam_Four );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		//begin compressor
		inputSample *= inputgain;
		double inputpos = inputSample + 1.0;		
		if (inputpos < 0.0) inputpos = 0.0;
		double outputpos = inputpos / 2.0;
		if (outputpos > 1.0) outputpos = 1.0;		
		inputpos *= inputpos;
		targetpos *= divisor;
		targetpos += (inputpos * remainder);
		double calcpos = 1.0/targetpos;
		
		double inputneg = -inputSample + 1.0;		
		if (inputneg < 0.0) inputneg = 0.0;
		double outputneg = inputneg / 2.0;
		if (outputneg > 1.0) outputneg = 1.0;		
		inputneg *= inputneg;
		targetneg *= divisor;
		targetneg += (inputneg * remainder);
		double calcneg = 1.0/targetneg;
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		
		if (inputSample > 0)
		{ //working on pos
			if (true == flip)
			{
				controlApos *= divisor;
				controlApos += (calcpos*remainder);
			} else {
				controlBpos *= divisor;
				controlBpos += (calcpos*remainder);
			}	
		} else { //working on neg
			if (true == flip)
			{
				controlAneg *= divisor;
				controlAneg += (calcneg*remainder);
			} else {
				controlBneg *= divisor;
				controlBneg += (calcneg*remainder);
			}
		}
		//this causes each of the four to update only when active and in the correct 'flip'
		
		double totalmultiplier;
		if (true == flip) totalmultiplier = (controlApos * outputpos) + (controlAneg * outputneg);
		else totalmultiplier = (controlBpos * outputpos) + (controlBneg * outputneg);
		//this combines the sides according to flip, blending relative to the input value
		inputSample *= totalmultiplier;
		inputSample /= inputgain;
		
		flip = !flip;
		//end compressor
		
		//begin Gate
		if (drySample > 0.0)
		{
			if (WasNegative == true) ZeroCross = absmax * 0.3;
			WasNegative = false;
		} else {
			ZeroCross += 1; WasNegative = true;
		}
				
		if (ZeroCross > absmax) ZeroCross = absmax;
		
		if (gate == 0.0)
		{
			//if gate is totally silent
			if (fabs(drySample) > onthreshold)
			{
				if (gateroller == 0.0) gateroller = ZeroCross;
				else gateroller -= release;
				// trigger from total silence only- if we're active then signal must clear offthreshold
			}
			else gateroller -= release;
		} else {
			//gate is not silent but closing
			if (fabs(drySample) > offthreshold)
			{
				if (gateroller < ZeroCross) gateroller = ZeroCross;
				else gateroller -= release;
				//always trigger if gate is over offthreshold, otherwise close anyway
			}
			else gateroller -= release;
		}
				
		if (gateroller < 0.0) gateroller = 0.0;
		
		if (gateroller < 1.0)
		{
			gate = gateroller;
			double bridgerectifier = 1-cos(fabs(inputSample));			
			if (inputSample > 0) inputSample = (inputSample*gate)+(bridgerectifier*(1.0-gate));
			else inputSample = (inputSample*gate)-(bridgerectifier*(1.0-gate));
			if (gate == 0.0) inputSample = 0.0;			
		} else gate = 1.0;
		//end Gate
		
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
	//begin Gate
	WasNegative = false;
	ZeroCross = 0;
	gateroller = 0.0;
	gate = 0.0;
	//end Gate
	
	//begin ButterComp
	controlApos = 1.0;
	controlAneg = 1.0;
	controlBpos = 1.0;
	controlBneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;	
	avgA = avgB = 0.0;
	nvgA = nvgB = 0.0;
	//end ButterComp
	flip = false;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
