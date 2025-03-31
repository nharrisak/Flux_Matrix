#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pop3"
#define AIRWINDOWS_DESCRIPTION "The dynamics from ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','r' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	//Add your parameters here...
	kNumberOfParameters=8
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Thresld", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "C Ratio", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "C Atk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "C Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thresld", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "G Ratio", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "G Sust", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "G Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double popCompL;
	double popCompR;
	double popGate;
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double compThresh = pow(GetParameter( kParam_A ),4);
	double compRatio = 1.0-pow(1.0-GetParameter( kParam_B ),2);
	double compAttack = 1.0/(((pow(GetParameter( kParam_C ),3)*5000.0)+500.0)*overallscale);
	double compRelease = 1.0/(((pow(GetParameter( kParam_D ),5)*50000.0)+500.0)*overallscale);
	
	double gateThresh = pow(GetParameter( kParam_E ),4);
	double gateRatio = 1.0-pow(1.0-GetParameter( kParam_F ),2);
	double gateSustain = M_PI_2 * pow(GetParameter( kParam_G )+1.0,4.0);
	double gateRelease = 1.0/(((pow(GetParameter( kParam_H ),5)*500000.0)+500.0)*overallscale);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		if (fabs(inputSampleL) > compThresh) { //compression L
			popCompL -= (popCompL * compAttack);
			popCompL += ((compThresh / fabs(inputSampleL))*compAttack);
		} else popCompL = (popCompL*(1.0-compRelease))+compRelease;
		if (fabs(inputSampleR) > compThresh) { //compression R
			popCompR -= (popCompR * compAttack);
			popCompR += ((compThresh / fabs(inputSampleR))*compAttack);
		} else popCompR = (popCompR*(1.0-compRelease))+compRelease;
		if (popCompL > popCompR) popCompL -= (popCompL * compAttack);
		if (popCompR > popCompL) popCompR -= (popCompR * compAttack);
		if (fabs(inputSampleL) > gateThresh) popGate = gateSustain;
		else if (fabs(inputSampleR) > gateThresh) popGate = gateSustain;
		else popGate *= (1.0-gateRelease);
		if (popGate < 0.0) popGate = 0.0;
		popCompL = fmax(fmin(popCompL,1.0),0.0);
		popCompR = fmax(fmin(popCompR,1.0),0.0);
		inputSampleL *= ((1.0-compRatio)+(popCompL*compRatio));
		inputSampleR *= ((1.0-compRatio)+(popCompR*compRatio));
		if (popGate < M_PI_2) {
			inputSampleL *= ((1.0-gateRatio)+(sin(popGate)*gateRatio));
			inputSampleR *= ((1.0-gateRatio)+(sin(popGate)*gateRatio));
		}
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	popCompL = 1.0;
	popCompR = 1.0;
	popGate = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
