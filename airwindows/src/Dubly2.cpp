#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dubly2"
#define AIRWINDOWS_DESCRIPTION "A key part of seventies sonics!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','g' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	//Add your parameters here...
	kNumberOfParameters=7
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "EncAmnt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "EncFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "TapeDrv", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DecAmnt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DecFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Out Pad", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		double iirEnc;
		double iirDec;

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

	double dublyAmount = pow(GetParameter( kParam_A ),3)*0.25;
	double iirEncFreq = GetParameter( kParam_B )/overallscale;
	double tapeDrv = (GetParameter( kParam_C )*2.0)+1.0;
	
	double outlyAmount = pow(GetParameter( kParam_D ),3)*0.25;
	double iirDecFreq = GetParameter( kParam_E )/overallscale;
	double outPad = (GetParameter( kParam_F )*2.0)+1.0;
	if (tapeDrv > 1.0) outPad += 0.005;
	
	double wet = GetParameter( kParam_G );

	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		
		iirEnc = (iirEnc * (1.0 - iirEncFreq)) + (inputSample * iirEncFreq);
		double doubly = inputSample - iirEnc;
		if (doubly > 1.0) doubly = 1.0; if (doubly < -1.0) doubly = -1.0;
		if (doubly > 0) doubly = log(1.0+(255*fabs(doubly)))/2.40823996531;
		if (doubly < 0) doubly = -log(1.0+(255*fabs(doubly)))/2.40823996531;
		inputSample += doubly*dublyAmount;
		//end Dubly encode
		
		if (tapeDrv > 1.0) inputSample *= tapeDrv;
		if (inputSample > 1.57079633) inputSample = 1.57079633;
		if (inputSample < -1.57079633) inputSample = -1.57079633;
		inputSample = sin(inputSample);
		if (outPad > 1.0) inputSample /= outPad;
		
		iirDec = (iirDec * (1.0 - iirDecFreq)) + (inputSample * iirDecFreq);
		doubly = inputSample - iirDec;
		if (doubly > 1.0) doubly = 1.0; if (doubly < -1.0) doubly = -1.0;
		if (doubly > 0) doubly = log(1.0+(255*fabs(doubly)))/2.40823996531;
		if (doubly < 0) doubly = -log(1.0+(255*fabs(doubly)))/2.40823996531;
		inputSample -= doubly*outlyAmount;
		//end Dubly decode
		
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
	iirEnc = 0.0; iirDec = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
