#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Zoom"
#define AIRWINDOWS_DESCRIPTION "A bank of Density-like waveshapers for subtly modulating peak energy."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Z','o','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Zoom", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	
	double zoom = (GetParameter( kParam_A )*2.0)-1.0;
	double zoomStages = (fabs(zoom)*4.0)+1.0;
	for (int count = 0; count < sqrt(zoomStages); count++) zoom *= fabs(zoom);
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		for (int count = 0; count < zoomStages; count++) {
			if (zoom > 0.0) {
				double closer = inputSample * 1.57079633;
				if (closer > 1.57079633) closer = 1.57079633;
				if (closer < -1.57079633) closer = -1.57079633;
				inputSample = (inputSample*(1.0-zoom))+(sin(closer)*zoom);
			} //zooming in will make the body of the sound louder: it's just Density
			if (zoom < 0.0) {
				double farther = fabs(inputSample) * 1.57079633;
				if (farther > 1.57079633) farther = 1.0;
				else farther = 1.0-cos(farther);
				if (inputSample > 0.0) inputSample = (inputSample*(1.0+zoom))-(farther*zoom*1.57079633);
				if (inputSample < 0.0) inputSample = (inputSample*(1.0+zoom))+(farther*zoom*1.57079633);			
			} //zooming out boosts the hottest peaks but cuts back softer stuff
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
