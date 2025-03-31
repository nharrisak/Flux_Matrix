#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Inflamer"
#define AIRWINDOWS_DESCRIPTION "An Airwindowsized take on the waveshapers in Oxford Inflator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','n','h' )
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
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Curve", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Effect", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	
	double gainControl = (GetParameter( kParam_One )*0.5)+0.05; //0.0 to 1.0
	int gainBits = 20; //start beyond maximum attenuation
	if (gainControl > 0.0) gainBits = floor(1.0 / gainControl)-2;
	if (gainBits < -2) gainBits = -2; if (gainBits > 17) gainBits = 17;
	double gain = 1.0;
	switch (gainBits)
	{
		case 17: gain = 0.0; break;
		case 16: gain = 0.0000152587890625; break;
		case 15: gain = 0.000030517578125; break;
		case 14: gain = 0.00006103515625; break;
		case 13: gain = 0.0001220703125; break;
		case 12: gain = 0.000244140625; break;
		case 11: gain = 0.00048828125; break;
		case 10: gain = 0.0009765625; break;
		case 9: gain = 0.001953125; break;
		case 8: gain = 0.00390625; break;
		case 7: gain = 0.0078125; break;
		case 6: gain = 0.015625; break;
		case 5: gain = 0.03125; break;
		case 4: gain = 0.0625; break;
		case 3: gain = 0.125; break;
		case 2: gain = 0.25; break;
		case 1: gain = 0.5; break;
		case 0: gain = 1.0; break;
		case -1: gain = 2.0; break;
		case -2: gain = 4.0; break;
	} //now we have our input trim
	
	int bitshiftL = 1;
	int bitshiftR = 1;
	double panControl = (GetParameter( kParam_Two )*2.0)-1.0; //-1.0 to 1.0
	double panAttenuation = (1.0-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0) panBits = floor(1.0 / panAttenuation);
	if (panControl > 0.25) bitshiftL += panBits;
	if (panControl < -0.25) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2; if (bitshiftR > 17) bitshiftR = 17;
	double negacurve = 0.5;
	double curve = 0.5;
	switch (bitshiftL)
	{
		case 17: negacurve = 0.0; break;
		case 16: negacurve = 0.0000152587890625; break;
		case 15: negacurve = 0.000030517578125; break;
		case 14: negacurve = 0.00006103515625; break;
		case 13: negacurve = 0.0001220703125; break;
		case 12: negacurve = 0.000244140625; break;
		case 11: negacurve = 0.00048828125; break;
		case 10: negacurve = 0.0009765625; break;
		case 9: negacurve = 0.001953125; break;
		case 8: negacurve = 0.00390625; break;
		case 7: negacurve = 0.0078125; break;
		case 6: negacurve = 0.015625; break;
		case 5: negacurve = 0.03125; break;
		case 4: negacurve = 0.0625; break;
		case 3: negacurve = 0.125; break;
		case 2: negacurve = 0.25; break;
		case 1: negacurve = 0.5; break;
		case 0: negacurve = 1.0; break;
		case -1: negacurve = 2.0; break;
		case -2: negacurve = 4.0; break;
	}
	switch (bitshiftR)
	{
		case 17: curve = 0.0; break;
		case 16: curve = 0.0000152587890625; break;
		case 15: curve = 0.000030517578125; break;
		case 14: curve = 0.00006103515625; break;
		case 13: curve = 0.0001220703125; break;
		case 12: curve = 0.000244140625; break;
		case 11: curve = 0.00048828125; break;
		case 10: curve = 0.0009765625; break;
		case 9: curve = 0.001953125; break;
		case 8: curve = 0.00390625; break;
		case 7: curve = 0.0078125; break;
		case 6: curve = 0.015625; break;
		case 5: curve = 0.03125; break;
		case 4: curve = 0.0625; break;
		case 3: curve = 0.125; break;
		case 2: curve = 0.25; break;
		case 1: curve = 0.5; break;
		case 0: curve = 1.0; break;
		case -1: curve = 2.0; break;
		case -2: curve = 4.0; break;
	}
	double effectOut = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		
		inputSample *= gain; //input trim
		double inflamerPlus = (inputSample*2.0)-pow(inputSample,2); //+50, very much just second harmonic
		double inflamerMinus = inputSample+(pow(inputSample,3)*0.25)-((pow(inputSample,2)+pow(inputSample,4))*0.0625); //-50
		inputSample = (inflamerPlus * curve) + (inflamerMinus * negacurve);
		inputSample = (inputSample * effectOut) + (drySample * (1.0-effectOut));
		
		
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
