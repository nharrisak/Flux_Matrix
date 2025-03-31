#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Galactic3"
#define AIRWINDOWS_DESCRIPTION "Exactly Galactic again, but with better undersampling."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','n' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Replace", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Brightness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Detune", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Derez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bigness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double iirAL;
	double iirBL;
	
	
	
	
	double vibML, vibMR, depthM, oldfpd;
	
	double feedbackAL;
	double feedbackBL;
	double feedbackCL;
	double feedbackDL;
	
	double iirAR;
	double iirBR;
	
	
	
	
	double feedbackAR;
	double feedbackBR;
	double feedbackCR;
	double feedbackDR;
		
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
	int countM, delayM; //all these ints are shared across channels, not duplicated
	
	double vibM;
	
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
	
	double regen = 0.0625+((1.0-GetParameter( kParam_A ))*0.0625);
	double attenuate = (1.0 - (regen / 0.125))*1.333;
	double lowpass = pow(1.00001-(1.0-GetParameter( kParam_B )),2.0)/sqrt(overallscale);
	double drift = pow(GetParameter( kParam_C ),3)*0.001;
	double derez = GetParameter( kParam_D )/overallscale;
	if (derez < 0.0005) derez = 0.0005; if (derez > 1.0) derez = 1.0;
	derez = 1.0 / ((int)(1.0/derez));
	//this hard-locks derez to exact subdivisions of 1.0
	double size = (GetParameter( kParam_E )*1.77)+0.1;
	double wet = 1.0-(pow(1.0-GetParameter( kParam_F ),3));
	
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
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0;
			
			dram->aIL[countI] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackAR * regen);
			dram->aJL[countJ] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackBR * regen);
			dram->aKL[countK] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackCR * regen);
			dram->aLL[countL] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackDR * regen);
			dram->bez[bez_UnInL] = dram->bez[bez_SampL];
			
			dram->aIR[countI] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackAL * regen);
			dram->aJR[countJ] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackBL * regen);
			dram->aKR[countK] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackCL * regen);
			dram->aLR[countL] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackDL * regen);
			dram->bez[bez_UnInR] = dram->bez[bez_SampR];
			
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
	
	vibM = 3.0;
	
	oldfpd = 429496.7295;
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;

	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
