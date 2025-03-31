#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Interstage"
#define AIRWINDOWS_DESCRIPTION "A subtle and sophisticated analogifier."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','n','t' )
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 iirSampleF;
		double lastSample;
		uint32_t fpd;
		bool flip;
	
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
	
	Float64 firstStage = 0.381966011250105 / overallscale;
	Float64 iirAmount = 0.00295 / overallscale;
	Float64 threshold = 0.381966011250105;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = *sourceP;
		
		inputSample = (inputSample+lastSample)*0.5; //start the lowpassing with an average
		
		if (flip) {
			iirSampleA = (iirSampleA * (1 - firstStage)) + (inputSample * firstStage); inputSample = iirSampleA;
			iirSampleC = (iirSampleC * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleC;
			iirSampleE = (iirSampleE * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleE;
			inputSample = drySample - inputSample;
			//make highpass
			if (inputSample - iirSampleA > threshold) inputSample = iirSampleA + threshold;
			if (inputSample - iirSampleA < -threshold) inputSample = iirSampleA - threshold;
			//slew limit against lowpassed reference point
		} else {
			iirSampleB = (iirSampleB * (1 - firstStage)) + (inputSample * firstStage); inputSample = iirSampleB;
			iirSampleD = (iirSampleD * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleD;
			iirSampleF = (iirSampleF * (1 - iirAmount)) + (inputSample * iirAmount); inputSample = iirSampleF;
			inputSample = drySample - inputSample;
			//make highpass
			if (inputSample - iirSampleB > threshold) inputSample = iirSampleB + threshold;
			if (inputSample - iirSampleB < -threshold) inputSample = iirSampleB - threshold;
			//slew limit against lowpassed reference point
		}
		flip = !flip;
		lastSample = inputSample;
		
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
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	flip = true;
}
};
