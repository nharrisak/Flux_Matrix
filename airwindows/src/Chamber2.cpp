#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Chamber2"
#define AIRWINDOWS_DESCRIPTION "A feedforward reverb, a blur delay, and a glitch topping!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','b' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Delay", .min = 0, .max = 1000, .def = 340, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 310, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thick", .min = 0, .max = 1000, .def = 280, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wet", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
    
	double aEL[10000];
	double aFL[10000];
	double aGL[10000];
	double aHL[10000];
	double aAL[10000];
	double aBL[10000];
	double aCL[10000];
	double aDL[10000];
	double aIL[10000];
	double aJL[10000];
	double aKL[10000];
	double aLL[10000];
	double aML[10000];
	
	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double previousAL;
	double previousBL;
	double previousCL;
	double previousDL;
	
	double lastRefL[10];
	
	double aER[10000];
	double aFR[10000];
	double aGR[10000];
	double aHR[10000];
	double aAR[10000];
	double aBR[10000];
	double aCR[10000];
	double aDR[10000];
	double aIR[10000];
	double aJR[10000];
	double aKR[10000];
	double aLR[10000];
	double aMR[10000];
	
	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	double previousAR;
	double previousBR;
	double previousCR;
	double previousDR;
	
	double lastRefR[10];
	
	int countA, delayA;
	int countB, delayB;
	int countC, delayC;
	int countD, delayD;
	int countE, delayE;
	int countF, delayF;
	int countG, delayG;
	int countH, delayH;
	int countI, delayI;
	int countJ, delayJ;
	int countK, delayK;
	int countL, delayL;
	int countM, delayM;
	int cycle; //all these ints are shared across channels, not duplicated	
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	Float64 size = (GetParameter( kParam_One )*0.9)+0.1;
	Float64 regen = (1.0-(pow(1.0-GetParameter( kParam_Two ),2)))*0.123;
	Float64 echoScale = 1.0-GetParameter( kParam_Three );
	Float64 echo = 0.618033988749894848204586+((1.0-0.618033988749894848204586)*echoScale);
	Float64 interpolate = (1.0-echo)*0.381966011250105;
	//this now goes from Chamber, to all the reverb delays being exactly the same
	//much larger usage of RAM due to the larger reverb delays everywhere, but
	//ability to go to an unusual variation on blurred delay.
	Float64 wet = GetParameter( kParam_Four )*2.0;
	Float64 dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	delayM = sqrt(9900*size);
	delayE = 9900*size;
	delayF = delayE*echo; 
	delayG = delayF*echo;
	delayH = delayG*echo;
	delayA = delayH*echo;
	delayB = delayA*echo;
	delayC = delayB*echo;
	delayD = delayC*echo;
	delayI = delayD*echo;
	delayJ = delayI*echo;
	delayK = delayJ*echo;
	delayL = delayK*echo;
	//initially designed around the Fibonnaci series, Chamber uses
	//delay coefficients that are all related to the Golden Ratio,
	//Turns out that as you continue to sustain them, it turns from a
	//chunky slapback effect into a smoother reverb tail that can
	//sustain infinitely.	
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			
			aML[countM] = inputSampleL;
			aMR[countM] = inputSampleR;
			countM++; if (countM < 0 || countM > delayM) countM = 0;
			inputSampleL = aML[countM-((countM > delayM)?delayM+1:0)];
			inputSampleR = aMR[countM-((countM > delayM)?delayM+1:0)];
			//predelay to make the first echo still be an echo even when blurred
			
			feedbackAL = (feedbackAL*(1.0-interpolate))+(previousAL*interpolate); previousAL = feedbackAL;
			feedbackBL = (feedbackBL*(1.0-interpolate))+(previousBL*interpolate); previousBL = feedbackBL;
			feedbackCL = (feedbackCL*(1.0-interpolate))+(previousCL*interpolate); previousCL = feedbackCL;
			feedbackDL = (feedbackDL*(1.0-interpolate))+(previousDL*interpolate); previousDL = feedbackDL;
			feedbackAR = (feedbackAR*(1.0-interpolate))+(previousAR*interpolate); previousAR = feedbackAR;
			feedbackBR = (feedbackBR*(1.0-interpolate))+(previousBR*interpolate); previousBR = feedbackBR;
			feedbackCR = (feedbackCR*(1.0-interpolate))+(previousCR*interpolate); previousCR = feedbackCR;
			feedbackDR = (feedbackDR*(1.0-interpolate))+(previousDR*interpolate); previousDR = feedbackDR;
			
			aIL[countI] = inputSampleL + (feedbackAL * regen);
			aJL[countJ] = inputSampleL + (feedbackBL * regen);
			aKL[countK] = inputSampleL + (feedbackCL * regen);
			aLL[countL] = inputSampleL + (feedbackDL * regen);
			aIR[countI] = inputSampleR + (feedbackAR * regen);
			aJR[countJ] = inputSampleR + (feedbackBR * regen);
			aKR[countK] = inputSampleR + (feedbackCR * regen);
			aLR[countL] = inputSampleR + (feedbackDR * regen);
			
			countI++; if (countI < 0 || countI > delayI) countI = 0;
			countJ++; if (countJ < 0 || countJ > delayJ) countJ = 0;
			countK++; if (countK < 0 || countK > delayK) countK = 0;
			countL++; if (countL < 0 || countL > delayL) countL = 0;
			
			double outIL = aIL[countI-((countI > delayI)?delayI+1:0)];
			double outJL = aJL[countJ-((countJ > delayJ)?delayJ+1:0)];
			double outKL = aKL[countK-((countK > delayK)?delayK+1:0)];
			double outLL = aLL[countL-((countL > delayL)?delayL+1:0)];
			double outIR = aIR[countI-((countI > delayI)?delayI+1:0)];
			double outJR = aJR[countJ-((countJ > delayJ)?delayJ+1:0)];
			double outKR = aKR[countK-((countK > delayK)?delayK+1:0)];
			double outLR = aLR[countL-((countL > delayL)?delayL+1:0)];
			//first block: now we have four outputs
			
			aAL[countA] = (outIL - (outJL + outKL + outLL));
			aBL[countB] = (outJL - (outIL + outKL + outLL));
			aCL[countC] = (outKL - (outIL + outJL + outLL));
			aDL[countD] = (outLL - (outIL + outJL + outKL));
			aAR[countA] = (outIR - (outJR + outKR + outLR));
			aBR[countB] = (outJR - (outIR + outKR + outLR));
			aCR[countC] = (outKR - (outIR + outJR + outLR));
			aDR[countD] = (outLR - (outIR + outJR + outKR));
			
			countA++; if (countA < 0 || countA > delayA) countA = 0;
			countB++; if (countB < 0 || countB > delayB) countB = 0;
			countC++; if (countC < 0 || countC > delayC) countC = 0;
			countD++; if (countD < 0 || countD > delayD) countD = 0;
			
			double outAL = aAL[countA-((countA > delayA)?delayA+1:0)];
			double outBL = aBL[countB-((countB > delayB)?delayB+1:0)];
			double outCL = aCL[countC-((countC > delayC)?delayC+1:0)];
			double outDL = aDL[countD-((countD > delayD)?delayD+1:0)];
			double outAR = aAR[countA-((countA > delayA)?delayA+1:0)];
			double outBR = aBR[countB-((countB > delayB)?delayB+1:0)];
			double outCR = aCR[countC-((countC > delayC)?delayC+1:0)];
			double outDR = aDR[countD-((countD > delayD)?delayD+1:0)];
			//second block: four more outputs
			
			aEL[countE] = (outAL - (outBL + outCL + outDL));
			aFL[countF] = (outBL - (outAL + outCL + outDL));
			aGL[countG] = (outCL - (outAL + outBL + outDL));
			aHL[countH] = (outDL - (outAL + outBL + outCL));
			aER[countE] = (outAR - (outBR + outCR + outDR));
			aFR[countF] = (outBR - (outAR + outCR + outDR));
			aGR[countG] = (outCR - (outAR + outBR + outDR));
			aHR[countH] = (outDR - (outAR + outBR + outCR));
			
			countE++; if (countE < 0 || countE > delayE) countE = 0;
			countF++; if (countF < 0 || countF > delayF) countF = 0;
			countG++; if (countG < 0 || countG > delayG) countG = 0;
			countH++; if (countH < 0 || countH > delayH) countH = 0;
			
			double outEL = aEL[countE-((countE > delayE)?delayE+1:0)];
			double outFL = aFL[countF-((countF > delayF)?delayF+1:0)];
			double outGL = aGL[countG-((countG > delayG)?delayG+1:0)];
			double outHL = aHL[countH-((countH > delayH)?delayH+1:0)];
			double outER = aER[countE-((countE > delayE)?delayE+1:0)];
			double outFR = aFR[countF-((countF > delayF)?delayF+1:0)];
			double outGR = aGR[countG-((countG > delayG)?delayG+1:0)];
			double outHR = aHR[countH-((countH > delayH)?delayH+1:0)];
			//third block: final outputs
			
			feedbackAR = (outEL - (outFL + outGL + outHL));
			feedbackBL = (outFL - (outEL + outGL + outHL));
			feedbackCR = (outGL - (outEL + outFL + outHL));
			feedbackDL = (outHL - (outEL + outFL + outGL));
			feedbackAL = (outER - (outFR + outGR + outHR));
			feedbackBR = (outFR - (outER + outGR + outHR));
			feedbackCL = (outGR - (outER + outFR + outHR));
			feedbackDR = (outHR - (outER + outFR + outGR));
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outEL + outFL + outGL + outHL)/8.0;
			inputSampleR = (outER + outFR + outGR + outHR)/8.0;
			//and take the final combined sum of outputs
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
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[7])*0.5;
				lastRefL[7] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[7])*0.5;
				lastRefR[7] = lastRefR[8]; //continue, do not break
			case 3:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[6])*0.5;
				lastRefL[6] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[6])*0.5;
				lastRefR[6] = lastRefR[8]; //continue, do not break
			case 2:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[5])*0.5;
				lastRefL[5] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[5])*0.5;
				lastRefR[5] = lastRefR[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (wet < 1.0) {inputSampleL *= wet; inputSampleR *= wet;}
		if (dry < 1.0) {drySampleL *= dry; drySampleR *= dry;}
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		//this is our submix verb dry/wet: 0.5 is BOTH at FULL VOLUME
		//purpose is that, if you're adding verb, you're not altering other balances
		
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
	for(int count = 0; count < 9999; count++) {aEL[count] = 0.0;aER[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aFL[count] = 0.0;aFR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aGL[count] = 0.0;aGR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aHL[count] = 0.0;aHR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aAL[count] = 0.0;aAR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aBL[count] = 0.0;aBR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aCL[count] = 0.0;aCR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aDL[count] = 0.0;aDR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aIL[count] = 0.0;aIR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aJL[count] = 0.0;aJR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aKL[count] = 0.0;aKR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aLL[count] = 0.0;aLR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {aML[count] = 0.0;aMR[count] = 0.0;}
	
	feedbackAL = 0.0; feedbackAR = 0.0;
	feedbackBL = 0.0; feedbackBR = 0.0;
	feedbackCL = 0.0; feedbackCR = 0.0;
	feedbackDL = 0.0; feedbackDR = 0.0;
	previousAL = 0.0; previousAR = 0.0;
	previousBL = 0.0; previousBR = 0.0;
	previousCL = 0.0; previousCR = 0.0;
	previousDL = 0.0; previousDR = 0.0;
	
	for(int count = 0; count < 9; count++) {lastRefL[count] = 0.0;lastRefR[count] = 0.0;}
	
	countI = 1;
	countJ = 1;
	countK = 1;
	countL = 1;
	countM = 1;
	
	countA = 1;
	countB = 1;
	countC = 1;
	countD = 1;	
	
	countE = 1;
	countF = 1;
	countG = 1;
	countH = 1;
	cycle = 0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
