#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Verbity2"
#define AIRWINDOWS_DESCRIPTION "Adds stereo crossmodulation and expands Verbity's feedforward reverb topology."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','e','s' )
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
{ .name = "RmSize", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sustain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mulch", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double iirAL;
	double iirBL;
	
	//double aZL[5191];
	
	
	
	
	
	
	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	double feedbackEL;
	
	double previousAL;
	double previousBL;
	double previousCL;
	double previousDL;
	double previousEL;
	
	double lastRefL[7];
	
	double iirAR;
	double iirBR;
	
	//double aZR[5191];
	
	
	
	
	
	
	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	double feedbackER;
	
	double previousAR;
	double previousBR;
	double previousCR;
	double previousDR;
	double previousER;
	
	double lastRefR[7];
	
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
	int countN, delayN;		
	int countO, delayO;		
	int countP, delayP;		
	int countQ, delayQ;		
	int countR, delayR;		
	int countS, delayS;		
	int countT, delayT;		
	int countU, delayU;		
	int countV, delayV;		
	int countW, delayW;		
	int countX, delayX;		
	int countY, delayY;		
	int countZ, delayZ;		
	int cycle;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double aAL[5005]; //AFKPU
	double aFL[4953];
	double aKL[4921];
	double aPL[4801];
	double aUL[4753];
	double aBL[4351]; //BGLQV
	double aGL[4159];
	double aLL[3931];
	double aQL[3531];
	double aVL[3331];
	double aCL[3325]; //CHMRW
	double aHL[2793];
	double aML[2769];
	double aRL[2391];
	double aWL[2349];
	double aDL[2143]; //DINSX
	double aIL[1813];
	double aNL[1735];
	double aSL[1173];
	double aXL[789];
	double aEL[679]; //EJOTY
	double aJL[645];
	double aOL[441];
	double aTL[351];
	double aYL[283];
	double aAR[5005]; //AFKPU
	double aFR[4953];
	double aKR[4921];
	double aPR[4801];
	double aUR[4753];
	double aBR[4351]; //BGLQV
	double aGR[4159];
	double aLR[3931];
	double aQR[3531];
	double aVR[3331];
	double aCR[3325]; //CHMRW
	double aHR[2793];
	double aMR[2769];
	double aRR[2391];
	double aWR[2349];
	double aDR[2143]; //DINSX
	double aIR[1813];
	double aNR[1735];
	double aSR[1173];
	double aXR[789];
	double aER[679]; //EJOTY
	double aJR[645];
	double aOR[441];
	double aTR[351];
	double aYR[283];
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
	
	//mulch is tone control, darken to obscure the Chrome Oxide, not as much highpass
	Float64 size = (pow(GetParameter( kParam_One ),2.0)*0.9)+0.1;
	Float64 regen = (1.0-pow(1.0-GetParameter( kParam_Two ),3.0))*0.00032;
	Float64 mulchSetting = GetParameter( kParam_Three );
	Float64 lowpass = (1.0-(mulchSetting*0.75))/sqrt(overallscale);
	Float64 highpass = (0.007+(mulchSetting*0.022))/sqrt(overallscale);
	Float64 interpolateMax = 0.07+(mulchSetting*0.4);
	Float64 wet = GetParameter( kParam_Four )*2.0;
	Float64 dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	//delayZ = 5189.0*size; //z can be predelay
	
	delayA = 5003.0*size;
	delayF = 4951.0*size;
	delayK = 4919.0*size;
	delayP = 4799.0*size;
	delayU = 4751.0*size;
	
	delayB = 4349.0*size;
	delayG = 4157.0*size;
	delayL = 3929.0*size;
	delayQ = 3529.0*size;
	delayV = 3329.0*size;
	
	delayC = 3323.0*size;
	delayH = 2791.0*size;
	delayM = 2767.0*size;
	delayR = 2389.0*size;
	delayW = 2347.0*size;
	
	delayD = 2141.0*size;
	delayI = 1811.0*size;
	delayN = 1733.0*size;
	delayS = 1171.0*size;
	delayX = 787.0*size;
	
	delayE = 677.0*size;
	delayJ = 643.0*size;
	delayO = 439.0*size;
	delayT = 349.0*size;
	delayY = 281.0*size; //balanced primes of order two
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		if (fabs(iirAL)<1.18e-37) iirAL = 0.0;
		iirAL = (iirAL*(1.0-highpass))+(inputSampleL*highpass); inputSampleL -= iirAL;
		if (fabs(iirAR)<1.18e-37) iirAR = 0.0;
		iirAR = (iirAR*(1.0-highpass))+(inputSampleR*highpass); inputSampleR -= iirAR;
		//first filter is highpass
		
		double interpolateL = interpolateMax + (interpolateMax * (double(fpdL)/UINT32_MAX));
		double interpolateR = interpolateMax + (interpolateMax * (double(fpdR)/UINT32_MAX));
		//we have our feedback soften also working as Chrome Oxide soften/noise
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			feedbackAL = (feedbackAL*(1.0-interpolateL))+(previousAL*interpolateL); previousAL = feedbackAL;
			feedbackBL = (feedbackBL*(1.0-interpolateL))+(previousBL*interpolateL); previousBL = feedbackBL;
			feedbackCL = (feedbackCL*(1.0-interpolateL))+(previousCL*interpolateL); previousCL = feedbackCL;
			feedbackDL = (feedbackDL*(1.0-interpolateL))+(previousDL*interpolateL); previousDL = feedbackDL;
			feedbackEL = (feedbackEL*(1.0-interpolateL))+(previousEL*interpolateL); previousEL = feedbackEL;
			feedbackAR = (feedbackAR*(1.0-interpolateR))+(previousAR*interpolateR); previousAR = feedbackAR;
			feedbackBR = (feedbackBR*(1.0-interpolateR))+(previousBR*interpolateR); previousBR = feedbackBR;
			feedbackCR = (feedbackCR*(1.0-interpolateR))+(previousCR*interpolateR); previousCR = feedbackCR;
			feedbackDR = (feedbackDR*(1.0-interpolateR))+(previousDR*interpolateR); previousDR = feedbackDR;
			feedbackER = (feedbackER*(1.0-interpolateR))+(previousER*interpolateR); previousER = feedbackER;
			
			//	aZL[countZ] = inputSampleL;
			//	aZR[countZ] = inputSampleR;
			//	countZ++; if (countZ < 0 || countZ > delayZ) countZ = 0;
			//	inputSampleL = aZL[countZ-((countZ > delayZ)?delayZ+1:0)];
			//	inputSampleR = aZR[countZ-((countZ > delayZ)?delayZ+1:0)];
			//predelay 
			
			dram->aAL[countA] = inputSampleL + (feedbackAL * (regen*(1.0-fabs(feedbackAL*regen))));
			dram->aBL[countB] = inputSampleL + (feedbackBL * (regen*(1.0-fabs(feedbackBL*regen))));
			dram->aCL[countC] = inputSampleL + (feedbackCL * (regen*(1.0-fabs(feedbackCL*regen))));
			dram->aDL[countD] = inputSampleL + (feedbackDL * (regen*(1.0-fabs(feedbackDL*regen))));
			dram->aEL[countE] = inputSampleL + (feedbackEL * (regen*(1.0-fabs(feedbackEL*regen))));
			
			dram->aAR[countA] = inputSampleR + (feedbackAR * (regen*(1.0-fabs(feedbackAR*regen))));
			dram->aBR[countB] = inputSampleR + (feedbackBR * (regen*(1.0-fabs(feedbackBR*regen))));
			dram->aCR[countC] = inputSampleR + (feedbackCR * (regen*(1.0-fabs(feedbackCR*regen))));
			dram->aDR[countD] = inputSampleR + (feedbackDR * (regen*(1.0-fabs(feedbackDR*regen))));
			dram->aER[countE] = inputSampleR + (feedbackER * (regen*(1.0-fabs(feedbackER*regen))));
			
			countA++; if (countA < 0 || countA > delayA) countA = 0;
			countB++; if (countB < 0 || countB > delayB) countB = 0;
			countC++; if (countC < 0 || countC > delayC) countC = 0;
			countD++; if (countD < 0 || countD > delayD) countD = 0;
			countE++; if (countE < 0 || countE > delayE) countE = 0;
			
			double outAL = dram->aAL[countA-((countA > delayA)?delayA+1:0)];
			double outBL = dram->aBL[countB-((countB > delayB)?delayB+1:0)];
			double outCL = dram->aCL[countC-((countC > delayC)?delayC+1:0)];
			double outDL = dram->aDL[countD-((countD > delayD)?delayD+1:0)];
			double outEL = dram->aEL[countE-((countE > delayE)?delayE+1:0)];
			
			double outAR = dram->aAR[countA-((countA > delayA)?delayA+1:0)];
			double outBR = dram->aBR[countB-((countB > delayB)?delayB+1:0)];
			double outCR = dram->aCR[countC-((countC > delayC)?delayC+1:0)];
			double outDR = dram->aDR[countD-((countD > delayD)?delayD+1:0)];
			double outER = dram->aER[countE-((countE > delayE)?delayE+1:0)];
			
			//-------- one
			
			dram->aFL[countF] = ((outAL*3.0) - ((outBL + outCL + outDL + outEL)*2.0));
			dram->aGL[countG] = ((outBL*3.0) - ((outAL + outCL + outDL + outEL)*2.0));
			dram->aHL[countH] = ((outCL*3.0) - ((outAL + outBL + outDL + outEL)*2.0));
			dram->aIL[countI] = ((outDL*3.0) - ((outAL + outBL + outCL + outEL)*2.0));
			dram->aJL[countJ] = ((outEL*3.0) - ((outAL + outBL + outCL + outDL)*2.0));
			
			dram->aFR[countF] = ((outAR*3.0) - ((outBR + outCR + outDR + outER)*2.0));
			dram->aGR[countG] = ((outBR*3.0) - ((outAR + outCR + outDR + outER)*2.0));
			dram->aHR[countH] = ((outCR*3.0) - ((outAR + outBR + outDR + outER)*2.0));
			dram->aIR[countI] = ((outDR*3.0) - ((outAR + outBR + outCR + outER)*2.0));
			dram->aJR[countJ] = ((outER*3.0) - ((outAR + outBR + outCR + outDR)*2.0));
			
			countF++; if (countF < 0 || countF > delayF) countF = 0;
			countG++; if (countG < 0 || countG > delayG) countG = 0;
			countH++; if (countH < 0 || countH > delayH) countH = 0;
			countI++; if (countI < 0 || countI > delayI) countI = 0;
			countJ++; if (countJ < 0 || countJ > delayJ) countJ = 0;
			
			double outFL = dram->aFL[countF-((countF > delayF)?delayF+1:0)];
			double outGL = dram->aGL[countG-((countG > delayG)?delayG+1:0)];
			double outHL = dram->aHL[countH-((countH > delayH)?delayH+1:0)];
			double outIL = dram->aIL[countI-((countI > delayI)?delayI+1:0)];
			double outJL = dram->aJL[countJ-((countJ > delayJ)?delayJ+1:0)];
			
			double outFR = dram->aFR[countF-((countF > delayF)?delayF+1:0)];
			double outGR = dram->aGR[countG-((countG > delayG)?delayG+1:0)];
			double outHR = dram->aHR[countH-((countH > delayH)?delayH+1:0)];
			double outIR = dram->aIR[countI-((countI > delayI)?delayI+1:0)];
			double outJR = dram->aJR[countJ-((countJ > delayJ)?delayJ+1:0)];
			
			//-------- two
			
			dram->aKL[countK] = ((outFL*3.0) - ((outGL + outHL + outIL + outJL)*2.0));
			dram->aLL[countL] = ((outGL*3.0) - ((outFL + outHL + outIL + outJL)*2.0));
			dram->aML[countM] = ((outHL*3.0) - ((outFL + outGL + outIL + outJL)*2.0));
			dram->aNL[countN] = ((outIL*3.0) - ((outFL + outGL + outHL + outJL)*2.0));
			dram->aOL[countO] = ((outJL*3.0) - ((outFL + outGL + outHL + outIL)*2.0));
			
			dram->aKR[countK] = ((outFR*3.0) - ((outGR + outHR + outIR + outJR)*2.0));
			dram->aLR[countL] = ((outGR*3.0) - ((outFR + outHR + outIR + outJR)*2.0));
			dram->aMR[countM] = ((outHR*3.0) - ((outFR + outGR + outIR + outJR)*2.0));
			dram->aNR[countN] = ((outIR*3.0) - ((outFR + outGR + outHR + outJR)*2.0));
			dram->aOR[countO] = ((outJR*3.0) - ((outFR + outGR + outHR + outIR)*2.0));
			
			countK++; if (countK < 0 || countK > delayK) countK = 0;
			countL++; if (countL < 0 || countL > delayL) countL = 0;
			countM++; if (countM < 0 || countM > delayM) countM = 0;
			countN++; if (countN < 0 || countN > delayN) countN = 0;
			countO++; if (countO < 0 || countO > delayO) countO = 0;
			
			double outKL = dram->aKL[countK-((countK > delayK)?delayK+1:0)];
			double outLL = dram->aLL[countL-((countL > delayL)?delayL+1:0)];
			double outML = dram->aML[countM-((countM > delayM)?delayM+1:0)];
			double outNL = dram->aNL[countN-((countN > delayN)?delayN+1:0)];
			double outOL = dram->aOL[countO-((countO > delayO)?delayO+1:0)];
			
			double outKR = dram->aKR[countK-((countK > delayK)?delayK+1:0)];
			double outLR = dram->aLR[countL-((countL > delayL)?delayL+1:0)];
			double outMR = dram->aMR[countM-((countM > delayM)?delayM+1:0)];
			double outNR = dram->aNR[countN-((countN > delayN)?delayN+1:0)];
			double outOR = dram->aOR[countO-((countO > delayO)?delayO+1:0)];
			
			//-------- three
			
			dram->aPL[countP] = ((outKL*3.0) - ((outLL + outML + outNL + outOL)*2.0));
			dram->aQL[countQ] = ((outLL*3.0) - ((outKL + outML + outNL + outOL)*2.0));
			dram->aRL[countR] = ((outML*3.0) - ((outKL + outLL + outNL + outOL)*2.0));
			dram->aSL[countS] = ((outNL*3.0) - ((outKL + outLL + outML + outOL)*2.0));
			dram->aTL[countT] = ((outOL*3.0) - ((outKL + outLL + outML + outNL)*2.0));
			
			dram->aPR[countP] = ((outKR*3.0) - ((outLR + outMR + outNR + outOR)*2.0));
			dram->aQR[countQ] = ((outLR*3.0) - ((outKR + outMR + outNR + outOR)*2.0));
			dram->aRR[countR] = ((outMR*3.0) - ((outKR + outLR + outNR + outOR)*2.0));
			dram->aSR[countS] = ((outNR*3.0) - ((outKR + outLR + outMR + outOR)*2.0));
			dram->aTR[countT] = ((outOR*3.0) - ((outKR + outLR + outMR + outNR)*2.0));
			
			countP++; if (countP < 0 || countP > delayP) countP = 0;
			countQ++; if (countQ < 0 || countQ > delayQ) countQ = 0;
			countR++; if (countR < 0 || countR > delayR) countR = 0;
			countS++; if (countS < 0 || countS > delayS) countS = 0;
			countT++; if (countT < 0 || countT > delayT) countT = 0;
			
			double outPL = dram->aPL[countP-((countP > delayP)?delayP+1:0)];
			double outQL = dram->aQL[countQ-((countQ > delayQ)?delayQ+1:0)];
			double outRL = dram->aRL[countR-((countR > delayR)?delayR+1:0)];
			double outSL = dram->aSL[countS-((countS > delayS)?delayS+1:0)];
			double outTL = dram->aTL[countT-((countT > delayT)?delayT+1:0)];
			
			double outPR = dram->aPR[countP-((countP > delayP)?delayP+1:0)];
			double outQR = dram->aQR[countQ-((countQ > delayQ)?delayQ+1:0)];
			double outRR = dram->aRR[countR-((countR > delayR)?delayR+1:0)];
			double outSR = dram->aSR[countS-((countS > delayS)?delayS+1:0)];
			double outTR = dram->aTR[countT-((countT > delayT)?delayT+1:0)];
			
			//-------- four
			
			dram->aUL[countU] = ((outPL*3.0) - ((outQL + outRL + outSL + outTL)*2.0));
			dram->aVL[countV] = ((outQL*3.0) - ((outPL + outRL + outSL + outTL)*2.0));
			dram->aWL[countW] = ((outRL*3.0) - ((outPL + outQL + outSL + outTL)*2.0));
			dram->aXL[countX] = ((outSL*3.0) - ((outPL + outQL + outRL + outTL)*2.0));
			dram->aYL[countY] = ((outTL*3.0) - ((outPL + outQL + outRL + outSL)*2.0));
			
			dram->aUR[countU] = ((outPR*3.0) - ((outQR + outRR + outSR + outTR)*2.0));
			dram->aVR[countV] = ((outQR*3.0) - ((outPR + outRR + outSR + outTR)*2.0));
			dram->aWR[countW] = ((outRR*3.0) - ((outPR + outQR + outSR + outTR)*2.0));
			dram->aXR[countX] = ((outSR*3.0) - ((outPR + outQR + outRR + outTR)*2.0));
			dram->aYR[countY] = ((outTR*3.0) - ((outPR + outQR + outRR + outSR)*2.0));
			
			countU++; if (countU < 0 || countU > delayU) countU = 0;
			countV++; if (countV < 0 || countV > delayV) countV = 0;
			countW++; if (countW < 0 || countW > delayW) countW = 0;
			countX++; if (countX < 0 || countX > delayX) countX = 0;
			countY++; if (countY < 0 || countY > delayY) countY = 0;
			
			double outUL = dram->aUL[countU-((countU > delayU)?delayU+1:0)];
			double outVL = dram->aVL[countV-((countV > delayV)?delayV+1:0)];
			double outWL = dram->aWL[countW-((countW > delayW)?delayW+1:0)];
			double outXL = dram->aXL[countX-((countX > delayX)?delayX+1:0)];
			double outYL = dram->aYL[countY-((countY > delayY)?delayY+1:0)];
			
			double outUR = dram->aUR[countU-((countU > delayU)?delayU+1:0)];
			double outVR = dram->aVR[countV-((countV > delayV)?delayV+1:0)];
			double outWR = dram->aWR[countW-((countW > delayW)?delayW+1:0)];
			double outXR = dram->aXR[countX-((countX > delayX)?delayX+1:0)];
			double outYR = dram->aYR[countY-((countY > delayY)?delayY+1:0)];
			
			//-------- five
			
			
			feedbackAR = ((outUL*3.0) - ((outVL + outWL + outXL + outYL)*2.0));
			feedbackBL = ((outVL*3.0) - ((outUL + outWL + outXL + outYL)*2.0));
			feedbackCR = ((outWL*3.0) - ((outUL + outVL + outXL + outYL)*2.0));
			feedbackDL = ((outXL*3.0) - ((outUL + outVL + outWL + outYL)*2.0));
			feedbackER = ((outYL*3.0) - ((outUL + outVL + outWL + outXL)*2.0));
			
			feedbackAL = ((outUR*3.0) - ((outVR + outWR + outXR + outYR)*2.0));
			feedbackBR = ((outVR*3.0) - ((outUR + outWR + outXR + outYR)*2.0));
			feedbackCL = ((outWR*3.0) - ((outUR + outVR + outXR + outYR)*2.0));
			feedbackDR = ((outXR*3.0) - ((outUR + outVR + outWR + outYR)*2.0));
			feedbackEL = ((outYR*3.0) - ((outUR + outVR + outWR + outXR)*2.0));
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outUL + outVL + outWL + outXL + outYL)*0.0016;
			inputSampleR = (outUR + outVR + outWR + outXR + outYR)*0.0016;
			//and take the final combined sum of outputs, corrected for Householder gain
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
		
		if (fabs(iirBL)<1.18e-37) iirBL = 0.0;
		iirBL = (iirBL*(1.0-lowpass))+(inputSampleL*lowpass); inputSampleL = iirBL;
		if (fabs(iirBR)<1.18e-37) iirBR = 0.0;
		iirBR = (iirBR*(1.0-lowpass))+(inputSampleR*lowpass); inputSampleR = iirBR;
		//second filter
		
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
	iirAL = 0.0;
	iirBL = 0.0;
	
	iirAR = 0.0;
	iirBR = 0.0;
	
	//for(int count = 0; count < 5190; count++) {aZL[count] = 0.0; aZR[count] = 0.0;}
	
	for(int count = 0; count < 5004; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;} //AFKPU
	for(int count = 0; count < 4952; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < 4920; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < 4800; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
	for(int count = 0; count < 4752; count++) {dram->aUL[count] = 0.0; dram->aUR[count] = 0.0;}
	
	for(int count = 0; count < 4350; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;} //BGLQV
	for(int count = 0; count < 4158; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < 3930; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < 3530; count++) {dram->aQL[count] = 0.0; dram->aQR[count] = 0.0;}
	for(int count = 0; count < 3330; count++) {dram->aVL[count] = 0.0; dram->aVR[count] = 0.0;}
	
	for(int count = 0; count < 3324; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;} //CHMRW
	for(int count = 0; count < 2792; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < 2768; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < 2390; count++) {dram->aRL[count] = 0.0; dram->aRR[count] = 0.0;}
	for(int count = 0; count < 2348; count++) {dram->aWL[count] = 0.0; dram->aWR[count] = 0.0;}
	
	for(int count = 0; count < 2142; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;} //DINSX
	for(int count = 0; count < 1812; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < 1734; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < 1172; count++) {dram->aSL[count] = 0.0; dram->aSR[count] = 0.0;}
	for(int count = 0; count < 788; count++) {dram->aXL[count] = 0.0; dram->aXR[count] = 0.0;}
	
	for(int count = 0; count < 678; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;} //EJOTY
	for(int count = 0; count < 644; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < 440; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < 350; count++) {dram->aTL[count] = 0.0; dram->aTR[count] = 0.0;}
	for(int count = 0; count < 282; count++) {dram->aYL[count] = 0.0; dram->aYR[count] = 0.0;}
	
	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	feedbackEL = 0.0;
	
	previousAL = 0.0;
	previousBL = 0.0;
	previousCL = 0.0;
	previousDL = 0.0;
	previousEL = 0.0;
	
	feedbackAR = 0.0;
	feedbackBR = 0.0;
	feedbackCR = 0.0;
	feedbackDR = 0.0;
	feedbackER = 0.0;
	
	previousAR = 0.0;
	previousBR = 0.0;
	previousCR = 0.0;
	previousDR = 0.0;
	previousER = 0.0;
	
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0; lastRefR[count] = 0.0;}
	
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
	countQ = 1;
	countR = 1;
	countS = 1;
	countT = 1;
	countU = 1;
	countV = 1;
	countW = 1;
	countX = 1;
	countY = 1;
	countZ = 1;
	
	cycle = 0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;	
	return noErr;
}

};
