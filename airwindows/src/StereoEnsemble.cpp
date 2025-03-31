#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "StereoEnsemble"
#define AIRWINDOWS_DESCRIPTION "A sort of hyperchorus blast from the past."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','g' )
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
{ .name = "Depth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Effect Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
		double dA[7491];
		double dB[7533];
		double dC[5789];
		double dD[5747];		
		double dE[4841];
		double dF[4871];
		double dG[3119];
		double dH[3089];
		double dI[2213];
		double dJ[2223];
		double dK[1337];
		double dL[1331];
		
		double levelA;
		double levelB;
		double levelC;
		double levelD;
		double levelE;
		double levelF;
		
		double levelG;
		double levelH;
		double levelI;
		double levelJ;
		double levelK;
		double levelL;
		
		double dryL;
		double dryR;
		
		int oneA, maxdelayA;
		int oneB, maxdelayB;
		int oneC, maxdelayC;
		int oneD, maxdelayD;
		int oneE, maxdelayE;
		int oneF, maxdelayF;
		
		int oneG, maxdelayG;
		int oneH, maxdelayH;
		int oneI, maxdelayI;
		int oneJ, maxdelayJ;
		int oneK, maxdelayK;
		int oneL, maxdelayL;
			
		double lastRefL[7];
		double lastRefR[7];
		int cycle;
	
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
	
	double delayfactor = 0.66 + (GetParameter( kParam_One )/3.0);
	
	double outlevel = GetParameter( kParam_Two );
	
	dryL = 1.0 - (outlevel * 0.65);
	dryR = 1.0 - (outlevel * 0.65);
	
	maxdelayA = (int)(7481.0 * delayfactor);
	maxdelayB = (int)(7523.0 * delayfactor);

	maxdelayC = (int)(5779.0 * delayfactor);
	maxdelayD = (int)(5737.0 * delayfactor);
	
	maxdelayE = (int)(4831.0 * delayfactor);
	maxdelayF = (int)(4861.0 * delayfactor);
	
	maxdelayG = (int)(3109.0 * delayfactor);
	maxdelayH = (int)(3079.0 * delayfactor);
	
	maxdelayI = (int)(2203.0 * delayfactor);
	maxdelayJ = (int)(2213.0 * delayfactor);
	
	maxdelayK = (int)(1327.0 * delayfactor);
	maxdelayL = (int)(1321.0 * delayfactor);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a ensemble sample
			double drySampleL = inputSampleL;
			double drySampleR = inputSampleR;
			double outL = 0.0;
			double outR = 0.0;
			double temp = 0.0;
			
			dA[oneA] = inputSampleL;
			oneA--; if (oneA < 0 || oneA > maxdelayA) {oneA = maxdelayA;} temp = dA[oneA];
			outL += (temp*levelA);
			
			dB[oneB] = inputSampleR;
			oneB--; if (oneB < 0 || oneB > maxdelayB) {oneB = maxdelayB;} temp += dB[oneB];
			outR += (temp*levelB);
			
			dC[oneC] = inputSampleL;
			oneC--; if (oneC < 0 || oneC > maxdelayC) {oneC = maxdelayC;} temp = dC[oneC];
			outL += (temp*levelC);
			
			dD[oneD] = inputSampleR;
			oneD--; if (oneD < 0 || oneD > maxdelayD) {oneD = maxdelayD;} temp += dD[oneD];
			outR += (temp*levelD);
			
			dE[oneE] = inputSampleL;
			oneE--; if (oneE < 0 || oneE > maxdelayE) {oneE = maxdelayE;} temp = dE[oneE];
			outL += (temp*levelE);
			
			dF[oneF] = inputSampleR;
			oneF--; if (oneF < 0 || oneF > maxdelayF) {oneF = maxdelayF;} temp += dF[oneF];
			outR += (temp*levelF);
			
			dG[oneG] = inputSampleL;
			oneG--; if (oneG < 0 || oneG > maxdelayG) {oneG = maxdelayG;} temp = dG[oneG];
			outL += (temp*levelG);
			
			dH[oneH] = inputSampleR;
			oneH--; if (oneH < 0 || oneH > maxdelayH) {oneH = maxdelayH;} temp += dH[oneH];
			outR += (temp*levelH);
			
			dI[oneI] = inputSampleL;
			oneI--; if (oneI < 0 || oneI > maxdelayI) {oneI = maxdelayI;} temp = dI[oneI];
			outL += (temp*levelI);
			
			dJ[oneJ] = inputSampleR;
			oneJ--; if (oneJ < 0 || oneJ > maxdelayJ) {oneJ = maxdelayJ;} temp += dJ[oneJ];
			outR += (temp*levelJ);
			
			dK[oneK] = inputSampleL;
			oneK--; if (oneK < 0 || oneK > maxdelayK) {oneK = maxdelayK;} temp = dK[oneK];
			outL += (temp*levelK);
			
			dL[oneL] = inputSampleR;
			oneL--; if (oneL < 0 || oneL > maxdelayL) {oneL = maxdelayL;} temp += dL[oneL];
			outR += (temp*levelL);
			
			inputSampleL = (outL * outlevel) + (drySampleL * dryL);
			inputSampleR = (outR * outlevel) + (drySampleR * dryR);
			
			
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
	for(int count = 0; count < 7490; count++) {dA[count] = 0.0;}
	for(int count = 0; count < 7532; count++) {dB[count] = 0.0;}

	for(int count = 0; count < 5788; count++) {dC[count] = 0.0;}
	for(int count = 0; count < 5746; count++) {dD[count] = 0.0;}

	for(int count = 0; count < 4840; count++) {dE[count] = 0.0;}
	for(int count = 0; count < 4870; count++) {dF[count] = 0.0;}

	for(int count = 0; count < 3118; count++) {dG[count] = 0.0;}
	for(int count = 0; count < 3088; count++) {dH[count] = 0.0;}

	for(int count = 0; count < 2212; count++) {dI[count] = 0.0;}
	for(int count = 0; count < 2222; count++) {dJ[count] = 0.0;}

	for(int count = 0; count < 1336; count++) {dK[count] = 0.0;}
	for(int count = 0; count < 1330; count++) {dL[count] = 0.0;}
	
	oneA = 1;
	oneB = 1;
	oneC = 1;
	oneD = 1;
	oneE = 1;
	oneF = 1;
	oneG = 1;
	oneH = 1;
	oneI = 1;
	oneJ = 1;
	oneK = 1;
	oneL = 1;
		
	levelA = 0.75;
	levelB = 0.15;

	levelC = 0.15;
	levelD = 0.75;
	
	levelE = 0.63;
	levelF = 0.27;
	
	levelG = 0.27;
	levelH = 0.63;
	
	levelI = 0.48;
	levelJ = 0.32;
	
	levelK = 0.32;
	levelL = 0.48;
		
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0;lastRefR[count] = 0.0;}
	cycle = 0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
