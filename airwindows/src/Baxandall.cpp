#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Baxandall"
#define AIRWINDOWS_DESCRIPTION "A killer general-purpose EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','x' )
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
{ .name = "Treble", .min = -15000, .max = 15000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = -15000, .max = 15000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = -15000, .max = 15000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		double trebleA[9];
		double trebleB[9];
		double bassA[9];
		double bassB[9];
		bool flip;
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
	
	Float64 trebleGain = pow(10.0,GetParameter( kParam_One )/20.0);
	Float64 trebleFreq = (4410.0*trebleGain)/GetSampleRate();
	if (trebleFreq > 0.45) trebleFreq = 0.45;
	trebleA[0] = trebleB[0] = trebleFreq;
	Float64 bassGain = pow(10.0,GetParameter( kParam_Two )/20.0);
	Float64 bassFreq = pow(10.0,-GetParameter( kParam_Two )/20.0);
	bassFreq = (8820.0*bassFreq)/GetSampleRate();
	if (bassFreq > 0.45) bassFreq = 0.45;
	bassA[0] = bassB[0] = bassFreq;
    trebleA[1] = trebleB[1] = 0.4;
    bassA[1] = bassB[1] = 0.2;
	Float64 output = pow(10.0,GetParameter( kParam_Three )/20.0);
	
	double K = tan(M_PI * trebleA[0]);
	double norm = 1.0 / (1.0 + K / trebleA[1] + K * K);
	trebleB[2] = trebleA[2] = K * K * norm;
	trebleB[3] = trebleA[3] = 2.0 * trebleA[2];
	trebleB[4] = trebleA[4] = trebleA[2];
	trebleB[5] = trebleA[5] = 2.0 * (K * K - 1.0) * norm;
	trebleB[6] = trebleA[6] = (1.0 - K / trebleA[1] + K * K) * norm;
	
	K = tan(M_PI * bassA[0]);
	norm = 1.0 / (1.0 + K / bassA[1] + K * K);
	bassB[2] = bassA[2] = K * K * norm;
	bassB[3] = bassA[3] = 2.0 * bassA[2];
	bassB[4] = bassA[4] = bassA[2];
	bassB[5] = bassA[5] = 2.0 * (K * K - 1.0) * norm;
	bassB[6] = bassA[6] = (1.0 - K / bassA[1] + K * K) * norm;
		
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (output != 1.0) {
			inputSample *= output;
		}//gain trim in front of plugin, in case Console stage clips
		
		inputSample = sin(inputSample);
		//encode Console5: good cleanness
		
		double trebleSample;
		double bassSample;

		if (flip)
		{
			trebleSample = (inputSample * trebleA[2]) + trebleA[7];
			trebleA[7] = (inputSample * trebleA[3]) - (trebleSample * trebleA[5]) + trebleA[8];
			trebleA[8] = (inputSample * trebleA[4]) - (trebleSample * trebleA[6]);
			trebleSample = inputSample - trebleSample;
			
			bassSample = (inputSample * bassA[2]) + bassA[7];
			bassA[7] = (inputSample * bassA[3]) - (bassSample * bassA[5]) + bassA[8];
			bassA[8] = (inputSample * bassA[4]) - (bassSample * bassA[6]);
		}
		else
		{
			trebleSample = (inputSample * trebleB[2]) + trebleB[7];
			trebleB[7] = (inputSample * trebleB[3]) - (trebleSample * trebleB[5]) + trebleB[8];
			trebleB[8] = (inputSample * trebleB[4]) - (trebleSample * trebleB[6]);
			trebleSample = inputSample - trebleSample;
			
			bassSample = (inputSample * bassB[2]) + bassB[7];
			bassB[7] = (inputSample * bassB[3]) - (bassSample * bassB[5]) + bassB[8];
			bassB[8] = (inputSample * bassB[4]) - (bassSample * bassB[6]);
		}
		flip = !flip;
		
		trebleSample *= trebleGain;
		bassSample *= bassGain;
		inputSample = bassSample + trebleSample; //interleaved biquad

		
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		//without this, you can get a NaN condition where it spits out DC offset at full blast!
		inputSample = asin(inputSample);
		//amplitude aspect
		
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
	for (int x = 0; x < 9; x++) {
		trebleA[x] = 0.0;
		trebleB[x] = 0.0;
		bassA[x] = 0.0;
		bassB[x] = 0.0;
	}
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
