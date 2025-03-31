#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Tube2"
#define AIRWINDOWS_DESCRIPTION "Extends Tube, with more sophisticated processing and input trim."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','u','c' )
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
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tube", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double previousSampleA;
		double previousSampleB;
		double previousSampleC;
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
	
	double inputPad = GetParameter( kParam_One );
	double iterations = 1.0-GetParameter( kParam_Two );
	int powerfactor = (9.0*iterations)+1;
	double asymPad = (double)powerfactor;
	double gainscaling = 1.0/(double)(powerfactor+1);
	double outputscaling = 1.0 + (1.0/(double)(powerfactor));
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (inputPad < 1.0) inputSample *= inputPad;
		
		if (overallscale > 1.9) {
			double stored = inputSample;
			inputSample += previousSampleA; previousSampleA = stored; inputSample *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average		
				
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		
		//flatten bottom, point top of sine waveshaper
		inputSample /= asymPad;
		double sharpen = -inputSample;
		if (sharpen > 0.0) sharpen = 1.0+sqrt(sharpen);
		else sharpen = 1.0-sqrt(-sharpen);
		inputSample -= inputSample*fabs(inputSample)*sharpen*0.25;
		//this will take input from exactly -1.0 to 1.0 max
		inputSample *= asymPad;
		//and we are asym clipping more when Tube is cranked, to compensate
		
		//original Tube algorithm: powerfactor widens the more linear region of the wave
		double factor = inputSample;
		for (int x = 0; x < powerfactor; x++) factor *= inputSample;
		if ((powerfactor % 2 == 1) && (inputSample != 0.0)) factor = (factor/inputSample)*fabs(inputSample);		
		factor *= gainscaling;
		inputSample -= factor;
		inputSample *= outputscaling;
				
		if (overallscale > 1.9) {
			double stored = inputSample;
			inputSample += previousSampleB; previousSampleB = stored; inputSample *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average
		//end original Tube. Now we have a boosted fat sound peaking at 0dB exactly
				
		//hysteresis and spiky fuzz
		double slew = previousSampleC - inputSample;
		if (overallscale > 1.9) {
			double stored = inputSample;
			inputSample += previousSampleC; previousSampleC = stored; inputSample *= 0.5;
		} else previousSampleC = inputSample; //for this, need previousSampleC always
		if (slew > 0.0) slew = 1.0+(sqrt(slew)*0.5);
		else slew = 1.0-(sqrt(-slew)*0.5);
		inputSample -= inputSample*fabs(inputSample)*slew*gainscaling;
		//reusing gainscaling that's part of another algorithm
		if (inputSample > 0.52) inputSample = 0.52;
		if (inputSample < -0.52) inputSample = -0.52;
		inputSample *= 1.923076923076923;
		//end hysteresis and spiky fuzz section
		
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
	previousSampleA = 0.0;
	previousSampleB = 0.0;
	previousSampleC = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
