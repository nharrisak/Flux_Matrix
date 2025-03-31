#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "kCathedral"
#define AIRWINDOWS_DESCRIPTION "A giant reverby space."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','k','C','a' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
const int earlyA = 437; const int earlyB = 284; const int earlyC = 754; const int earlyD = 1538; const int earlyE = 59; const int earlyF = 1829; const int earlyG = 37; const int earlyH = 3304; const int earlyI = 200; const int predelay = 1014; const int vlfpredelay = 3275; //6 to 151 ms, 741 seat theater. Scarcity, 1 in 17259
const int delayA = 253; const int delayB = 1395; const int delayC = 248; const int delayD = 284; const int delayE = 952; const int delayF = 430; const int delayG = 1253; const int delayH = 889; const int delayI = 798; const int delayJ = 397; const int delayK = 1166; const int delayL = 250; const int delayM = 38; const int delayN = 1389; const int delayO = 1103; const int delayP = 50; const int delayQ = 1317; const int delayR = 40; const int delayS = 1393; const int delayT = 325; const int delayU = 11; const int delayV = 265; const int delayW = 1339; const int delayX = 315; const int delayY = 753; //16 to 153 ms, 860 seat hall. Scarcity, 1 in 60182
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Wetness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double gainOutL;
 	double gainOutR;
	
	
	
	int earlyAL, earlyAR;
	int earlyBL, earlyBR;
	int earlyCL, earlyCR;
	int earlyDL, earlyDR;
	int earlyEL, earlyER;
	int earlyFL, earlyFR;
	int earlyGL, earlyGR;
	int earlyHL, earlyHR;
	int earlyIL, earlyIR;
	
	
	
	
	
	
	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double feedbackEL;
	
	double feedbackER;
	double feedbackJR;
	double feedbackOR;
	double feedbackTR;
	double feedbackYR;
		
	double lastRefL[7];
	double lastRefR[7];
	
	int countAL;
	int countBL;
	int countCL;
	int countDL;
	int countEL;
	int countFL;
	int countGL;
	int countHL;
	int countIL;
	int countJL;
	int countKL;
	int countLL;		
	int countML;		
	int countNL;		
	int countOL;		
	int countPL;		
	int countQL;		
	int countRL;		
	int countSL;		
	int countTL;		
	int countUL;		
	int countVL;		
	int countWL;		
	int countXL;		
	int countYL;		
	
	int countAR;
	int countBR;
	int countCR;
	int countDR;
	int countER;
	int countFR;
	int countGR;
	int countHR;
	int countIR;
	int countJR;
	int countKR;
	int countLR;		
	int countMR;		
	int countNR;		
	int countOR;		
	int countPR;		
	int countQR;		
	int countRR;		
	int countSR;		
	int countTR;		
	int countUR;		
	int countVR;		
	int countWR;		
	int countXR;		
	int countYR;
	
	int countZ;		
	int countVLF;		
	
	int cycle;
	
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		prevSampL2,
		prevSlewL2,
		prevSampR2,
		prevSlewR2,
		prevSampL3,
		prevSlewL3,
		prevSampR3,
		prevSlewR3,
		prevSampL4,
		prevSlewL4,
		prevSampR4,
		prevSlewR4,
		prevSampL5,
		prevSlewL5,
		prevSampR5,
		prevSlewR5,
		prevSampL6,
		prevSlewL6,
		prevSampR6,
		prevSlewR6,
		prevSampL7,
		prevSlewL7,
		prevSampR7,
		prevSlewR7,
		prevSampL8,
		prevSlewL8,
		prevSampR8,
		prevSlewR8,
		prevSampL9,
		prevSlewL9,
		prevSampR9,
		prevSlewR9,
		prevSampL10,
		prevSlewL10,
		prevSampR10,
		prevSlewR10,
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	
		
	double subAL;
	double subAR;
	double subBL;
	double subBR;
	double subCL;
	double subCR;
	
	double prevMulchBL;
	double prevMulchBR;
	double prevMulchCL;
	double prevMulchCR;
	double prevMulchDL;
	double prevMulchDR;
	double prevMulchEL;
	double prevMulchER;
	
	double prevOutEL;
	double prevOutER;
	double prevInEL;
	double prevInER;
	
	double tailL;
	double tailR;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double eAL[earlyA+5];
	double eBL[earlyB+5];
	double eCL[earlyC+5];
	double eDL[earlyD+5];
	double eEL[earlyE+5];
	double eFL[earlyF+5];
	double eGL[earlyG+5];
	double eHL[earlyH+5];
	double eIL[earlyI+5];
	double eAR[earlyA+5];
	double eBR[earlyB+5];
	double eCR[earlyC+5];
	double eDR[earlyD+5];
	double eER[earlyE+5];
	double eFR[earlyF+5];
	double eGR[earlyG+5];
	double eHR[earlyH+5];
	double eIR[earlyI+5];
	double aAL[delayA+5];
	double aBL[delayB+5];
	double aCL[delayC+5];
	double aDL[delayD+5];
	double aEL[delayE+5];
	double aFL[delayF+5];
	double aGL[delayG+5];
	double aHL[delayH+5];
	double aIL[delayI+5];
	double aJL[delayJ+5];
	double aKL[delayK+5];
	double aLL[delayL+5];
	double aML[delayM+5];
	double aNL[delayN+5];
	double aOL[delayO+5];
	double aPL[delayP+5];
	double aQL[delayQ+5];
	double aRL[delayR+5];
	double aSL[delayS+5];
	double aTL[delayT+5];
	double aUL[delayU+5];
	double aVL[delayV+5];
	double aWL[delayW+5];
	double aXL[delayX+5];
	double aYL[delayY+5];
	double aAR[delayA+5];
	double aBR[delayB+5];
	double aCR[delayC+5];
	double aDR[delayD+5];
	double aER[delayE+5];
	double aFR[delayF+5];
	double aGR[delayG+5];
	double aHR[delayH+5];
	double aIR[delayI+5];
	double aJR[delayJ+5];
	double aKR[delayK+5];
	double aLR[delayL+5];
	double aMR[delayM+5];
	double aNR[delayN+5];
	double aOR[delayO+5];
	double aPR[delayP+5];
	double aQR[delayQ+5];
	double aRR[delayR+5];
	double aSR[delayS+5];
	double aTR[delayT+5];
	double aUR[delayU+5];
	double aVR[delayV+5];
	double aWR[delayW+5];
	double aXR[delayX+5];
	double aYR[delayY+5];
	double aZL[predelay+5];
	double aZR[predelay+5];
	double aVLFL[vlfpredelay+5];
	double aVLFR[vlfpredelay+5];
	double pearA[pear_total]; //probably worth just using a number here
	double pearB[pear_total]; //probably worth just using a number here
	double pearC[pear_total]; //probably worth just using a number here
	};
	_dram* dram;
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
	int adjPredelay = predelay;
	int adjSubDelay = vlfpredelay;
	double wet = GetParameter( kParam_One )*2.0;
	double dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			double outSample;
			outSample = (inputSampleL + prevInEL)*0.5;
			prevInEL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + prevInER)*0.5;
			prevInER = inputSampleR; inputSampleR = outSample;
			
			//predelay
			dram->aZL[countZ] = inputSampleL;
			dram->aZR[countZ] = inputSampleR;
			countZ++; if (countZ < 0 || countZ > adjPredelay) countZ = 0;
			inputSampleL = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			inputSampleR = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay
			
			//begin SubTight section
			double outSampleL = inputSampleL * 0.0026856;
			double outSampleR = inputSampleR * 0.0026856;
			double scale = 0.5+fabs(outSampleL*0.5);
			outSampleL = (subAL+(sin(subAL-outSampleL)*scale));
			subAL = outSampleL*scale;
			scale = 0.5+fabs(outSampleR*0.5);
			outSampleR = (subAR+(sin(subAR-outSampleR)*scale));
			subAR = outSampleR*scale;
			scale = 0.5+fabs(outSampleL*0.5);
			outSampleL = (subBL+(sin(subBL-outSampleL)*scale));
			subBL = outSampleL*scale;
			scale = 0.5+fabs(outSampleR*0.5);
			outSampleR = (subBR+(sin(subBR-outSampleR)*scale));
			subBR = outSampleR*scale;
			scale = 0.5+fabs(outSampleL*0.5);
			outSampleL = (subCL+(sin(subCL-outSampleL)*scale));
			subCL = outSampleL*scale;
			scale = 0.5+fabs(outSampleR*0.5);
			outSampleR = (subCR+(sin(subCR-outSampleR)*scale));
			subCR = outSampleR*scale;
			outSampleL = -outSampleL; outSampleR = -outSampleR;
			if (outSampleL > 0.25) outSampleL = 0.25; if (outSampleL < -0.25) outSampleL = -0.25;
			if (outSampleR > 0.25) outSampleR = 0.25; if (outSampleR < -0.25) outSampleR = -0.25;
			outSampleL *= 16.0;
			outSampleR *= 16.0;
			inputSampleL -= outSampleL;
			inputSampleR -= outSampleR;
			//end SubTight section
			
			//VLF predelay
			dram->aVLFL[countVLF] = outSampleL;
			dram->aVLFR[countVLF] = outSampleR;
			countVLF++; if (countVLF < 0 || countVLF > adjSubDelay) countVLF = 0;
			outSampleL = dram->aVLFL[countVLF-((countVLF > adjSubDelay)?adjSubDelay+1:0)] * 2.0;
			outSampleR = dram->aVLFR[countVLF-((countVLF > adjSubDelay)?adjSubDelay+1:0)] * 2.0;
			//end VLF predelay
			
			//begin with early reflections 
			dram->eAL[earlyAL] = inputSampleL;
			dram->eBL[earlyBL] = inputSampleL;
			dram->eCL[earlyCL] = inputSampleL;
			dram->eCR[earlyCR] = inputSampleR;
			dram->eFR[earlyFR] = inputSampleR;
			dram->eIR[earlyIR] = inputSampleR;
			
			earlyAL++; if (earlyAL < 0 || earlyAL > earlyA) earlyAL = 0;
			earlyBL++; if (earlyBL < 0 || earlyBL > earlyB) earlyBL = 0;
			earlyCL++; if (earlyCL < 0 || earlyCL > earlyC) earlyCL = 0;
			earlyCR++; if (earlyCR < 0 || earlyCR > earlyC) earlyCR = 0;
			earlyFR++; if (earlyFR < 0 || earlyFR > earlyF) earlyFR = 0;
			earlyIR++; if (earlyIR < 0 || earlyIR > earlyI) earlyIR = 0;
			
			double oeAL = dram->eAL[earlyAL-((earlyAL > earlyA)?earlyA+1:0)];
			double oeBL = dram->eBL[earlyBL-((earlyBL > earlyB)?earlyB+1:0)];
			double oeCL = dram->eCL[earlyCL-((earlyCL > earlyC)?earlyC+1:0)];
			double oeCR = dram->eCR[earlyCR-((earlyCR > earlyC)?earlyC+1:0)];
			double oeFR = dram->eFR[earlyFR-((earlyFR > earlyF)?earlyF+1:0)];
			double oeIR = dram->eIR[earlyIR-((earlyIR > earlyI)?earlyI+1:0)];
			
			dram->eDL[earlyDL] = ((oeBL + oeCL) - oeAL);
			dram->eEL[earlyEL] = ((oeAL + oeCL) - oeBL);
			dram->eFL[earlyFL] = ((oeAL + oeBL) - oeCL);
			dram->eBR[earlyBR] = ((oeFR + oeIR) - oeCR);
			dram->eER[earlyER] = ((oeCR + oeIR) - oeFR);
			dram->eHR[earlyHR] = ((oeCR + oeFR) - oeIR);
			
			earlyDL++; if (earlyDL < 0 || earlyDL > earlyD) earlyDL = 0;
			earlyEL++; if (earlyEL < 0 || earlyEL > earlyE) earlyEL = 0;
			earlyFL++; if (earlyFL < 0 || earlyFL > earlyF) earlyFL = 0;
			earlyBR++; if (earlyBR < 0 || earlyBR > earlyB) earlyBR = 0;
			earlyER++; if (earlyER < 0 || earlyER > earlyE) earlyER = 0;
			earlyHR++; if (earlyHR < 0 || earlyHR > earlyH) earlyHR = 0;
			
			double oeDL = dram->eDL[earlyDL-((earlyDL > earlyD)?earlyD+1:0)];
			double oeEL = dram->eEL[earlyEL-((earlyEL > earlyE)?earlyE+1:0)];
			double oeFL = dram->eFL[earlyFL-((earlyFL > earlyF)?earlyF+1:0)];
			double oeBR = dram->eBR[earlyBR-((earlyBR > earlyB)?earlyB+1:0)];
			double oeER = dram->eER[earlyER-((earlyER > earlyE)?earlyE+1:0)];
			double oeHR = dram->eHR[earlyHR-((earlyHR > earlyH)?earlyH+1:0)];
			
			dram->eGL[earlyGL] = ((oeEL + oeFL) - oeDL);
			dram->eHL[earlyHL] = ((oeDL + oeFL) - oeEL);
			dram->eIL[earlyIL] = ((oeDL + oeEL) - oeFL);
			dram->eAR[earlyAR] = ((oeER + oeHR) - oeBR);
			dram->eDR[earlyDR] = ((oeBR + oeHR) - oeER);
			dram->eGR[earlyGR] = ((oeBR + oeER) - oeHR);
			
			earlyGL++; if (earlyGL < 0 || earlyGL > earlyG) earlyGL = 0;
			earlyHL++; if (earlyHL < 0 || earlyHL > earlyH) earlyHL = 0;
			earlyIL++; if (earlyIL < 0 || earlyIL > earlyI) earlyIL = 0;
			earlyAR++; if (earlyAR < 0 || earlyAR > earlyA) earlyAR = 0;
			earlyDR++; if (earlyDR < 0 || earlyDR > earlyD) earlyDR = 0;
			earlyGR++; if (earlyGR < 0 || earlyGR > earlyG) earlyGR = 0;
			
			double oeGL = dram->eGL[earlyGL-((earlyGL > earlyG)?earlyG+1:0)];
			double oeHL = dram->eHL[earlyHL-((earlyHL > earlyH)?earlyH+1:0)];
			double oeIL = dram->eIL[earlyIL-((earlyIL > earlyI)?earlyI+1:0)];
			double oeAR = dram->eAR[earlyAR-((earlyAR > earlyA)?earlyA+1:0)];
			double oeDR = dram->eDR[earlyDR-((earlyDR > earlyD)?earlyD+1:0)];
			double oeGR = dram->eGR[earlyGR-((earlyGR > earlyG)?earlyG+1:0)];
			
			double earlyReflectionsL = oeGL + oeHL + oeIL;
			double earlyReflectionsR = oeAR + oeDR + oeGR;
			
			inputSampleL += outSampleL;
			inputSampleR += outSampleR;
			//having re-added our VLF delayed channel we can now re-use outSample
			
			dram->aAL[countAL] = inputSampleL + (feedbackAL * 0.000293);
			dram->aBL[countBL] = inputSampleL + (feedbackBL * 0.000293);
			dram->aCL[countCL] = inputSampleL + (feedbackCL * 0.000293);
			dram->aDL[countDL] = inputSampleL + (feedbackDL * 0.000293);
			dram->aEL[countEL] = inputSampleL + (feedbackEL * 0.000293);
			
			dram->aER[countER] = inputSampleR + (feedbackER * 0.000293);
			dram->aJR[countJR] = inputSampleR + (feedbackJR * 0.000293);
			dram->aOR[countOR] = inputSampleR + (feedbackOR * 0.000293);
			dram->aTR[countTR] = inputSampleR + (feedbackTR * 0.000293);
			dram->aYR[countYR] = inputSampleR + (feedbackYR * 0.000293);
			
			countAL++; if (countAL < 0 || countAL > delayA) countAL = 0;
			countBL++; if (countBL < 0 || countBL > delayB) countBL = 0;
			countCL++; if (countCL < 0 || countCL > delayC) countCL = 0;
			countDL++; if (countDL < 0 || countDL > delayD) countDL = 0;
			countEL++; if (countEL < 0 || countEL > delayE) countEL = 0;
			
			countER++; if (countER < 0 || countER > delayE) countER = 0;
			countJR++; if (countJR < 0 || countJR > delayJ) countJR = 0;
			countOR++; if (countOR < 0 || countOR > delayO) countOR = 0;
			countTR++; if (countTR < 0 || countTR > delayT) countTR = 0;
			countYR++; if (countYR < 0 || countYR > delayY) countYR = 0;
			
			double outAL = dram->aAL[countAL-((countAL > delayA)?delayA+1:0)];
			double outBL = dram->aBL[countBL-((countBL > delayB)?delayB+1:0)];
			double outCL = dram->aCL[countCL-((countCL > delayC)?delayC+1:0)];
			double outDL = dram->aDL[countDL-((countDL > delayD)?delayD+1:0)];
			double outEL = dram->aEL[countEL-((countEL > delayE)?delayE+1:0)];
			
			double outER = dram->aER[countER-((countER > delayE)?delayE+1:0)];
			double outJR = dram->aJR[countJR-((countJR > delayJ)?delayJ+1:0)];
			double outOR = dram->aOR[countOR-((countOR > delayO)?delayO+1:0)];
			double outTR = dram->aTR[countTR-((countTR > delayT)?delayT+1:0)];
			double outYR = dram->aYR[countYR-((countYR > delayY)?delayY+1:0)];
			
			//-------- one
			
			for (int x = 0; x < 31.32; x += 4) {
				double slew = ((outAL - dram->pearA[x]) + dram->pearA[x+1])*0.304*0.5;
				dram->pearA[x] = outAL = (0.304 * outAL) + ((1.0-0.304) * (dram->pearA[x] + dram->pearA[x+1]));
				dram->pearA[x+1] = slew;
				slew = ((outER - dram->pearA[x+2]) + dram->pearA[x+3])*0.304*0.5;
				dram->pearA[x+2] = outER = (0.304 * outER) + ((1.0-0.304) * (dram->pearA[x+2] + dram->pearA[x+3]));
				dram->pearA[x+3] = slew;
			}
			
			dram->aFL[countFL] = ((outAL*3.0) - ((outBL + outCL + outDL + outEL)*2.0));
			dram->aGL[countGL] = ((outBL*3.0) - ((outAL + outCL + outDL + outEL)*2.0));
			dram->aHL[countHL] = ((outCL*3.0) - ((outAL + outBL + outDL + outEL)*2.0));
			dram->aIL[countIL] = ((outDL*3.0) - ((outAL + outBL + outCL + outEL)*2.0));
			dram->aJL[countJL] = ((outEL*3.0) - ((outAL + outBL + outCL + outDL)*2.0));
			
			dram->aDR[countDR] = ((outER*3.0) - ((outJR + outOR + outTR + outYR)*2.0));
			dram->aIR[countIR] = ((outJR*3.0) - ((outER + outOR + outTR + outYR)*2.0));
			dram->aNR[countNR] = ((outOR*3.0) - ((outER + outJR + outTR + outYR)*2.0));
			dram->aSR[countSR] = ((outTR*3.0) - ((outER + outJR + outOR + outYR)*2.0));
			dram->aXR[countXR] = ((outYR*3.0) - ((outER + outJR + outOR + outTR)*2.0));
			
			countFL++; if (countFL < 0 || countFL > delayF) countFL = 0;
			countGL++; if (countGL < 0 || countGL > delayG) countGL = 0;
			countHL++; if (countHL < 0 || countHL > delayH) countHL = 0;
			countIL++; if (countIL < 0 || countIL > delayI) countIL = 0;
			countJL++; if (countJL < 0 || countJL > delayJ) countJL = 0;
			
			countDR++; if (countDR < 0 || countDR > delayD) countDR = 0;
			countIR++; if (countIR < 0 || countIR > delayI) countIR = 0;
			countNR++; if (countNR < 0 || countNR > delayN) countNR = 0;
			countSR++; if (countSR < 0 || countSR > delayS) countSR = 0;
			countXR++; if (countXR < 0 || countXR > delayX) countXR = 0;
			
			double outFL = dram->aFL[countFL-((countFL > delayF)?delayF+1:0)];
			double outGL = dram->aGL[countGL-((countGL > delayG)?delayG+1:0)];
			double outHL = dram->aHL[countHL-((countHL > delayH)?delayH+1:0)];
			double outIL = dram->aIL[countIL-((countIL > delayI)?delayI+1:0)];
			double outJL = dram->aJL[countJL-((countJL > delayJ)?delayJ+1:0)];
			
			double outDR = dram->aDR[countDR-((countDR > delayD)?delayD+1:0)];
			double outIR = dram->aIR[countIR-((countIR > delayI)?delayI+1:0)];
			double outNR = dram->aNR[countNR-((countNR > delayN)?delayN+1:0)];
			double outSR = dram->aSR[countSR-((countSR > delayS)?delayS+1:0)];
			double outXR = dram->aXR[countXR-((countXR > delayX)?delayX+1:0)];
			
			//-------- mulch
			
			for (int x = 0; x < 31.32; x += 4) {
				double slew = ((outFL - dram->pearB[x]) + dram->pearB[x+1])*0.566*0.5;
				dram->pearB[x] = outFL = (0.566 * outFL) + ((1.0-0.566) * (dram->pearB[x] + dram->pearB[x+1]));
				dram->pearB[x+1] = slew;
				slew = ((outDR - dram->pearB[x+2]) + dram->pearB[x+3])*0.566*0.5;
				dram->pearB[x+2] = outDR = (0.566 * outDR) + ((1.0-0.566) * (dram->pearB[x+2] + dram->pearB[x+3]));
				dram->pearB[x+3] = slew;
			}
			
			outSample = (outGL + prevMulchBL)*0.5;
			prevMulchBL = outGL; outGL = outSample;
			
			outSample = (outIR + prevMulchBR)*0.5;
			prevMulchBR = outIR; outIR = outSample;
			
			//-------- two
			
			dram->aKL[countKL] = ((outFL*3.0) - ((outGL + outHL + outIL + outJL)*2.0));
			dram->aLL[countLL] = ((outGL*3.0) - ((outFL + outHL + outIL + outJL)*2.0));
			dram->aML[countML] = ((outHL*3.0) - ((outFL + outGL + outIL + outJL)*2.0));
			dram->aNL[countNL] = ((outIL*3.0) - ((outFL + outGL + outHL + outJL)*2.0));
			dram->aOL[countOL] = ((outJL*3.0) - ((outFL + outGL + outHL + outIL)*2.0));
			
			dram->aCR[countCR] = ((outDR*3.0) - ((outIR + outNR + outSR + outXR)*2.0));
			dram->aHR[countHR] = ((outIR*3.0) - ((outDR + outNR + outSR + outXR)*2.0));
			dram->aMR[countMR] = ((outNR*3.0) - ((outDR + outIR + outSR + outXR)*2.0));
			dram->aRR[countRR] = ((outSR*3.0) - ((outDR + outIR + outNR + outXR)*2.0));
			dram->aWR[countWR] = ((outXR*3.0) - ((outDR + outIR + outNR + outSR)*2.0));
			
			countKL++; if (countKL < 0 || countKL > delayK) countKL = 0;
			countLL++; if (countLL < 0 || countLL > delayL) countLL = 0;
			countML++; if (countML < 0 || countML > delayM) countML = 0;
			countNL++; if (countNL < 0 || countNL > delayN) countNL = 0;
			countOL++; if (countOL < 0 || countOL > delayO) countOL = 0;
			
			countCR++; if (countCR < 0 || countCR > delayC) countCR = 0;
			countHR++; if (countHR < 0 || countHR > delayH) countHR = 0;
			countMR++; if (countMR < 0 || countMR > delayM) countMR = 0;
			countRR++; if (countRR < 0 || countRR > delayR) countRR = 0;
			countWR++; if (countWR < 0 || countWR > delayW) countWR = 0;
			
			double outKL = dram->aKL[countKL-((countKL > delayK)?delayK+1:0)];
			double outLL = dram->aLL[countLL-((countLL > delayL)?delayL+1:0)];
			double outML = dram->aML[countML-((countML > delayM)?delayM+1:0)];
			double outNL = dram->aNL[countNL-((countNL > delayN)?delayN+1:0)];
			double outOL = dram->aOL[countOL-((countOL > delayO)?delayO+1:0)];
			
			double outCR = dram->aCR[countCR-((countCR > delayC)?delayC+1:0)];
			double outHR = dram->aHR[countHR-((countHR > delayH)?delayH+1:0)];
			double outMR = dram->aMR[countMR-((countMR > delayM)?delayM+1:0)];
			double outRR = dram->aRR[countRR-((countRR > delayR)?delayR+1:0)];
			double outWR = dram->aWR[countWR-((countWR > delayW)?delayW+1:0)];
			
			//-------- mulch
			
			for (int x = 0; x < 31.32; x += 4) {
				double slew = ((outKL - dram->pearC[x]) + dram->pearC[x+1])*0.416*0.5;
				dram->pearC[x] = outKL = (0.416 * outKL) + ((1.0-0.416) * (dram->pearC[x] + dram->pearC[x+1]));
				dram->pearC[x+1] = slew;
				slew = ((outCR - dram->pearC[x+2]) + dram->pearC[x+3])*0.416*0.5;
				dram->pearC[x+2] = outCR = (0.416 * outCR) + ((1.0-0.416) * (dram->pearC[x+2] + dram->pearC[x+3]));
				dram->pearC[x+3] = slew;
			}
			
			outSample = (outLL + prevMulchCL)*0.5;
			prevMulchCL = outLL; outLL = outSample;
			
			outSample = (outHR + prevMulchCR)*0.5;
			prevMulchCR = outHR; outHR = outSample;
			
			//-------- three
			
			dram->aPL[countPL] = ((outKL*3.0) - ((outLL + outML + outNL + outOL)*2.0));
			dram->aQL[countQL] = ((outLL*3.0) - ((outKL + outML + outNL + outOL)*2.0));
			dram->aRL[countRL] = ((outML*3.0) - ((outKL + outLL + outNL + outOL)*2.0));
			dram->aSL[countSL] = ((outNL*3.0) - ((outKL + outLL + outML + outOL)*2.0));
			dram->aTL[countTL] = ((outOL*3.0) - ((outKL + outLL + outML + outNL)*2.0));
			
			dram->aBR[countBR] = ((outCR*3.0) - ((outHR + outMR + outRR + outWR)*2.0));
			dram->aGR[countGR] = ((outHR*3.0) - ((outCR + outMR + outRR + outWR)*2.0));
			dram->aLR[countLR] = ((outMR*3.0) - ((outCR + outHR + outRR + outWR)*2.0));
			dram->aQR[countQR] = ((outRR*3.0) - ((outCR + outHR + outMR + outWR)*2.0));
			dram->aVR[countVR] = ((outWR*3.0) - ((outCR + outHR + outMR + outRR)*2.0));
			
			countPL++; if (countPL < 0 || countPL > delayP) countPL = 0;
			countQL++; if (countQL < 0 || countQL > delayQ) countQL = 0;
			countRL++; if (countRL < 0 || countRL > delayR) countRL = 0;
			countSL++; if (countSL < 0 || countSL > delayS) countSL = 0;
			countTL++; if (countTL < 0 || countTL > delayT) countTL = 0;
			
			countBR++; if (countBR < 0 || countBR > delayB) countBR = 0;
			countGR++; if (countGR < 0 || countGR > delayG) countGR = 0;
			countLR++; if (countLR < 0 || countLR > delayL) countLR = 0;
			countQR++; if (countQR < 0 || countQR > delayQ) countQR = 0;
			countVR++; if (countVR < 0 || countVR > delayV) countVR = 0;
			
			double outPL = dram->aPL[countPL-((countPL > delayP)?delayP+1:0)];
			double outQL = dram->aQL[countQL-((countQL > delayQ)?delayQ+1:0)];
			double outRL = dram->aRL[countRL-((countRL > delayR)?delayR+1:0)];
			double outSL = dram->aSL[countSL-((countSL > delayS)?delayS+1:0)];
			double outTL = dram->aTL[countTL-((countTL > delayT)?delayT+1:0)];
			
			double outBR = dram->aBR[countBR-((countBR > delayB)?delayB+1:0)];
			double outGR = dram->aGR[countGR-((countGR > delayG)?delayG+1:0)];
			double outLR = dram->aLR[countLR-((countLR > delayL)?delayL+1:0)];
			double outQR = dram->aQR[countQR-((countQR > delayQ)?delayQ+1:0)];
			double outVR = dram->aVR[countVR-((countVR > delayV)?delayV+1:0)];
			
			outSample = (outQL + prevMulchDL)*0.5;
			prevMulchDL = outQL; outQL = outSample;
			
			outSample = (outGR + prevMulchDR)*0.5;
			prevMulchDR = outGR; outGR = outSample;
			
			//-------- four
			
			dram->aUL[countUL] = ((outPL*3.0) - ((outQL + outRL + outSL + outTL)*2.0));
			dram->aVL[countVL] = ((outQL*3.0) - ((outPL + outRL + outSL + outTL)*2.0));
			dram->aWL[countWL] = ((outRL*3.0) - ((outPL + outQL + outSL + outTL)*2.0));
			dram->aXL[countXL] = ((outSL*3.0) - ((outPL + outQL + outRL + outTL)*2.0));
			dram->aYL[countYL] = ((outTL*3.0) - ((outPL + outQL + outRL + outSL)*2.0));
			
			dram->aAR[countAR] = ((outBR*3.0) - ((outGR + outLR + outQR + outVR)*2.0));
			dram->aFR[countFR] = ((outGR*3.0) - ((outBR + outLR + outQR + outVR)*2.0));
			dram->aKR[countKR] = ((outLR*3.0) - ((outBR + outGR + outQR + outVR)*2.0));
			dram->aPR[countPR] = ((outQR*3.0) - ((outBR + outGR + outLR + outVR)*2.0));
			dram->aUR[countUR] = ((outVR*3.0) - ((outBR + outGR + outLR + outQR)*2.0));
			
			countUL++; if (countUL < 0 || countUL > delayU) countUL = 0;
			countVL++; if (countVL < 0 || countVL > delayV) countVL = 0;
			countWL++; if (countWL < 0 || countWL > delayW) countWL = 0;
			countXL++; if (countXL < 0 || countXL > delayX) countXL = 0;
			countYL++; if (countYL < 0 || countYL > delayY) countYL = 0;
			
			countAR++; if (countAR < 0 || countAR > delayA) countAR = 0;
			countFR++; if (countFR < 0 || countFR > delayF) countFR = 0;
			countKR++; if (countKR < 0 || countKR > delayK) countKR = 0;
			countPR++; if (countPR < 0 || countPR > delayP) countPR = 0;
			countUR++; if (countUR < 0 || countUR > delayU) countUR = 0;
			
			double outUL = dram->aUL[countUL-((countUL > delayU)?delayU+1:0)];
			double outVL = dram->aVL[countVL-((countVL > delayV)?delayV+1:0)];
			double outWL = dram->aWL[countWL-((countWL > delayW)?delayW+1:0)];
			double outXL = dram->aXL[countXL-((countXL > delayX)?delayX+1:0)];
			double outYL = dram->aYL[countYL-((countYL > delayY)?delayY+1:0)];
			
			double outAR = dram->aAR[countAR-((countAR > delayA)?delayA+1:0)];
			double outFR = dram->aFR[countFR-((countFR > delayF)?delayF+1:0)];
			double outKR = dram->aKR[countKR-((countKR > delayK)?delayK+1:0)];
			double outPR = dram->aPR[countPR-((countPR > delayP)?delayP+1:0)];
			double outUR = dram->aUR[countUR-((countUR > delayU)?delayU+1:0)];
			
			//-------- mulch
			
			outSample = (outVL + prevMulchEL)*0.5;
			prevMulchEL = outVL; outVL = outSample;
			
			outSample = (outFR + prevMulchER)*0.5;
			prevMulchER = outFR; outFR = outSample;
			
			//-------- five
			
			feedbackER = ((outUL*3.0) - ((outVL + outWL + outXL + outYL)*2.0));
			feedbackAL = ((outAR*3.0) - ((outFR + outKR + outPR + outUR)*2.0));
			feedbackJR = ((outVL*3.0) - ((outUL + outWL + outXL + outYL)*2.0));
			feedbackBL = ((outFR*3.0) - ((outAR + outKR + outPR + outUR)*2.0));
			feedbackCL = ((outWL*3.0) - ((outUL + outVL + outXL + outYL)*2.0));
			feedbackOR = ((outKR*3.0) - ((outAR + outFR + outPR + outUR)*2.0));
			feedbackDL = ((outXL*3.0) - ((outUL + outVL + outWL + outYL)*2.0));
			feedbackTR = ((outPR*3.0) - ((outAR + outFR + outKR + outUR)*2.0));
			feedbackEL = ((outYL*3.0) - ((outUL + outVL + outWL + outXL)*2.0));
			feedbackYR = ((outUR*3.0) - ((outAR + outFR + outKR + outPR)*2.0));
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outUL + outVL + outWL + outXL + outYL)*0.0016;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR)*0.0016;
			//and take the final combined sum of outputs, corrected for Householder gain
			
			inputSampleL += (earlyReflectionsL*0.2);
			inputSampleR += (earlyReflectionsR*0.2);
			
			inputSampleL *= 0.25; inputSampleR *= 0.25;
			if (gainOutL < 0.0078125) gainOutL = 0.0078125; if (gainOutL > 1.0) gainOutL = 1.0;
			if (gainOutR < 0.0078125) gainOutR = 0.0078125; if (gainOutR > 1.0) gainOutR = 1.0;
			//gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
			//smaller number is maximum clamping, if too small it'll take a while to bounce back
			inputSampleL *= gainOutL; inputSampleR *= gainOutR;
			gainOutL += sin((fabs(inputSampleL*4)>1)?4:fabs(inputSampleL*4))*pow(inputSampleL,4);
			gainOutR += sin((fabs(inputSampleR*4)>1)?4:fabs(inputSampleR*4))*pow(inputSampleR,4);
			//4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
			inputSampleL *= 4.0; inputSampleR *= 4.0;
			//curve! To get a compressed effect that matches a certain other plugin
			//that is too overprocessed for its own good :)	
			
			outSample = (inputSampleL + prevOutEL)*0.5;
			prevOutEL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + prevOutER)*0.5;
			prevOutER = inputSampleR; inputSampleR = outSample;
			
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
		
		inputSampleL *= 0.25; inputSampleR *= 0.25;
		if (inputSampleL > 2.8) inputSampleL = 2.8;
		if (inputSampleL < -2.8) inputSampleL = -2.8;
		if (inputSampleR > 2.8) inputSampleR = 2.8;
		if (inputSampleR < -2.8) inputSampleR = -2.8;//clip BigFastArcSin harder
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL*2.0)/(2.8274333882308-inputSampleL);
		else inputSampleL = -(inputSampleL*-2.0)/(2.8274333882308+inputSampleL);
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR*2.0)/(2.8274333882308-inputSampleR);
		else inputSampleR = -(inputSampleR*-2.0)/(2.8274333882308+inputSampleR);
		//BigFastArcSin output stage
		
		if (cycleEnd > 1) {
			double outSample = (inputSampleL + tailL)*0.5;
			tailL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + tailR)*0.5;
			tailR = inputSampleR; inputSampleR = outSample;
		} //let's average only at elevated sample rates
		
		if (wet < 1.0) {inputSampleL *= wet; inputSampleR *= wet;}
		if (dry < 1.0) {drySampleL *= dry; drySampleR *= dry;}
		inputSampleL += drySampleL; inputSampleR += drySampleR;
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
	gainOutL = gainOutR = 1.0;
	
	for(int count = 0; count < delayA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < delayB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < delayC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < delayD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < delayE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < delayF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < delayG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < delayH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < delayI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < delayJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < delayK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < delayL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < delayM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < delayN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < delayO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < delayP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
	for(int count = 0; count < delayQ+2; count++) {dram->aQL[count] = 0.0; dram->aQR[count] = 0.0;}
	for(int count = 0; count < delayR+2; count++) {dram->aRL[count] = 0.0; dram->aRR[count] = 0.0;}
	for(int count = 0; count < delayS+2; count++) {dram->aSL[count] = 0.0; dram->aSR[count] = 0.0;}
	for(int count = 0; count < delayT+2; count++) {dram->aTL[count] = 0.0; dram->aTR[count] = 0.0;}
	for(int count = 0; count < delayU+2; count++) {dram->aUL[count] = 0.0; dram->aUR[count] = 0.0;}
	for(int count = 0; count < delayV+2; count++) {dram->aVL[count] = 0.0; dram->aVR[count] = 0.0;}
	for(int count = 0; count < delayW+2; count++) {dram->aWL[count] = 0.0; dram->aWR[count] = 0.0;}
	for(int count = 0; count < delayX+2; count++) {dram->aXL[count] = 0.0; dram->aXR[count] = 0.0;}
	for(int count = 0; count < delayY+2; count++) {dram->aYL[count] = 0.0; dram->aYR[count] = 0.0;}
	
	for(int count = 0; count < earlyA+2; count++) {dram->eAL[count] = 0.0; dram->eAR[count] = 0.0;}
	for(int count = 0; count < earlyB+2; count++) {dram->eBL[count] = 0.0; dram->eBR[count] = 0.0;}
	for(int count = 0; count < earlyC+2; count++) {dram->eCL[count] = 0.0; dram->eCR[count] = 0.0;}
	for(int count = 0; count < earlyD+2; count++) {dram->eDL[count] = 0.0; dram->eDR[count] = 0.0;}
	for(int count = 0; count < earlyE+2; count++) {dram->eEL[count] = 0.0; dram->eER[count] = 0.0;}
	for(int count = 0; count < earlyF+2; count++) {dram->eFL[count] = 0.0; dram->eFR[count] = 0.0;}
	for(int count = 0; count < earlyG+2; count++) {dram->eGL[count] = 0.0; dram->eGR[count] = 0.0;}
	for(int count = 0; count < earlyH+2; count++) {dram->eHL[count] = 0.0; dram->eHR[count] = 0.0;}
	for(int count = 0; count < earlyI+2; count++) {dram->eIL[count] = 0.0; dram->eIR[count] = 0.0;}
	
	for(int count = 0; count < predelay+2; count++) {dram->aZL[count] = 0.0; dram->aZR[count] = 0.0;}
	for(int count = 0; count < vlfpredelay+2; count++) {dram->aVLFL[count] = 0.0; dram->aVLFR[count] = 0.0;}
	
	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	feedbackEL = 0.0;
	
	feedbackER = 0.0;
	feedbackJR = 0.0;
	feedbackOR = 0.0;
	feedbackTR = 0.0;
	feedbackYR = 0.0;
	
	prevMulchBL = 0.0;
	prevMulchBR = 0.0;
	prevMulchCL = 0.0;
	prevMulchCR = 0.0;
	prevMulchDL = 0.0;
	prevMulchDR = 0.0;
	prevMulchEL = 0.0;
	prevMulchER = 0.0;
	
	tailL = 0.0;
	tailR = 0.0;
	
	prevOutEL = 0.0;
	prevOutER = 0.0;
	prevInEL = 0.0;
	prevInER = 0.0;
	
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0; lastRefR[count] = 0.0;}
	
	earlyAL = 1;
	earlyBL = 1;
	earlyCL = 1;
	earlyDL = 1;
	earlyEL = 1;
	earlyFL = 1;
	earlyGL = 1;
	earlyHL = 1;
	earlyIL = 1;
	
	earlyAR = 1;
	earlyBR = 1;
	earlyCR = 1;
	earlyDR = 1;
	earlyER = 1;
	earlyFR = 1;
	earlyGR = 1;
	earlyHR = 1;
	earlyIR = 1;
	
	countAL = 1;
	countBL = 1;
	countCL = 1;
	countDL = 1;	
	countEL = 1;
	countFL = 1;
	countGL = 1;
	countHL = 1;
	countIL = 1;
	countJL = 1;
	countKL = 1;
	countLL = 1;
	countML = 1;
	countNL = 1;
	countOL = 1;
	countPL = 1;
	countQL = 1;
	countRL = 1;
	countSL = 1;
	countTL = 1;
	countUL = 1;
	countVL = 1;
	countWL = 1;
	countXL = 1;
	countYL = 1;
	
	countAR = 1;
	countBR = 1;
	countCR = 1;
	countDR = 1;	
	countER = 1;
	countFR = 1;
	countGR = 1;
	countHR = 1;
	countIR = 1;
	countJR = 1;
	countKR = 1;
	countLR = 1;
	countMR = 1;
	countNR = 1;
	countOR = 1;
	countPR = 1;
	countQR = 1;
	countRR = 1;
	countSR = 1;
	countTR = 1;
	countUR = 1;
	countVR = 1;
	countWR = 1;
	countXR = 1;
	countYR = 1;
	
	countZ = 1;
	countVLF = 1;
	
	cycle = 0;
		
	for (int x = 0; x < pear_total; x++) {dram->pearA[x] = 0.0; dram->pearB[x] = 0.0; dram->pearC[x] = 0.0;}
	//from PearEQ
	
	subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
	//from SubTight
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
