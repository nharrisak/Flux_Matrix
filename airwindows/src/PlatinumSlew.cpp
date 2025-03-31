#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PlatinumSlew"
#define AIRWINDOWS_DESCRIPTION "The best easy-to-use slew clipper."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','l','a' )
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
{ .name = "Slew", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		enum {
			prevSampL1,
			prevSampR1,
			invSampL1,
			invSampR1,
			threshold1,
			prevSampL2,
			prevSampR2,
			invSampL2,
			invSampR2,
			threshold2,
			prevSampL3,
			prevSampR3,
			invSampL3,
			invSampR3,
			threshold3,
			prevSampL4,
			prevSampR4,
			invSampL4,
			invSampR4,
			threshold4,
			prevSampL5,
			prevSampR5,
			invSampL5,
			invSampR5,
			threshold5,
			prevSampL6,
			prevSampR6,
			invSampL6,
			invSampR6,
			threshold6,
			prevSampL7,
			prevSampR7,
			invSampL7,
			invSampR7,
			threshold7,
			prevSampL8,
			prevSampR8,
			invSampL8,
			invSampR8,
			threshold8,
			prevSampL9,
			prevSampR9,
			invSampL9,
			invSampR9,
			threshold9,
			prevSampL10,
			prevSampR10,
			invSampL10,
			invSampR10,
			threshold10,
			gslew_total
		}; //fixed frequency pear filter for ultrasonics, stereo
		double gslew[gslew_total]; //probably worth just using a number here
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
	
	double source = pow(1-GetParameter( kParam_One ),4)/overallscale;
		
	gslew[threshold10] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold9] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold8] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold7] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold6] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold5] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold4] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold3] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold2] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold1] = source;
	source *= 1.618033988749894848204586;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		for (int x = 0; x < gslew_total; x += 5) {
			
			if (((inputSample-gslew[x])-((gslew[x]-gslew[x+2])*0.618033988749894848204586)) > gslew[x+4])
				inputSample = (gslew[x]-((gslew[x]-gslew[x+2])*0.156)) + (gslew[x+4]*0.844);
			if (-((inputSample-gslew[x])-((gslew[x]-gslew[x+2])*0.618033988749894848204586)) > gslew[x+4])
				inputSample = (gslew[x]-((gslew[x]-gslew[x+2])*0.2)) - (gslew[x+4]*0.8);
			gslew[x+2] = gslew[x]*0.844;
			gslew[x] = inputSample;
			
			//if (((inputSampleR-gslew[x+1])-((gslew[x+1]-gslew[x+3])*0.618033988749894848204586)) > gslew[x+4])
			//	inputSampleR = (gslew[x+1]-((gslew[x+1]-gslew[x+3])*0.156)) + (gslew[x+4]*0.844);
			//if (-((inputSampleR-gslew[x+1])-((gslew[x+1]-gslew[x+3])*0.618033988749894848204586)) > gslew[x+4])
			//	inputSampleR = (gslew[x+1]-((gslew[x+1]-gslew[x+3])*0.2)) - (gslew[x+4]*0.8);
			//gslew[x+3] = gslew[x+1]*0.844;
			//gslew[x+1] = inputSampleR;
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
	for (int x = 0; x < gslew_total; x++) gslew[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
