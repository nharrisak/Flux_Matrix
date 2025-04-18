#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BrightAmbience3"
#define AIRWINDOWS_DESCRIPTION "Adds undersampling for high sample rates, and better feedback."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','r','k' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Start", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Length", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Feedback", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	int gcount;	
	float feedbackA;
	float feedbackB;
	float figureL[9];
	float figureR[9];
	float lastRefL[10];
	float lastRefR[10];
	int cycle;	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		Float32 pL[32768];
	Float32 pR[32768];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check	
	
	int start = (int)(GetParameter( kParam_One ) * 400)+88;
	int length = (int)(pow(GetParameter( kParam_Two ),2) * 487)+1;
	if (start + length > 488) start = 488-length;
	Float32 feedbackAmount = GetParameter( kParam_Three )*0.25f;
	Float32 wet = GetParameter( kParam_Four );
	
	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	figureL[0] = figureR[0] = 1000.0f/GetSampleRate(); //fixed frequency, 3.515775k
	figureL[1] = figureR[1] = pow(length*0.037f*feedbackAmount,2)+0.01f; //resonance
	float K = tan(M_PI * figureR[0]);
	float norm = 1.0f / (1.0f + K / figureR[1] + K * K);
	figureL[2] = figureR[2] = K / figureR[1] * norm;
	figureL[4] = figureR[4] = -figureR[2];
	figureL[5] = figureR[5] = 2.0f * (K * K - 1.0f) * norm;
	figureL[6] = figureR[6] = (1.0f - K / figureR[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do an Air sample
			float tempL = 0.0f;
			float tempR = 0.0f;
			if (gcount < 0 || gcount > 32767) gcount = 32767; int count = gcount;
			
			dram->pL[count] = inputSampleL + feedbackB;
			dram->pR[count] = inputSampleR + feedbackA;
			
			for(int offset = start; offset < start+length; offset++) {
				tempL += dram->pL[count+primeL[offset] - ((count+primeL[offset] > 32767)?32768:0)];
				tempR += dram->pR[count+primeR[offset] - ((count+primeR[offset] > 32767)?32768:0)];
			}
			
			inputSampleL = tempL/cbrt(length);
			inputSampleR = tempR/cbrt(length);
			
			float tempSample = (inputSampleL * figureL[2]) + figureL[7];
			figureL[7] = -(tempSample * figureL[5]) + figureL[8];
			figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
			feedbackA = sin(tempSample) * feedbackAmount;
			
			tempSample = (inputSampleR * figureR[2]) + figureR[7];
			figureR[7] = -(tempSample * figureR[5]) + figureR[8];
			figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
			feedbackB = sin(tempSample) * feedbackAmount;
			gcount--;
			
			if (cycleEnd == 4) {
				lastRefL[0] = lastRefL[4]; //start from previous last
				lastRefL[2] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[1] = (lastRefL[0] + lastRefL[2])/2; //one quarter
				lastRefL[3] = (lastRefL[2] + inputSampleL)/2; //three quarters
				lastRefL[4] = inputSampleL; //full
				lastRefR[0] = lastRefR[4]; //start from previous last
				lastRefR[2] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[1] = (lastRefR[0] + lastRefR[2])/2; //one quarter
				lastRefR[3] = (lastRefR[2] + inputSampleR)/2; //three quarters
				lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				lastRefL[0] = lastRefL[3]; //start from previous last
				lastRefL[2] = (lastRefL[0]+lastRefL[0]+inputSampleL)/3; //third
				lastRefL[1] = (lastRefL[0]+inputSampleL+inputSampleL)/3; //two thirds
				lastRefL[3] = inputSampleL; //full
				lastRefR[0] = lastRefR[3]; //start from previous last
				lastRefR[2] = (lastRefR[0]+lastRefR[0]+inputSampleR)/3; //third
				lastRefR[1] = (lastRefR[0]+inputSampleR+inputSampleR)/3; //two thirds
				lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				lastRefL[0] = lastRefL[2]; //start from previous last
				lastRefL[1] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[2] = inputSampleL; //full
				lastRefR[0] = lastRefR[2]; //start from previous last
				lastRefR[1] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				lastRefL[0] = inputSampleL;
				lastRefR[0] = inputSampleR;
			}
			cycle = 0; //reset
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
		} else {
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[7])*0.5f;
				lastRefL[7] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[7])*0.5f;
				lastRefR[7] = lastRefR[8]; //continue, do not break
			case 3:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[6])*0.5f;
				lastRefL[6] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[6])*0.5f;
				lastRefR[6] = lastRefR[8]; //continue, do not break
			case 2:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[5])*0.5f;
				lastRefL[5] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[5])*0.5f;
				lastRefR[5] = lastRefR[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (wet != 1.0f) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		
		
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
	for(int count = 0; count < 32767; count++) {dram->pL[count] = 0.0; dram->pR[count] = 0.0;}
	feedbackA = feedbackB = 0.0;
	for (int x = 0; x < 9; x++) {figureL[x] = 0.0; figureR[x] = 0.0;}
	for (int c = 0; c < 9; c++) {lastRefL[c] = 0.0;lastRefR[c] = 0.0;}
	cycle = 0;
	gcount = 0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
