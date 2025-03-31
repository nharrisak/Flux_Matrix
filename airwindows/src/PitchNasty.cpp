#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PitchNasty"
#define AIRWINDOWS_DESCRIPTION "A primitive pitch shifter that also has primitive time-stretch artifacts."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','i','u' )
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
{ .name = "Note", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bend", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Grind", .min = -3600, .max = 3600, .def = -1200, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Feedback", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		int inCount;
		double outCount;
		bool switchTransition;
		double switchAmount;
		double feedbackSample;
		uint32_t fpd;
	
	struct _dram {
			double d[10002];
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
	
	double note = GetParameter( kParam_One );// -12 to +12
	double bend = GetParameter( kParam_Two );// -12.0 to +12.0
	double speed = pow(2,note/12.0)*pow(2,bend/12.0);
	
	double grindRef = (1.0/261.6)*GetSampleRate(); //samples per Middle C cycle
	double grindNote = pow(2,GetParameter( kParam_Three )/12.0); // -36 to 36
	double width = grindRef / grindNote;
	if (width > 9990) width = 9990; //safety check
	double feedback = GetParameter( kParam_Four );
	double trim = GetParameter( kParam_Five );
	double wet = GetParameter( kParam_Six );

	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		inputSample += (feedbackSample * feedback);
		if (fabs(feedbackSample) > 1.0) inputSample /= fabs(feedbackSample);
		
		dram->d[inCount] = inputSample;
		
		inCount++;
		outCount += speed;
		
		if (outCount > inCount && switchTransition) {
			switchAmount = 1.0;
			switchTransition = false;
		}
				
		if (inCount > width) inCount -= width;
		if (outCount > width) {
			outCount -= width;
			switchTransition = true;
			//reset the transition point
		}
				
		int count = floor(outCount);
		int arrayWidth = floor(width);
		
		inputSample = (dram->d[count-((count > arrayWidth)?arrayWidth+1:0)] * (1-(outCount-floor(outCount))));
		inputSample += (dram->d[count+1-((count+1 > arrayWidth)?arrayWidth+1:0)] * (outCount-floor(outCount)));
		
		if (switchAmount > fabs(inputSample-dram->d[inCount])*2.0) {
			switchAmount = (switchAmount * 0.5) + (fabs(inputSample-dram->d[inCount]));
		}
		inputSample = (dram->d[inCount] * switchAmount) + (inputSample * (1.0-switchAmount));
		
		feedbackSample = inputSample;
		
		if (trim != 1.0) {
			inputSample *= trim;
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
	for (int temp = 0; temp < 10001; temp++) {dram->d[temp] = 0.0;}
	inCount = 1;	
	outCount = 1.0;
	switchTransition = false;
	switchAmount = 0.0;
	feedbackSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
