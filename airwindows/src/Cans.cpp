#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Cans"
#define AIRWINDOWS_DESCRIPTION "A room simulator for headphone mixers."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','a','n' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
static const int kA = 1;
static const int kB = 2;
static const int kC = 3;
static const int kD = 4;
static const int kE = 5;
static const int kDefaultValue_ParamA = kC;
const int kshortA = 193;
const int kshortB = 588;
const int kshortC = 551;
const int kshortD = 325;
const int kshortE = 166;
const int kshortF = 427;
const int kshortG = 313;
const int kshortH = 575;
const int kshortI = 101;
const int kshortJ = 794;
const int kshortK = 789;
const int kshortL = 385;
const int kshortM = 12;
const int kshortN = 1009;
const int kshortO = 64;
const int kshortP = 544;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static char const * const enumStrings0[] = { "", "Studio A", "Studio B", "Studio C", "Studio D", "Studio E", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Room", .min = 1, .max = 5, .def = 3, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Diffuse", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Damping", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Crossfd", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	
		
	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	
	double iirInL;
	double iirFAL;
	double iirFBL;
	double iirFCL;
	double iirFDL;
	double iirOutL;
	double iirInR;
	double iirFAR;
	double iirFBR;
	double iirFCR;
	double iirFDR;
	double iirOutR;
	
	int countA;
	int countB;
	int countC;
	int countD;
	int countE;
	int countF;
	int countG;
	int countH;
	int countI;
	int countJ;
	int countK;
	int countL;		
	int countM;		
	int countN;		
	int countO;		
	int countP;		
		
	int shortA;
	int shortB;
	int shortC;
	int shortD;
	int shortE;
	int shortF;
	int shortG;
	int shortH;
	int shortI;
	int shortJ;
	int shortK;
	int shortL;
	int shortM;
	int shortN;
	int shortO;
	int shortP;
	
	int prevclearcoat;
	
	enum {
		bez_AL,
		bez_AR,
		bez_BL,
		bez_BR,
		bez_CL,
		bez_CR,	
		bez_InL,
		bez_InR,
		bez_UnInL,
		bez_UnInR,
		bez_SampL,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double aAL[kshortA+5];
	double aBL[kshortB+5];
	double aCL[kshortC+5];
	double aDL[kshortD+5];
	double aEL[kshortE+5];
	double aFL[kshortF+5];
	double aGL[kshortG+5];
	double aHL[kshortH+5];
	double aIL[kshortI+5];
	double aJL[kshortJ+5];
	double aKL[kshortK+5];
	double aLL[kshortL+5];
	double aML[kshortM+5];
	double aNL[kshortN+5];
	double aOL[kshortO+5];
	double aPL[kshortP+5];
	double aAR[kshortA+5];
	double aBR[kshortB+5];
	double aCR[kshortC+5];
	double aDR[kshortD+5];
	double aER[kshortE+5];
	double aFR[kshortF+5];
	double aGR[kshortG+5];
	double aHR[kshortH+5];
	double aIR[kshortI+5];
	double aJR[kshortJ+5];
	double aKR[kshortK+5];
	double aLR[kshortL+5];
	double aMR[kshortM+5];
	double aNR[kshortN+5];
	double aOR[kshortO+5];
	double aPR[kshortP+5];
	double bez[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	int clearcoat = GetParameter( kParam_A );
	if (clearcoat != prevclearcoat) {
		for(int count = 0; count < kshortA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
		for(int count = 0; count < kshortB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
		for(int count = 0; count < kshortC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
		for(int count = 0; count < kshortD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
		for(int count = 0; count < kshortE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
		for(int count = 0; count < kshortF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
		for(int count = 0; count < kshortG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
		for(int count = 0; count < kshortH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
		for(int count = 0; count < kshortI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
		for(int count = 0; count < kshortJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
		for(int count = 0; count < kshortK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
		for(int count = 0; count < kshortL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
		for(int count = 0; count < kshortM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
		for(int count = 0; count < kshortN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
		for(int count = 0; count < kshortO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
		for(int count = 0; count < kshortP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}		
		countA = 1;
		countB = 1;
		countC = 1;
		countD = 1;	
		countE = 1;
		countF = 1;
		countG = 1;
		countH = 1;
		countI = 1;
		countJ = 1;
		countK = 1;
		countL = 1;
		countM = 1;
		countN = 1;
		countO = 1;
		countP = 1;
		
		switch (clearcoat)
		{
			case 0:
				shortA = 34;  shortB = 193;  shortC = 255;  shortD = 150;  shortE = 24;  shortF = 204;  shortG = 85;  shortH = 295;  shortI = 40;  shortJ = 371;  shortK = 370;  shortL = 202;  shortM = 11;  shortN = 431;  shortO = 23;  shortP = 323; //2 to 30 ms, 32 seat room  
				break; //32b-U rated incompressible if filesize larger than 20,150,000 bytes
			case 1:
				shortA = 32;  shortB = 298;  shortC = 402;  shortD = 161;  shortE = 165;  shortF = 149;  shortG = 43;  shortH = 313;  shortI = 83;  shortJ = 397;  shortK = 316;  shortL = 315;  shortM = 11;  shortN = 685;  shortO = 16;  shortP = 486; //3 to 40 ms, 59 seat room  
				break; //59b-U rated incompressible if filesize larger than 21,035,831 bytes
			case 2:
				shortA = 23;  shortB = 357;  shortC = 305;  shortD = 186;  shortE = 104;  shortF = 255;  shortG = 163;  shortH = 147;  shortI = 56;  shortJ = 480;  shortK = 317;  shortL = 107;  shortM = 11;  shortN = 704;  shortO = 26;  shortP = 543; //4 to 40 ms, 60 seat room  
				break; //60b-U rated incompressible if filesize larger than 21,035,831 bytes
			case 3:
				shortA = 192;  shortB = 463;  shortC = 420;  shortD = 34;  shortE = 161;  shortF = 426;  shortG = 160;  shortH = 474;  shortI = 21;  shortJ = 343;  shortK = 296;  shortL = 294;  shortM = 11;  shortN = 987;  shortO = 64;  shortP = 482; //5 to 51 ms, 94 seat room  
				break; //94b-U rated incompressible if filesize larger than 21,700,000 bytes
			case 4:
			default:
				shortA = 173;  shortB = 587;  shortC = 550;  shortD = 324;  shortE = 165;  shortF = 324;  shortG = 312;  shortH = 574;  shortI = 100;  shortJ = 793;  shortK = 788;  shortL = 385;  shortM = 11;  shortN = 1008;  shortO = 46;  shortP = 380; //10 to 67 ms, 177 seat club  
				break; //177-U rated incompressible if filesize larger than 22,796,452 bytes
		}
		prevclearcoat = clearcoat;
	}
	double derez = 1.0/overallscale;
	derez = 1.0 / ((int)(1.0/derez));
	//this hard-locks derez to exact subdivisions of 1.0
	
	int diffuse = pow(GetParameter( kParam_B ),1.618033988749894848204586)*16;
	double iirAmt = 1.0-pow(1.0-GetParameter( kParam_C ),1.618033988749894848204586);
	double iirExt = iirAmt/overallscale;
	double crossfade = (1.0-pow(1.0-GetParameter( kParam_D ),1.618033988749894848204586));
	double wet = pow(GetParameter( kParam_E ),1.618033988749894848204586);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		iirInL = (inputSampleL*iirExt)+(iirInL*(1.0-iirExt)); inputSampleL = iirInL;
		iirInR = (inputSampleR*iirExt)+(iirInR*(1.0-iirExt)); inputSampleR = iirInR;
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0;
			
			iirFAL = (feedbackAL*iirAmt)+(iirFAL*(1.0-iirAmt)); feedbackAL = iirFAL;
			iirFAR = (feedbackAR*iirAmt)+(iirFAR*(1.0-iirAmt)); feedbackAR = iirFAR;
			iirFBL = (feedbackBL*iirAmt)+(iirFBL*(1.0-iirAmt)); feedbackBL = iirFBL;
			iirFBR = (feedbackBR*iirAmt)+(iirFBR*(1.0-iirAmt)); feedbackBR = iirFBR;
			iirFCL = (feedbackCL*iirAmt)+(iirFCL*(1.0-iirAmt)); feedbackCL = iirFCL;
			iirFCR = (feedbackCR*iirAmt)+(iirFCR*(1.0-iirAmt)); feedbackCR = iirFCR;
			iirFDL = (feedbackDL*iirAmt)+(iirFDL*(1.0-iirAmt)); feedbackDL = iirFDL;
			iirFDR = (feedbackDR*iirAmt)+(iirFDR*(1.0-iirAmt)); feedbackDR = iirFDR;
			
			double outAL, outBL, outCL, outDL, outAR, outBR, outCR, outDR;
			if (diffuse < 4) {
				dram->aAL[countA] = (dram->bez[bez_SampL]) + (feedbackAL * 0.03125);
				dram->aAR[countA] = (dram->bez[bez_SampR]) + (feedbackAR * 0.03125);
				countA++; if (countA < 0 || countA > shortA) countA = 0;
				outAL = dram->aAL[countA-((countA > shortA)?shortA+1:0)];
				outAR = dram->aAR[countA-((countA > shortA)?shortA+1:0)];
			} else {
				outAL = (dram->bez[bez_SampL] - (dram->aAL[(countA+1)-((countA+1 > shortA)?shortA+1:0)]*0.618033988749894848204586)) + (feedbackAL * 0.03125);
				dram->aAL[countA] = outAL; outAL *= 0.618033988749894848204586;
				outAR = (dram->bez[bez_SampR] - (dram->aAR[(countA+1)-((countA+1 > shortA)?shortA+1:0)]*0.618033988749894848204586)) + (feedbackAR * 0.03125);
				dram->aAR[countA] = outAR; outAR *= 0.618033988749894848204586;
				countA++; if (countA < 0 || countA > shortA) countA = 0;
				outAL += dram->aAL[countA-((countA > shortA)?shortA+1:0)];
				outAR += dram->aAR[countA-((countA > shortA)?shortA+1:0)];
			}
			
			if (diffuse < 12) {
				dram->aBL[countB] = (dram->bez[bez_SampL]) + (feedbackBL * 0.03125);
				dram->aBR[countB] = (dram->bez[bez_SampR]) + (feedbackBR * 0.03125);
				countB++; if (countB < 0 || countB > shortB) countB = 0;
				outBL = dram->aBL[countB-((countB > shortB)?shortB+1:0)];
				outBR = dram->aBR[countB-((countB > shortB)?shortB+1:0)];
			} else {
				outBL = (dram->bez[bez_SampL] - (dram->aBL[(countB+1)-((countB+1 > shortB)?shortB+1:0)]*0.618033988749894848204586)) + (feedbackBL * 0.03125);
				dram->aBL[countB] = outBL; outBL *= 0.618033988749894848204586;
				outBR = (dram->bez[bez_SampR] - (dram->aBR[(countB+1)-((countB+1 > shortB)?shortB+1:0)]*0.618033988749894848204586)) + (feedbackBR * 0.03125);
				dram->aBR[countB] = outBR; outBR *= 0.618033988749894848204586;
				countB++; if (countB < 0 || countB > shortB) countB = 0;
				outBL += dram->aBL[countB-((countB > shortB)?shortB+1:0)];
				outBR += dram->aBR[countB-((countB > shortB)?shortB+1:0)];
			}
			
			if (diffuse < 10) {
				dram->aCL[countC] = (dram->bez[bez_SampL]) + (feedbackCL * 0.03125);
				dram->aCR[countC] = (dram->bez[bez_SampR]) + (feedbackCR * 0.03125);
				countC++; if (countC < 0 || countC > shortC) countC = 0;
				outCL = dram->aCL[countC-((countC > shortC)?shortC+1:0)];
				outCR = dram->aCR[countC-((countC > shortC)?shortC+1:0)];
			} else {
				outCL = (dram->bez[bez_SampL] - (dram->aCL[(countC+1)-((countC+1 > shortC)?shortC+1:0)]*0.618033988749894848204586)) + (feedbackCL * 0.03125);
				dram->aCL[countC] = outCL; outCL *= 0.618033988749894848204586;
				outCR = (dram->bez[bez_SampR] - (dram->aCR[(countC+1)-((countC+1 > shortC)?shortC+1:0)]*0.618033988749894848204586)) + (feedbackCR * 0.03125);
				dram->aCR[countC] = outCR; outCR *= 0.618033988749894848204586;
				countC++; if (countC < 0 || countC > shortC) countC = 0;
				outCL += dram->aCL[countC-((countC > shortC)?shortC+1:0)];
				outCR += dram->aCR[countC-((countC > shortC)?shortC+1:0)];
			}
			
			if (diffuse < 6) {
				dram->aDL[countD] = (dram->bez[bez_SampL]) + (feedbackDL * 0.03125);
				dram->aDR[countD] = (dram->bez[bez_SampR]) + (feedbackDR * 0.03125);
				countD++; if (countD < 0 || countD > shortD) countD = 0;
				outDL = dram->aDL[countD-((countD > shortD)?shortD+1:0)];
				outDR = dram->aDR[countD-((countD > shortD)?shortD+1:0)];
			} else {
				outDL = (dram->bez[bez_SampL] - (dram->aDL[(countD+1)-((countD+1 > shortD)?shortD+1:0)]*0.618033988749894848204586)) + (feedbackDL * 0.03125);
				dram->aDL[countD] = outDL; outDL *= 0.618033988749894848204586;
				outDR = (dram->bez[bez_SampR] - (dram->aDR[(countD+1)-((countD+1 > shortD)?shortD+1:0)]*0.618033988749894848204586)) + (feedbackDR * 0.03125);
				dram->aDR[countD] = outDR; outDR *= 0.618033988749894848204586;
				countD++; if (countD < 0 || countD > shortD) countD = 0;
				outDL += dram->aDL[countD-((countD > shortD)?shortD+1:0)];
				outDR += dram->aDR[countD-((countD > shortD)?shortD+1:0)];
			}
			
			double outEL, outFL, outGL, outHL, outER, outFR, outGR, outHR;
			if (diffuse < 3) {
				dram->aEL[countE] = outAL - (outBL + outCL + outDL);
				dram->aER[countE] = outAR - (outBR + outCR + outDR);
				countE++; if (countE < 0 || countE > shortE) countE = 0;
				outEL = dram->aEL[countE-((countE > shortE)?shortE+1:0)];
				outER = dram->aER[countE-((countE > shortE)?shortE+1:0)];
			} else {
				outEL = (outAL - (outBL + outCL + outDL)) - (dram->aEL[(countE+1)-((countE+1 > shortE)?shortE+1:0)]*0.618033988749894848204586);
				dram->aEL[countE] = outEL; outEL *= 0.618033988749894848204586;
				outER = (outAR - (outBR + outCR + outDR)) - (dram->aER[(countE+1)-((countE+1 > shortE)?shortE+1:0)]*0.618033988749894848204586);
				dram->aER[countE] = outER; outER *= 0.618033988749894848204586;
				countE++; if (countE < 0 || countE > shortE) countE = 0;
				outEL += dram->aEL[countE-((countE > shortE)?shortE+1:0)];
				outER += dram->aER[countE-((countE > shortE)?shortE+1:0)];
			}
			
			if (diffuse < 8) {
				dram->aFL[countF] = outBL - (outAL + outCL + outDL);
				dram->aFR[countF] = outBR - (outAR + outCR + outDR);
				countF++; if (countF < 0 || countF > shortF) countF = 0;
				outFL = dram->aFL[countF-((countF > shortF)?shortF+1:0)];
				outFR = dram->aFR[countF-((countF > shortF)?shortF+1:0)];
			} else {
				outFL = (outBL - (outAL + outCL + outDL)) - (dram->aFL[(countF+1)-((countF+1 > shortF)?shortF+1:0)]*0.618033988749894848204586);
				dram->aFL[countF] = outFL; outFL *= 0.618033988749894848204586;
				outFR = (outBR - (outAR + outCR + outDR)) - (dram->aFR[(countF+1)-((countF+1 > shortF)?shortF+1:0)]*0.618033988749894848204586);
				dram->aFR[countF] = outFR; outFR *= 0.618033988749894848204586;
				countF++; if (countF < 0 || countF > shortF) countF = 0;
				outFL += dram->aFL[countF-((countF > shortF)?shortF+1:0)];
				outFR += dram->aFR[countF-((countF > shortF)?shortF+1:0)];
			}
			
			if (diffuse < 5) {
				dram->aGL[countG] = outCL - (outAL + outBL + outDL);
				dram->aGR[countG] = outCR - (outAR + outBR + outDR);
				countG++; if (countG < 0 || countG > shortG) countG = 0;
				outGL = dram->aGL[countG-((countG > shortG)?shortG+1:0)];
				outGR = dram->aGR[countG-((countG > shortG)?shortG+1:0)];
			} else {
				outGL = (outCL - (outAL + outBL + outDL)) - (dram->aGL[(countG+1)-((countG+1 > shortG)?shortG+1:0)]*0.618033988749894848204586);
				dram->aGL[countG] = outGL; outGL *= 0.618033988749894848204586;
				outGR = (outCR - (outAR + outBR + outDR)) - (dram->aGR[(countG+1)-((countG+1 > shortG)?shortG+1:0)]*0.618033988749894848204586);
				dram->aGR[countG] = outGR; outGR *= 0.618033988749894848204586;
				countG++; if (countG < 0 || countG > shortG) countG = 0;
				outGL += dram->aGL[countG-((countG > shortG)?shortG+1:0)];
				outGR += dram->aGR[countG-((countG > shortG)?shortG+1:0)];
			}
			
			if (diffuse < 11) {
				dram->aHL[countH] = outDL - (outAL + outBL + outCL);
				dram->aHR[countH] = outDR - (outAR + outBR + outCR);
				countH++; if (countH < 0 || countH > shortH) countH = 0;
				outHL = dram->aHL[countH-((countH > shortH)?shortH+1:0)];
				outHR = dram->aHR[countH-((countH > shortH)?shortH+1:0)];
			} else {
				outHL = (outDL - (outAL + outBL + outCL)) - (dram->aHL[(countH+1)-((countH+1 > shortH)?shortH+1:0)]*0.618033988749894848204586);
				dram->aHL[countH] = outHL; outHL *= 0.618033988749894848204586;
				outHR = (outDR - (outAR + outBR + outCR)) - (dram->aHR[(countH+1)-((countH+1 > shortH)?shortH+1:0)]*0.618033988749894848204586);
				dram->aHR[countH] = outHR; outHR *= 0.618033988749894848204586;
				countH++; if (countH < 0 || countH > shortH) countH = 0;
				outHL += dram->aHL[countH-((countH > shortH)?shortH+1:0)];
				outHR += dram->aHR[countH-((countH > shortH)?shortH+1:0)];
			}
			
			double outIL, outJL, outKL, outLL, outIR, outJR, outKR, outLR;
			if (diffuse < 2) {
				dram->aIL[countI] = outEL - (outFL + outGL + outHL);
				dram->aIR[countI] = outER - (outFR + outGR + outHR);
				countI++; if (countI < 0 || countI > shortI) countI = 0;
				outIL = dram->aIL[countI-((countI > shortI)?shortI+1:0)];
				outIR = dram->aIR[countI-((countI > shortI)?shortI+1:0)];
			} else {
				outIL = (outEL - (outFL + outGL + outHL)) - (dram->aIL[(countI+1)-((countI+1 > shortI)?shortI+1:0)]*0.618033988749894848204586);
				dram->aIL[countI] = outIL; outIL *= 0.618033988749894848204586;
				outIR = (outER - (outFR + outGR + outHR)) - (dram->aIR[(countI+1)-((countI+1 > shortI)?shortI+1:0)]*0.618033988749894848204586);
				dram->aIR[countI] = outIR; outIR *= 0.618033988749894848204586;
				countI++; if (countI < 0 || countI > shortI) countI = 0;
				outIL += dram->aIL[countI-((countI > shortI)?shortI+1:0)];
				outIR += dram->aIR[countI-((countI > shortI)?shortI+1:0)];
			}
			
			if (diffuse < 14) {
				dram->aJL[countJ] = outFL - (outEL + outGL + outHL);
				dram->aJR[countJ] = outFR - (outER + outGR + outHR);
				countJ++; if (countJ < 0 || countJ > shortJ) countJ = 0;
				outJL = dram->aJL[countJ-((countJ > shortJ)?shortJ+1:0)];
				outJR = dram->aJR[countJ-((countJ > shortJ)?shortJ+1:0)];
			} else {
				outJL = (outFL - (outEL + outGL + outHL)) - (dram->aJL[(countJ+1)-((countJ+1 > shortJ)?shortJ+1:0)]*0.618033988749894848204586);
				dram->aJL[countJ] = outJL; outJL *= 0.618033988749894848204586;
				outJR = (outFR - (outER + outGR + outHR)) - (dram->aJR[(countJ+1)-((countJ+1 > shortJ)?shortJ+1:0)]*0.618033988749894848204586);
				dram->aJR[countJ] = outJR; outJR *= 0.618033988749894848204586;
				countJ++; if (countJ < 0 || countJ > shortJ) countJ = 0;
				outJL += dram->aJL[countJ-((countJ > shortJ)?shortJ+1:0)];
				outJR += dram->aJR[countJ-((countJ > shortJ)?shortJ+1:0)];
			}
			
			if (diffuse < 13) {
				dram->aKL[countK] = outGL - (outEL + outFL + outHL);
				dram->aKR[countK] = outGR - (outER + outFR + outHR);
				countK++; if (countK < 0 || countK > shortK) countK = 0;
				outKL = dram->aKL[countK-((countK > shortK)?shortK+1:0)];
				outKR = dram->aKR[countK-((countK > shortK)?shortK+1:0)];
			} else {
				outKL = (outGL - (outEL + outFL + outHL)) - (dram->aKL[(countK+1)-((countK+1 > shortK)?shortK+1:0)]*0.618033988749894848204586);
				dram->aKL[countK] = outKL; outKL *= 0.618033988749894848204586;
				outKR = (outGR - (outER + outFR + outHR)) - (dram->aKR[(countK+1)-((countK+1 > shortK)?shortK+1:0)]*0.618033988749894848204586);
				dram->aKR[countK] = outKR; outKR *= 0.618033988749894848204586;
				countK++; if (countK < 0 || countK > shortK) countK = 0;
				outKL += dram->aKL[countK-((countK > shortK)?shortK+1:0)];
				outKR += dram->aKR[countK-((countK > shortK)?shortK+1:0)];
			}
			
			if (diffuse < 7) {
				dram->aLL[countL] = outHL - (outEL + outFL + outGL);
				dram->aLR[countL] = outHR - (outER + outFR + outGR);
				countL++; if (countL < 0 || countL > shortL) countL = 0;
				outLL = dram->aLL[countL-((countL > shortL)?shortL+1:0)];
				outLR = dram->aLR[countL-((countL > shortL)?shortL+1:0)];
			} else {
				outLL = (outHL - (outEL + outFL + outGL)) - (dram->aLL[(countL+1)-((countL+1 > shortL)?shortL+1:0)]*0.618033988749894848204586);
				dram->aLL[countL] = outLL; outLL *= 0.618033988749894848204586;
				outLR = (outHR - (outER + outFR + outGR)) - (dram->aLR[(countL+1)-((countL+1 > shortL)?shortL+1:0)]*0.618033988749894848204586);
				dram->aLR[countL] = outLR; outLR *= 0.618033988749894848204586;
				countL++; if (countL < 0 || countL > shortL) countL = 0;
				outLL += dram->aLL[countL-((countL > shortL)?shortL+1:0)];
				outLR += dram->aLR[countL-((countL > shortL)?shortL+1:0)];
			}
			
			double outML, outNL, outOL, outPL, outMR, outNR, outOR, outPR;
			if (diffuse < 0) {
				dram->aML[countM] = outIL - (outJL + outKL + outLL);
				dram->aMR[countM] = outIR - (outJR + outKR + outLR);
				countM++; if (countM < 0 || countM > shortM) countM = 0;
				outML = dram->aML[countM-((countM > shortM)?shortM+1:0)];
				outMR = dram->aMR[countM-((countM > shortM)?shortM+1:0)];
			} else {
				outML = (outIL - (outJL + outKL + outLL)) - (dram->aML[(countM+1)-((countM+1 > shortM)?shortM+1:0)]*0.618033988749894848204586);
				dram->aML[countM] = outML; outML *= 0.618033988749894848204586;
				outMR = (outIR - (outJR + outKR + outLR)) - (dram->aMR[(countM+1)-((countM+1 > shortM)?shortM+1:0)]*0.618033988749894848204586);
				dram->aMR[countM] = outMR; outMR *= 0.618033988749894848204586;
				countM++; if (countM < 0 || countM > shortM) countM = 0;
				outML += dram->aML[countM-((countM > shortM)?shortM+1:0)];
				outMR += dram->aMR[countM-((countM > shortM)?shortM+1:0)];
			}
			
			if (diffuse < 15) {
				dram->aNL[countN] = outJL - (outIL + outKL + outLL);
				dram->aNR[countN] = outJR - (outIR + outKR + outLR);
				countN++; if (countN < 0 || countN > shortN) countN = 0;
				outNL = dram->aNL[countN-((countN > shortN)?shortN+1:0)];
				outNR = dram->aNR[countN-((countN > shortN)?shortN+1:0)];
			} else {
				outNL = (outJL - (outIL + outKL + outLL)) - (dram->aNL[(countN+1)-((countN+1 > shortN)?shortN+1:0)]*0.618033988749894848204586);
				dram->aNL[countN] = outNL; outNL *= 0.618033988749894848204586;
				outNR = (outJR - (outIR + outKR + outLR)) - (dram->aNR[(countN+1)-((countN+1 > shortN)?shortN+1:0)]*0.618033988749894848204586);
				dram->aNR[countN] = outNR; outNR *= 0.618033988749894848204586;
				countN++; if (countN < 0 || countN > shortN) countN = 0;
				outNL += dram->aNL[countN-((countN > shortN)?shortN+1:0)];
				outNR += dram->aNR[countN-((countN > shortN)?shortN+1:0)];
			}
			
			if (diffuse < 1) {
				dram->aOL[countO] = outKL - (outIL + outJL + outLL);
				dram->aOR[countO] = outKR - (outIR + outJR + outLR);
				countO++; if (countO < 0 || countO > shortO) countO = 0;
				outOL = dram->aOL[countO-((countO > shortO)?shortO+1:0)];
				outOR = dram->aOR[countO-((countO > shortO)?shortO+1:0)];
			} else {
				outOL = (outKL - (outIL + outJL + outLL)) - (dram->aOL[(countO+1)-((countO+1 > shortO)?shortO+1:0)]*0.618033988749894848204586);
				dram->aOL[countO] = outOL; outOL *= 0.618033988749894848204586;
				outOR = (outKR - (outIR + outJR + outLR)) - (dram->aOR[(countO+1)-((countO+1 > shortO)?shortO+1:0)]*0.618033988749894848204586);
				dram->aOR[countO] = outOR; outOR *= 0.618033988749894848204586;
				countO++; if (countO < 0 || countO > shortO) countO = 0;
				outOL += dram->aOL[countO-((countO > shortO)?shortO+1:0)];
				outOR += dram->aOR[countO-((countO > shortO)?shortO+1:0)];
			}
			
			if (diffuse < 9) {
				dram->aPL[countP] = outLL - (outIL + outJL + outKL);
				dram->aPR[countP] = outLR - (outIR + outJR + outKR);
				countP++; if (countP < 0 || countP > shortP) countP = 0;
				outPL = dram->aPL[countP-((countP > shortP)?shortP+1:0)];
				outPR = dram->aPR[countP-((countP > shortP)?shortP+1:0)];
			} else {
				outPL = (outLL - (outIL + outJL + outKL)) - (dram->aPL[(countP+1)-((countP+1 > shortP)?shortP+1:0)]*0.618033988749894848204586);
				dram->aPL[countP] = outPL; outPL *= 0.618033988749894848204586;
				outPR = (outLR - (outIR + outJR + outKR)) - (dram->aPR[(countP+1)-((countP+1 > shortP)?shortP+1:0)]*0.618033988749894848204586);
				dram->aPR[countP] = outPR; outPR *= 0.618033988749894848204586;
				countP++; if (countP < 0 || countP > shortP) countP = 0;
				outPL += dram->aPL[countP-((countP > shortP)?shortP+1:0)];
				outPR += dram->aPR[countP-((countP > shortP)?shortP+1:0)];
			}
			
			double steerAL = outML - (outNL + outOL + outPL);
			double steerBL = outNL - (outML + outOL + outPL);
			double steerCL = outOL - (outML + outNL + outPL);
			double steerDL = outPL - (outML + outNL + outOL);
			
			double steerAR = outMR - (outNR + outOR + outPR);
			double steerBR = outNR - (outMR + outOR + outPR);
			double steerCR = outOR - (outMR + outNR + outPR);
			double steerDR = outPR - (outMR + outNR + outOR);
			
			feedbackAL = (steerAL * (1.0-crossfade)) + (steerAR * crossfade);
			feedbackBL = (steerBL * (1.0-crossfade)) + (steerBR * crossfade);
			feedbackCL = (steerCL * (1.0-crossfade)) + (steerCR * crossfade);
			feedbackDL = (steerDL * (1.0-crossfade)) + (steerDR * crossfade);
			
			feedbackAR = (steerAR * (1.0-crossfade)) + (steerAL * crossfade);
			feedbackBR = (steerBR * (1.0-crossfade)) + (steerBL * crossfade);
			feedbackCR = (steerCR * (1.0-crossfade)) + (steerCL * crossfade);
			feedbackDR = (steerDR * (1.0-crossfade)) + (steerDL * crossfade);
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outML + outNL + outOL + outPL)/16.0;
			inputSampleR = (outMR + outNR + outOR + outPR)/16.0;
			//and take the final combined sum of outputs, corrected for Householder gain and averaging
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = inputSampleR;
			dram->bez[bez_SampR] = 0.0;
		}
		double CBL = (dram->bez[bez_CL]*(1.0-dram->bez[bez_cycle]))+(dram->bez[bez_BL]*dram->bez[bez_cycle]);
		double CBR = (dram->bez[bez_CR]*(1.0-dram->bez[bez_cycle]))+(dram->bez[bez_BR]*dram->bez[bez_cycle]);
		double BAL = (dram->bez[bez_BL]*(1.0-dram->bez[bez_cycle]))+(dram->bez[bez_AL]*dram->bez[bez_cycle]);
		double BAR = (dram->bez[bez_BR]*(1.0-dram->bez[bez_cycle]))+(dram->bez[bez_AR]*dram->bez[bez_cycle]);
		double CBAL = (dram->bez[bez_BL]+(CBL*(1.0-dram->bez[bez_cycle]))+(BAL*dram->bez[bez_cycle]))*0.125;
		double CBAR = (dram->bez[bez_BR]+(CBR*(1.0-dram->bez[bez_cycle]))+(BAR*dram->bez[bez_cycle]))*0.125;
		inputSampleL = CBAL;
		inputSampleR = CBAR;
		
		iirOutL = (inputSampleL*iirExt)+(iirOutL*(1.0-iirExt)); inputSampleL = iirOutL;
		iirOutR = (inputSampleR*iirExt)+(iirOutR*(1.0-iirExt)); inputSampleR = iirOutR;
		
		inputSampleL = (inputSampleL*wet)+(drySampleL*(1.0-wet));
		inputSampleR = (inputSampleR*wet)+(drySampleR*(1.0-wet));
		
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		
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
	for(int count = 0; count < kshortA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < kshortB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < kshortC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < kshortD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < kshortE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < kshortF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < kshortG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < kshortH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < kshortI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < kshortJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < kshortK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < kshortL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < kshortM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < kshortN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < kshortO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < kshortP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
		
	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	feedbackAR = 0.0;
	feedbackBR = 0.0;
	feedbackCR = 0.0;
	feedbackDR = 0.0;
	
	iirInL = 0.0;
	iirFAL = 0.0;
	iirFBL = 0.0;
	iirFCL = 0.0;
	iirFDL = 0.0;
	iirOutL = 0.0;
	iirInR = 0.0;
	iirFAR = 0.0;
	iirFBR = 0.0;
	iirFCR = 0.0;
	iirFDR = 0.0;
	iirOutR = 0.0;
		
	countA = 1;
	countB = 1;
	countC = 1;
	countD = 1;	
	countE = 1;
	countF = 1;
	countG = 1;
	countH = 1;
	countI = 1;
	countJ = 1;
	countK = 1;
	countL = 1;
	countM = 1;
	countN = 1;
	countO = 1;
	countP = 1;
			
	shortA = 23;
	shortB = 357;
	shortC = 305;
	shortD = 186;
	shortE = 104;
	shortF = 255;
	shortG = 163;
	shortH = 147;
	shortI = 56;
	shortJ = 480;
	shortK = 317;
	shortL = 107;
	shortM = 11;
	shortN = 704;
	shortO = 26;
	shortP = 543;
	prevclearcoat = -1;
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
