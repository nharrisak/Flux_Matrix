#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dubly3"
#define AIRWINDOWS_DESCRIPTION "Refines and transforms the Dubly sound."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','h' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tilt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Shape", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		double iirEnc;
		double iirDec;
		double compEnc;
		double compDec;
		double avgEnc;
		double avgDec;

		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
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
	
	double inputGain = pow(GetParameter( kParam_A )*2.0,2.0);
	double dublyAmount = GetParameter( kParam_B )*2.0;
	double outlyAmount = (1.0-GetParameter( kParam_B ))*-2.0;
	if (outlyAmount < -1.0) outlyAmount = -1.0;
	double iirEncFreq = (1.0-GetParameter( kParam_C ))/overallscale;
	double iirDecFreq = GetParameter( kParam_C )/overallscale;
	double outputGain = GetParameter( kParam_D )*2.0;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (inputGain != 1.0) {
			inputSample *= inputGain;
		}
		
		//Dubly encode
		iirEnc = (iirEnc * (1.0 - iirEncFreq)) + (inputSample * iirEncFreq);
		double highPart = ((inputSample-iirEnc)*2.848);
		highPart += avgEnc; avgEnc = (inputSample-iirEnc)*1.152;
		if (highPart > 1.0) highPart = 1.0; if (highPart < -1.0) highPart = -1.0;
		double dubly = fabs(highPart);
		if (dubly > 0.0) {
			double adjust = log(1.0+(255.0*dubly))/2.40823996531;
			if (adjust > 0.0) dubly /= adjust;
			compEnc = (compEnc*(1.0-iirEncFreq))+(dubly*iirEncFreq);
			inputSample += ((highPart*compEnc)*dublyAmount);
		} //end Dubly encode
		
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		
		//Dubly decode
		iirDec = (iirDec * (1.0 - iirDecFreq)) + (inputSample * iirDecFreq);
		highPart = ((inputSample-iirDec)*2.628);
		highPart += avgDec; avgDec = (inputSample-iirDec)*1.372;
		if (highPart > 1.0) highPart = 1.0; if (highPart < -1.0) highPart = -1.0;
		dubly = fabs(highPart);
		if (dubly > 0.0) {
			double adjust = log(1.0+(255.0*dubly))/2.40823996531;
			if (adjust > 0.0) dubly /= adjust;
			compDec = (compDec*(1.0-iirDecFreq))+(dubly*iirDecFreq);
			inputSample += ((highPart*compDec)*outlyAmount);
		} //end Dubly decode
		
		if (outputGain != 1.0) {
			inputSample *= outputGain;
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
	iirEnc = 0.0; iirDec = 0.0;
	compEnc = 1.0; compDec = 1.0;
	avgEnc = 0.0; avgDec = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
