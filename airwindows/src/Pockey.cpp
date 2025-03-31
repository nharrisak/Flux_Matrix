#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pockey"
#define AIRWINDOWS_DESCRIPTION "12 bit (and under) lo-fi hiphop in a plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','c' )
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
{ .name = "DeFreq", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double lastSample;
		double heldSample;
		double lastSoften;
		double position;
		double freq;
		double freqA;
		double freqB;
		double rez;
		double rezA;
		double rezB;
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	double freqMin = 0.08 / overallscale;
	freqA = freqB; freqB = (pow(1.0-GetParameter( kParam_One ),3)*(0.618033988749894848204586-freqMin))+freqMin;
	//freq is always engaged at least a little
	rezA = rezB; rezB = pow(GetParameter( kParam_Two )*0.618033988749894848204586,3)+0.000244140625;
	//rez is always at least 12 bit truncation
	double wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double freq = (freqA*temp)+(freqB*(1.0-temp));
		double rez = (rezA*temp)+(rezB*(1.0-temp));
		
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		if (inputSample > 0) inputSample = log(1.0+(255*fabs(inputSample)))/log(255);
		if (inputSample < 0) inputSample = -log(1.0+(255*fabs(inputSample)))/log(255);
		//end uLaw encode		
		
		double offset = inputSample;
		if (inputSample > 0)
		{
			while (offset > 0) {offset -= rez;}
			inputSample -= offset;
			//it's below 0 so subtracting adds the remainder
		}
		if (inputSample < 0)
		{
			while (offset < 0) {offset += rez;}
			inputSample -= offset;
			//it's above 0 so subtracting subtracts the remainder
		}
		inputSample *= (1.0 - rez);
			
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		if (inputSample > 0) inputSample = (pow(256,fabs(inputSample))-1.0) / 255;
		if (inputSample < 0) inputSample = -(pow(256,fabs(inputSample))-1.0) / 255;
		//end uLaw decode
		
		position += freq;
		double outputSample = heldSample;
		if (position > 1.0)
		{
			position -= 1.0;
			heldSample = (lastSample * position) + (inputSample * (1-position));
			outputSample = (heldSample * (1-position)) + (outputSample * position);
		}
		inputSample = outputSample;
		
		double slew = fabs(inputSample - lastSoften)*freq;
		if (slew > 0.5) slew = 0.5;
		inputSample = (inputSample * slew) + (lastSoften * (1.0-slew));
		//conditional average: only if we actually have brightness
		
		lastSample = drySample;
		lastSoften = outputSample;
		//end Frequency Derez
		
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
	lastSample = 0.0;
	heldSample = 0.0;
	lastSoften = 0.0;
	position = 0.0;
	freq = 0.5;
	freqA = 0.5;
	freqB = 0.5;
	rez = 0.5;
	rezA = 0.5;
	rezB = 0.5;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
