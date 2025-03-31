#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Tube"
#define AIRWINDOWS_DESCRIPTION "A tube style SoundBetterIzer using a new algorithm for analog modeling!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','u','b' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "TUBE", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;

		double previousSampleA;
		double previousSampleB;
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
	
	Float64 gain = 1.0+(GetParameter( kParam_One )*0.2246161992650486);
	//this maxes out at +1.76dB, which is the exact difference between what a triangle/saw wave
	//would be, and a sine (the fullest possible wave at the same peak amplitude). Why do this?
	//Because the nature of this plugin is the 'more FAT TUUUUBE fatness!' knob, and because
	//sticking to this amount maximizes that effect on a 'normal' sound that is itself unclipped
	//while confining the resulting 'clipped' area to what is already 'fattened' into a flat
	//and distorted region. You can always put a gain trim in front of it for more distortion,
	//or cascade them in the DAW for more distortion.
	Float64 iterations = 1.0-GetParameter( kParam_One );
	int powerfactor = (5.0*iterations)+1;
	double gainscaling = 1.0/(double)(powerfactor+1);
	double outputscaling = 1.0 + (1.0/(double)(powerfactor));
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (overallscale > 1.9) {
			double stored = inputSample;
			inputSample += previousSampleA; previousSampleA = stored; inputSample *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average		
		
		inputSample *= gain;
		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		
		double factor = inputSample;
		
		for (int x = 0; x < powerfactor; x++) factor *= inputSample;
		//this applies more and more of a 'curve' to the transfer function
		
		if ((powerfactor % 2 == 1) && (inputSample != 0.0)) factor = (factor/inputSample)*fabs(inputSample);
		//if we would've got an asymmetrical effect this undoes the last step, and then
		//redoes it using an absolute value to make the effect symmetrical again
		
		factor *= gainscaling;
		inputSample -= factor;
		inputSample *= outputscaling;
		
		/*  This is the simplest raw form of the 'fattest' TUBE boost between -1.0 and 1.0
		 
		if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
		inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*2.0;
		
		*/
		
		if (overallscale > 1.9) {
			double stored = inputSample;
			inputSample += previousSampleB; previousSampleB = stored; inputSample *= 0.5;
		} //for high sample rates on this plugin we are going to do a simple average
		
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
