#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Chamber"
#define AIRWINDOWS_DESCRIPTION "A feedforward reverb based on the golden ratio."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','a' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Bigness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Longness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Liteness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darkness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 iirA;
		Float64 iirB;
		Float64 iirC;
		
		
		Float64 feedbackA;
		Float64 feedbackB;
		Float64 feedbackC;
		Float64 feedbackD;
		Float64 previousA;
		Float64 previousB;
		Float64 previousC;
		Float64 previousD;
		
		double lastRef[10];
		int cycle;
		
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
		
		uint32_t fpd;
	
	struct _dram {
			Float64 aE[20000];
		Float64 aF[12361];
		Float64 aG[7640];
		Float64 aH[4722];
		Float64 aA[2916];
		Float64 aB[1804];
		Float64 aC[1115];
		Float64 aD[689];
		Float64 aI[426];
		Float64 aJ[264];
		Float64 aK[163];
		Float64 aL[101];
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	Float64 size = (pow(GetParameter( kParam_One ),2)*0.9)+0.1;
	Float64 regen = (1.0-(pow(1.0-GetParameter( kParam_Two ),6)))*0.123;
	Float64 highpass = (pow(GetParameter( kParam_Three ),2.0))/sqrt(overallscale);
	Float64 lowpass = (1.0-pow(GetParameter( kParam_Four ),2.0))/sqrt(overallscale);
	Float64 interpolate = size*0.381966011250105;
	Float64 wet = GetParameter( kParam_Five )*2.0;
	Float64 dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	delayE = 19900*size;
	delayF = delayE*0.618033988749894848204586; 
	delayG = delayF*0.618033988749894848204586;
	delayH = delayG*0.618033988749894848204586;
	delayA = delayH*0.618033988749894848204586;
	delayB = delayA*0.618033988749894848204586;
	delayC = delayB*0.618033988749894848204586;
	delayD = delayC*0.618033988749894848204586;
	delayI = delayD*0.618033988749894848204586;
	delayJ = delayI*0.618033988749894848204586;
	delayK = delayJ*0.618033988749894848204586;
	delayL = delayK*0.618033988749894848204586;
	//initially designed around the Fibonnaci series, Chamber uses
	//delay coefficients that are all related to the Golden Ratio,
	//Turns out that as you continue to sustain them, it turns from a
	//chunky slapback effect into a smoother reverb tail that can
	//sustain infinitely.	
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		iirC = (iirC*(1.0-highpass))+(inputSample*highpass); inputSample -= iirC;
		//initial highpass
		
		iirA = (iirA*(1.0-lowpass))+(inputSample*lowpass); inputSample = iirA;
		//initial filter
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			
			feedbackA = (feedbackA*(1.0-interpolate))+(previousA*interpolate); previousA = feedbackA;
			feedbackB = (feedbackB*(1.0-interpolate))+(previousB*interpolate); previousB = feedbackB;
			feedbackC = (feedbackC*(1.0-interpolate))+(previousC*interpolate); previousC = feedbackC;
			feedbackD = (feedbackD*(1.0-interpolate))+(previousD*interpolate); previousD = feedbackD;
			
			dram->aI[countI] = inputSample + (feedbackA * regen);
			dram->aJ[countJ] = inputSample + (feedbackB * regen);
			dram->aK[countK] = inputSample + (feedbackC * regen);
			dram->aL[countL] = inputSample + (feedbackD * regen);
			
			countI++; if (countI < 0 || countI > delayI) countI = 0;
			countJ++; if (countJ < 0 || countJ > delayJ) countJ = 0;
			countK++; if (countK < 0 || countK > delayK) countK = 0;
			countL++; if (countL < 0 || countL > delayL) countL = 0;
			
			Float64 outI = dram->aI[countI-((countI > delayI)?delayI+1:0)];
			Float64 outJ = dram->aJ[countJ-((countJ > delayJ)?delayJ+1:0)];
			Float64 outK = dram->aK[countK-((countK > delayK)?delayK+1:0)];
			Float64 outL = dram->aL[countL-((countL > delayL)?delayL+1:0)];
			//first block: now we have four outputs
			
			dram->aA[countA] = (outI - (outJ + outK + outL));
			dram->aB[countB] = (outJ - (outI + outK + outL));
			dram->aC[countC] = (outK - (outI + outJ + outL));
			dram->aD[countD] = (outL - (outI + outJ + outK));
			
			countA++; if (countA < 0 || countA > delayA) countA = 0;
			countB++; if (countB < 0 || countB > delayB) countB = 0;
			countC++; if (countC < 0 || countC > delayC) countC = 0;
			countD++; if (countD < 0 || countD > delayD) countD = 0;
			
			Float64 outA = dram->aA[countA-((countA > delayA)?delayA+1:0)];
			Float64 outB = dram->aB[countB-((countB > delayB)?delayB+1:0)];
			Float64 outC = dram->aC[countC-((countC > delayC)?delayC+1:0)];
			Float64 outD = dram->aD[countD-((countD > delayD)?delayD+1:0)];
			//second block: four more outputs
			
			dram->aE[countE] = (outA - (outB + outC + outD));
			dram->aF[countF] = (outB - (outA + outC + outD));
			dram->aG[countG] = (outC - (outA + outB + outD));
			dram->aH[countH] = (outD - (outA + outB + outC));
			
			countE++; if (countE < 0 || countE > delayE) countE = 0;
			countF++; if (countF < 0 || countF > delayF) countF = 0;
			countG++; if (countG < 0 || countG > delayG) countG = 0;
			countH++; if (countH < 0 || countH > delayH) countH = 0;
			
			Float64 outE = dram->aE[countE-((countE > delayE)?delayE+1:0)];
			Float64 outF = dram->aF[countF-((countF > delayF)?delayF+1:0)];
			Float64 outG = dram->aG[countG-((countG > delayG)?delayG+1:0)];
			Float64 outH = dram->aH[countH-((countH > delayH)?delayH+1:0)];
			//third block: final outputs
			
			feedbackA = (outE - (outF + outG + outH));
			feedbackB = (outF - (outE + outG + outH));
			feedbackC = (outG - (outE + outF + outH));
			feedbackD = (outH - (outE + outF + outG));
			//which we need to feed back into the input again, a bit
			
			inputSample = (outE + outF + outG + outH)/8.0;
			//and take the final combined sum of outputs
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		
		iirB = (iirB*(1.0-lowpass))+(inputSample*lowpass); inputSample = iirB;
		//end filter
		
		if (wet < 1.0) inputSample *= wet;
		if (dry < 1.0) drySample *= dry;
		inputSample += drySample;
		//this is our submix verb dry/wet: 0.5 is BOTH at FULL VOLUME
		//purpose is that, if you're adding verb, you're not altering other balances
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirA = 0.0;
	iirB = 0.0;
	iirC = 0.0;
	
	for(int count = 0; count < 19999; count++) {dram->aE[count] = 0.0;}
	for(int count = 0; count < 12360; count++) {dram->aF[count] = 0.0;}
	for(int count = 0; count < 7639; count++) {dram->aG[count] = 0.0;}
	for(int count = 0; count < 4721; count++) {dram->aH[count] = 0.0;}
	for(int count = 0; count < 2915; count++) {dram->aA[count] = 0.0;}
	for(int count = 0; count < 1803; count++) {dram->aB[count] = 0.0;}
	for(int count = 0; count < 1114; count++) {dram->aC[count] = 0.0;}
	for(int count = 0; count < 688; count++) {dram->aD[count] = 0.0;}
	for(int count = 0; count < 425; count++) {dram->aI[count] = 0.0;}
	for(int count = 0; count < 263; count++) {dram->aJ[count] = 0.0;}
	for(int count = 0; count < 162; count++) {dram->aK[count] = 0.0;}
	for(int count = 0; count < 100; count++) {dram->aL[count] = 0.0;}
	
	feedbackA = 0.0;
	feedbackB = 0.0;
	feedbackC = 0.0;
	feedbackD = 0.0;
	previousA = 0.0;
	previousB = 0.0;
	previousC = 0.0;
	previousD = 0.0;
	
	for(int count = 0; count < 9; count++) {lastRef[count] = 0.0;}
	cycle = 0;
	
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
	
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
