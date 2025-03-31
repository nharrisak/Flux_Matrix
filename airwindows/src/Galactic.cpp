#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Galactic"
#define AIRWINDOWS_DESCRIPTION "A super-reverb designed specially for pads and space ambient."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','l' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Replace", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Brightness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Detune", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bigness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double iirAL;
	double iirBL;
	
	
	
	
	double vibML, vibMR, depthM, oldfpd;
	
	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	
	double lastRefL[7];
	double thunderL;
	
	double iirAR;
	double iirBR;
	
	
	
	
	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
	
	double lastRefR[7];
	double thunderR;
	
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
	
	double vibM;
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
struct _dram {
	double aIL[6480];
	double aJL[3660];
	double aKL[1720];
	double aLL[680];
	double aAL[9700];
	double aBL[6000];
	double aCL[2320];
	double aDL[940];
	double aEL[15220];
	double aFL[8460];
	double aGL[4540];
	double aHL[3200];
	double aML[3111];
	double aMR[3111];
	double aIR[6480];
	double aJR[3660];
	double aKR[1720];
	double aLR[680];
	double aAR[9700];
	double aBR[6000];
	double aCR[2320];
	double aDR[940];
	double aER[15220];
	double aFR[8460];
	double aGR[4540];
	double aHR[3200];
};
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
	
	double regen = 0.0625+((1.0-GetParameter( kParam_One ))*0.0625);
	double attenuate = (1.0 - (regen / 0.125))*1.333;
	double lowpass = pow(1.00001-(1.0-GetParameter( kParam_Two )),2.0)/sqrt(overallscale);
	double drift = pow(GetParameter( kParam_Three ),3)*0.001;
	double size = (GetParameter( kParam_Four )*1.77)+0.1;
	double wet = 1.0-(pow(1.0-GetParameter( kParam_Five ),3));
	
	delayI = 3407.0*size;
	delayJ = 1823.0*size;
	delayK = 859.0*size;
	delayL = 331.0*size;
	delayA = 4801.0*size;
	delayB = 2909.0*size;
	delayC = 1153.0*size;
	delayD = 461.0*size;
	delayE = 7607.0*size;
	delayF = 4217.0*size;
	delayG = 2269.0*size;
	delayH = 1597.0*size;
	delayM = 256;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		vibM += (oldfpd*drift);
		if (vibM > (3.141592653589793238*2.0)) {
			vibM = 0.0;
			oldfpd = 0.4294967295+(fpdL*0.0000000000618);
		}
		
		dram->aML[countM] = inputSampleL * attenuate;
		dram->aMR[countM] = inputSampleR * attenuate;
		countM++; if (countM < 0 || countM > delayM) countM = 0;
		
		double offsetML = (sin(vibM)+1.0)*127;
		double offsetMR = (sin(vibM+(3.141592653589793238/2.0))+1.0)*127;
		int workingML = countM + offsetML;
		int workingMR = countM + offsetMR;
		double interpolML = (dram->aML[workingML-((workingML > delayM)?delayM+1:0)] * (1-(offsetML-floor(offsetML))));
		interpolML += (dram->aML[workingML+1-((workingML+1 > delayM)?delayM+1:0)] * ((offsetML-floor(offsetML))) );
		double interpolMR = (dram->aMR[workingMR-((workingMR > delayM)?delayM+1:0)] * (1-(offsetMR-floor(offsetMR))));
		interpolMR += (dram->aMR[workingMR+1-((workingMR+1 > delayM)?delayM+1:0)] * ((offsetMR-floor(offsetMR))) );
		inputSampleL = interpolML;
		inputSampleR = interpolMR;
		//predelay that applies vibrato
		//want vibrato speed AND depth like in MatrixVerb
		
		iirAL = (iirAL*(1.0-lowpass))+(inputSampleL*lowpass); inputSampleL = iirAL;
		iirAR = (iirAR*(1.0-lowpass))+(inputSampleR*lowpass); inputSampleR = iirAR;
		//initial filter
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			dram->aIL[countI] = inputSampleL + (feedbackAR * regen);
			dram->aJL[countJ] = inputSampleL + (feedbackBR * regen);
			dram->aKL[countK] = inputSampleL + (feedbackCR * regen);
			dram->aLL[countL] = inputSampleL + (feedbackDR * regen);
			dram->aIR[countI] = inputSampleR + (feedbackAL * regen);
			dram->aJR[countJ] = inputSampleR + (feedbackBL * regen);
			dram->aKR[countK] = inputSampleR + (feedbackCL * regen);
			dram->aLR[countL] = inputSampleR + (feedbackDL * regen);
			
			countI++; if (countI < 0 || countI > delayI) countI = 0;
			countJ++; if (countJ < 0 || countJ > delayJ) countJ = 0;
			countK++; if (countK < 0 || countK > delayK) countK = 0;
			countL++; if (countL < 0 || countL > delayL) countL = 0;
			
			double outIL = dram->aIL[countI-((countI > delayI)?delayI+1:0)];
			double outJL = dram->aJL[countJ-((countJ > delayJ)?delayJ+1:0)];
			double outKL = dram->aKL[countK-((countK > delayK)?delayK+1:0)];
			double outLL = dram->aLL[countL-((countL > delayL)?delayL+1:0)];
			double outIR = dram->aIR[countI-((countI > delayI)?delayI+1:0)];
			double outJR = dram->aJR[countJ-((countJ > delayJ)?delayJ+1:0)];
			double outKR = dram->aKR[countK-((countK > delayK)?delayK+1:0)];
			double outLR = dram->aLR[countL-((countL > delayL)?delayL+1:0)];
			//first block: now we have four outputs
			
			dram->aAL[countA] = (outIL - (outJL + outKL + outLL));
			dram->aBL[countB] = (outJL - (outIL + outKL + outLL));
			dram->aCL[countC] = (outKL - (outIL + outJL + outLL));
			dram->aDL[countD] = (outLL - (outIL + outJL + outKL));
			dram->aAR[countA] = (outIR - (outJR + outKR + outLR));
			dram->aBR[countB] = (outJR - (outIR + outKR + outLR));
			dram->aCR[countC] = (outKR - (outIR + outJR + outLR));
			dram->aDR[countD] = (outLR - (outIR + outJR + outKR));
			
			countA++; if (countA < 0 || countA > delayA) countA = 0;
			countB++; if (countB < 0 || countB > delayB) countB = 0;
			countC++; if (countC < 0 || countC > delayC) countC = 0;
			countD++; if (countD < 0 || countD > delayD) countD = 0;
			
			double outAL = dram->aAL[countA-((countA > delayA)?delayA+1:0)];
			double outBL = dram->aBL[countB-((countB > delayB)?delayB+1:0)];
			double outCL = dram->aCL[countC-((countC > delayC)?delayC+1:0)];
			double outDL = dram->aDL[countD-((countD > delayD)?delayD+1:0)];
			double outAR = dram->aAR[countA-((countA > delayA)?delayA+1:0)];
			double outBR = dram->aBR[countB-((countB > delayB)?delayB+1:0)];
			double outCR = dram->aCR[countC-((countC > delayC)?delayC+1:0)];
			double outDR = dram->aDR[countD-((countD > delayD)?delayD+1:0)];
			//second block: four more outputs
			
			dram->aEL[countE] = (outAL - (outBL + outCL + outDL));
			dram->aFL[countF] = (outBL - (outAL + outCL + outDL));
			dram->aGL[countG] = (outCL - (outAL + outBL + outDL));
			dram->aHL[countH] = (outDL - (outAL + outBL + outCL));
			dram->aER[countE] = (outAR - (outBR + outCR + outDR));
			dram->aFR[countF] = (outBR - (outAR + outCR + outDR));
			dram->aGR[countG] = (outCR - (outAR + outBR + outDR));
			dram->aHR[countH] = (outDR - (outAR + outBR + outCR));
			
			countE++; if (countE < 0 || countE > delayE) countE = 0;
			countF++; if (countF < 0 || countF > delayF) countF = 0;
			countG++; if (countG < 0 || countG > delayG) countG = 0;
			countH++; if (countH < 0 || countH > delayH) countH = 0;
			
			double outEL = dram->aEL[countE-((countE > delayE)?delayE+1:0)];
			double outFL = dram->aFL[countF-((countF > delayF)?delayF+1:0)];
			double outGL = dram->aGL[countG-((countG > delayG)?delayG+1:0)];
			double outHL = dram->aHL[countH-((countH > delayH)?delayH+1:0)];
			double outER = dram->aER[countE-((countE > delayE)?delayE+1:0)];
			double outFR = dram->aFR[countF-((countF > delayF)?delayF+1:0)];
			double outGR = dram->aGR[countG-((countG > delayG)?delayG+1:0)];
			double outHR = dram->aHR[countH-((countH > delayH)?delayH+1:0)];
			//third block: final outputs
			
			feedbackAL = (outEL - (outFL + outGL + outHL));
			feedbackBL = (outFL - (outEL + outGL + outHL));
			feedbackCL = (outGL - (outEL + outFL + outHL));
			feedbackDL = (outHL - (outEL + outFL + outGL));
			feedbackAR = (outER - (outFR + outGR + outHR));
			feedbackBR = (outFR - (outER + outGR + outHR));
			feedbackCR = (outGR - (outER + outFR + outHR));
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
		
		iirBL = (iirBL*(1.0-lowpass))+(inputSampleL*lowpass); inputSampleL = iirBL;
		iirBR = (iirBR*(1.0-lowpass))+(inputSampleR*lowpass); inputSampleR = iirBR;
		//end filter
		
		if (wet < 1.0) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0-wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0-wet));
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
	iirAL = 0.0; iirAR = 0.0;
	iirBL = 0.0; iirBR = 0.0;
	
	for(int count = 0; count < 6479; count++) {dram->aIL[count] = 0.0;dram->aIR[count] = 0.0;}
	for(int count = 0; count < 3659; count++) {dram->aJL[count] = 0.0;dram->aJR[count] = 0.0;}
	for(int count = 0; count < 1719; count++) {dram->aKL[count] = 0.0;dram->aKR[count] = 0.0;}
	for(int count = 0; count < 679; count++) {dram->aLL[count] = 0.0;dram->aLR[count] = 0.0;}
	
	for(int count = 0; count < 9699; count++) {dram->aAL[count] = 0.0;dram->aAR[count] = 0.0;}
	for(int count = 0; count < 5999; count++) {dram->aBL[count] = 0.0;dram->aBR[count] = 0.0;}
	for(int count = 0; count < 2319; count++) {dram->aCL[count] = 0.0;dram->aCR[count] = 0.0;}
	for(int count = 0; count < 939; count++) {dram->aDL[count] = 0.0;dram->aDR[count] = 0.0;}
	
	for(int count = 0; count < 15219; count++) {dram->aEL[count] = 0.0;dram->aER[count] = 0.0;}
	for(int count = 0; count < 8459; count++) {dram->aFL[count] = 0.0;dram->aFR[count] = 0.0;}
	for(int count = 0; count < 4539; count++) {dram->aGL[count] = 0.0;dram->aGR[count] = 0.0;}
	for(int count = 0; count < 3199; count++) {dram->aHL[count] = 0.0;dram->aHR[count] = 0.0;}
	
	for(int count = 0; count < 3110; count++) {dram->aML[count] = dram->aMR[count] = 0.0;}	
	
	feedbackAL = 0.0; feedbackAR = 0.0;
	feedbackBL = 0.0; feedbackBR = 0.0;
	feedbackCL = 0.0; feedbackCR = 0.0;
	feedbackDL = 0.0; feedbackDR = 0.0;
	
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0;lastRefR[count] = 0.0;}
	
	thunderL = 0; thunderR = 0;
	
	countI = 1;
	countJ = 1;
	countK = 1;
	countL = 1;
	
	countA = 1;
	countB = 1;
	countC = 1;
	countD = 1;	
	
	countE = 1;
	countF = 1;
	countG = 1;
	countH = 1;
	countM = 1;
	//the predelay
	cycle = 0;
	
	vibM = 3.0;
	
	oldfpd = 429496.7295;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
