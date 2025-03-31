#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BitShiftPan"
#define AIRWINDOWS_DESCRIPTION "Gives you a gain and a pan control that are ONLY done using bit shifts."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','$' )
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Vol", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;

	double gainControl = (GetParameter( kParam_One )*0.5)+0.05; //0.0 to 1.0
	int gainBits = 20; //start beyond maximum attenuation
	if (gainControl > 0.0) gainBits = floor(1.0 / gainControl);
	int bitshiftL = gainBits - 3;
	int bitshiftR = gainBits - 3;
	double panControl = (GetParameter( kParam_Two )*2.0)-1.0; //-1.0 to 1.0
	double panAttenuation = (1.0-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0) panBits = floor(1.0 / panAttenuation);
	if (panControl > 0.25) bitshiftL += panBits;
	if (panControl < -0.25) bitshiftR += panBits;
	if (bitshiftL < -2) bitshiftL = -2; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < -2) bitshiftR = -2; if (bitshiftR > 17) bitshiftR = 17;
	double gainL = 1.0;
	double gainR = 1.0;
	switch (bitshiftL)
	{
		case 17: gainL = 0.0; break;
		case 16: gainL = 0.0000152587890625; break;
		case 15: gainL = 0.000030517578125; break;
		case 14: gainL = 0.00006103515625; break;
		case 13: gainL = 0.0001220703125; break;
		case 12: gainL = 0.000244140625; break;
		case 11: gainL = 0.00048828125; break;
		case 10: gainL = 0.0009765625; break;
		case 9: gainL = 0.001953125; break;
		case 8: gainL = 0.00390625; break;
		case 7: gainL = 0.0078125; break;
		case 6: gainL = 0.015625; break;
		case 5: gainL = 0.03125; break;
		case 4: gainL = 0.0625; break;
		case 3: gainL = 0.125; break;
		case 2: gainL = 0.25; break;
		case 1: gainL = 0.5; break;
		case 0: gainL = 1.0; break;
		case -1: gainL = 2.0; break;
		case -2: gainL = 4.0; break;
	}
	switch (bitshiftR)
	{
		case 17: gainR = 0.0; break;
		case 16: gainR = 0.0000152587890625; break;
		case 15: gainR = 0.000030517578125; break;
		case 14: gainR = 0.00006103515625; break;
		case 13: gainR = 0.0001220703125; break;
		case 12: gainR = 0.000244140625; break;
		case 11: gainR = 0.00048828125; break;
		case 10: gainR = 0.0009765625; break;
		case 9: gainR = 0.001953125; break;
		case 8: gainR = 0.00390625; break;
		case 7: gainR = 0.0078125; break;
		case 6: gainR = 0.015625; break;
		case 5: gainR = 0.03125; break;
		case 4: gainR = 0.0625; break;
		case 3: gainR = 0.125; break;
		case 2: gainR = 0.25; break;
		case 1: gainR = 0.5; break;
		case 0: gainR = 1.0; break;
		case -1: gainR = 2.0; break;
		case -2: gainR = 4.0; break;
	}
	
	while (nSampleFrames-- > 0) {
		*outputL = *inputL * gainL; //in this plugin, all the work is done 
		*outputR = *inputR * gainR; //before the buffer is processed
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	return noErr;
}

};
