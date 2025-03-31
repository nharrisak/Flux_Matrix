#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Distortion"
#define AIRWINDOWS_DESCRIPTION "A slightly dark analog-style distortion with several presets, like Focus."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','x' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
static const int kDENSITY = 0;
static const int kDRIVE = 1;
static const int kSPIRAL = 2;
static const int kMOJO = 3;
static const int kDYNO = 4;
static const int kDefaultValue_ParamTwo = kSPIRAL;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static char const * const enumStrings1[] = { "Density", "Drive", "Spiral", "Mojo", "Dyno", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mode", .min = 0, .max = 4, .def = 2, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings1 },
{ .name = "Output", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		double previousIn[9];
		double previousOut[9];
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
	int stages = (int)floor(GetSampleRate()/25000.0);
	if (stages > 8) stages = 8;
	Float64 input = pow(10.0,GetParameter( kParam_One )/20.0);
	int mode = (int) GetParameter( kParam_Two );
	Float64 output = pow(10.0,GetParameter( kParam_Three )/20.0);
	Float64 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		inputSample *= input;
		
		for (int x = 0; x < stages; x++) {
			double temp;
			temp = (inputSample+previousIn[x])*0.5;
			previousIn[x] = inputSample;
			inputSample = temp;
		}
		
		switch (mode)
		{
			case 0: //Density
				if (inputSample > 1.570796326794897) inputSample = 1.570796326794897;
				if (inputSample < -1.570796326794897) inputSample = -1.570796326794897;
				//clip to 1.570796326794897 to reach maximum output
				inputSample = sin(inputSample);
				break;
			case 1: //Drive				
				if (inputSample > 1.0) inputSample = 1.0;
				if (inputSample < -1.0) inputSample = -1.0;
				inputSample -= (inputSample * (fabs(inputSample) * 0.6) * (fabs(inputSample) * 0.6));
				inputSample *= 1.5;
				break;
			case 2: //Spiral
				if (inputSample > 1.2533141373155) inputSample = 1.2533141373155;
				if (inputSample < -1.2533141373155) inputSample = -1.2533141373155;
				//clip to 1.2533141373155 to reach maximum output
				inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0) ?1:fabs(inputSample));
				break;
			case 3: //Mojo
				double mojo; mojo = pow(fabs(inputSample),0.25);
				if (mojo > 0.0) inputSample = (sin(inputSample * mojo * M_PI * 0.5) / mojo) * 0.987654321;
				//mojo is the one that flattens WAAAAY out very softly before wavefolding				
				break;
			case 4: //Dyno
				double dyno; dyno = pow(fabs(inputSample),4);
				if (dyno > 0.0) inputSample = (sin(inputSample * dyno) / dyno) * 1.1654321;
				//dyno is the one that tries to raise peak energy				
				break;
		}
		
		for (int x = 1; x < (stages/2); x++) {
			double temp;
			temp = (inputSample+previousOut[x])*0.5;
			previousOut[x] = inputSample;
			inputSample = temp;
		}		
		
		if (output != 1.0) {
			inputSample *= output;
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
	for (int x = 0; x < 9; x++) {previousIn[x] = 0.0; previousOut[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
