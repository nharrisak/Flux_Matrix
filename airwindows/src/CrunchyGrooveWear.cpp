#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "CrunchyGrooveWear"
#define AIRWINDOWS_DESCRIPTION "A version of GrooveWear for more edge and distortion."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','r','v' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Freq", .min = 0, .max = 1000, .def = 64, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Apply", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 aMid[21];
		Float64 bMid[21];
		Float64 cMid[21];
		Float64 dMid[21];
		Float64 fMid[21];		
		Float64 aMidPrev;
		Float64 bMidPrev;
		Float64 cMidPrev;
		Float64 dMidPrev;
		
		uint32_t fpd;
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
	Float64 overallscale = (pow(GetParameter( kParam_One ),2)*19.0)+1.0;
	Float64 gain = overallscale;
	//mid groove wear
	if (gain > 1.0) {fMid[0] = 1.0; gain -= 1.0;} else {fMid[0] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[1] = 1.0; gain -= 1.0;} else {fMid[1] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[2] = 1.0; gain -= 1.0;} else {fMid[2] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[3] = 1.0; gain -= 1.0;} else {fMid[3] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[4] = 1.0; gain -= 1.0;} else {fMid[4] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[5] = 1.0; gain -= 1.0;} else {fMid[5] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[6] = 1.0; gain -= 1.0;} else {fMid[6] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[7] = 1.0; gain -= 1.0;} else {fMid[7] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[8] = 1.0; gain -= 1.0;} else {fMid[8] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[9] = 1.0; gain -= 1.0;} else {fMid[9] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[10] = 1.0; gain -= 1.0;} else {fMid[10] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[11] = 1.0; gain -= 1.0;} else {fMid[11] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[12] = 1.0; gain -= 1.0;} else {fMid[12] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[13] = 1.0; gain -= 1.0;} else {fMid[13] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[14] = 1.0; gain -= 1.0;} else {fMid[14] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[15] = 1.0; gain -= 1.0;} else {fMid[15] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[16] = 1.0; gain -= 1.0;} else {fMid[16] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[17] = 1.0; gain -= 1.0;} else {fMid[17] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[18] = 1.0; gain -= 1.0;} else {fMid[18] = gain; gain = 0.0;}
	if (gain > 1.0) {fMid[19] = 1.0; gain -= 1.0;} else {fMid[19] = gain; gain = 0.0;}
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0) overallscale = 1.0;
	fMid[0] /= overallscale;
	fMid[1] /= overallscale;
	fMid[2] /= overallscale;
	fMid[3] /= overallscale;
	fMid[4] /= overallscale;
	fMid[5] /= overallscale;
	fMid[6] /= overallscale;
	fMid[7] /= overallscale;
	fMid[8] /= overallscale;
	fMid[9] /= overallscale;
	fMid[10] /= overallscale;
	fMid[11] /= overallscale;
	fMid[12] /= overallscale;
	fMid[13] /= overallscale;
	fMid[14] /= overallscale;
	fMid[15] /= overallscale;
	fMid[16] /= overallscale;
	fMid[17] /= overallscale;
	fMid[18] /= overallscale;
	fMid[19] /= overallscale;
	//and now it's neatly scaled, too
	
	Float64 aWet = 1.0;
	Float64 bWet = 1.0;
	Float64 cWet = 1.0;
	Float64 dWet = GetParameter( kParam_Two )*4.0;
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0) {aWet = dWet; bWet = 0.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 2.0) {bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0;}
	else if (dWet < 3.0) {cWet = dWet - 2.0; dWet = 0.0;}
	else {dWet -= 3.0;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	
	//now we set them up so each full intensity one is blended evenly with dry for each stage.
	//That's because the GrooveWear algorithm works best combined with dry.
	//aWet *= 0.5;
	//bWet *= 0.5; This was the tweak which caused GrooveWear to be dark instead of distorty
	//cWet *= 0.5; Disabling this causes engaged stages to take on an edge, but 0.5 settings
	//dWet *= 0.5; for any stage will still produce a darker tone.
	// This will make the behavior of the plugin more complex
	//if you are using a more typical algorithm (like a sin() or something) you won't use this part
	
	Float64 aDry = 1.0 - aWet;
	Float64 bDry = 1.0 - bWet;
	Float64 cDry = 1.0 - cWet;
	Float64 dDry = 1.0 - dWet;
	
	Float64 drySample;
	double inputSample;
	Float64 accumulatorSample;
	Float64 correction;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;		
		
		if (aWet > 0.0) {
			aMid[19] = aMid[18]; aMid[18] = aMid[17]; aMid[17] = aMid[16]; aMid[16] = aMid[15];
			aMid[15] = aMid[14]; aMid[14] = aMid[13]; aMid[13] = aMid[12]; aMid[12] = aMid[11];
			aMid[11] = aMid[10]; aMid[10] = aMid[9];
			aMid[9] = aMid[8]; aMid[8] = aMid[7]; aMid[7] = aMid[6]; aMid[6] = aMid[5];
			aMid[5] = aMid[4]; aMid[4] = aMid[3]; aMid[3] = aMid[2]; aMid[2] = aMid[1];
			aMid[1] = aMid[0]; aMid[0] = accumulatorSample = (inputSample-aMidPrev);
			//this is different from Aura because that is accumulating rates of change OF the rate of change
			accumulatorSample *= fMid[0];
			accumulatorSample += (aMid[1] * fMid[1]);
			accumulatorSample += (aMid[2] * fMid[2]);
			accumulatorSample += (aMid[3] * fMid[3]);
			accumulatorSample += (aMid[4] * fMid[4]);
			accumulatorSample += (aMid[5] * fMid[5]);
			accumulatorSample += (aMid[6] * fMid[6]);
			accumulatorSample += (aMid[7] * fMid[7]);
			accumulatorSample += (aMid[8] * fMid[8]);
			accumulatorSample += (aMid[9] * fMid[9]);
			accumulatorSample += (aMid[10] * fMid[10]);
			accumulatorSample += (aMid[11] * fMid[11]);
			accumulatorSample += (aMid[12] * fMid[12]);
			accumulatorSample += (aMid[13] * fMid[13]);
			accumulatorSample += (aMid[14] * fMid[14]);
			accumulatorSample += (aMid[15] * fMid[15]);
			accumulatorSample += (aMid[16] * fMid[16]);
			accumulatorSample += (aMid[17] * fMid[17]);
			accumulatorSample += (aMid[18] * fMid[18]);
			accumulatorSample += (aMid[19] * fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-aMidPrev) - accumulatorSample;
			aMidPrev = inputSample;		
			inputSample -= correction;
			inputSample = (inputSample * aWet) + (drySample * aDry);
			drySample = inputSample;		
		}
		
		if (bWet > 0.0) {
			bMid[19] = bMid[18]; bMid[18] = bMid[17]; bMid[17] = bMid[16]; bMid[16] = bMid[15];
			bMid[15] = bMid[14]; bMid[14] = bMid[13]; bMid[13] = bMid[12]; bMid[12] = bMid[11];
			bMid[11] = bMid[10]; bMid[10] = bMid[9];
			bMid[9] = bMid[8]; bMid[8] = bMid[7]; bMid[7] = bMid[6]; bMid[6] = bMid[5];
			bMid[5] = bMid[4]; bMid[4] = bMid[3]; bMid[3] = bMid[2]; bMid[2] = bMid[1];
			bMid[1] = bMid[0]; bMid[0] = accumulatorSample = (inputSample-bMidPrev);
			
			accumulatorSample *= fMid[0];
			accumulatorSample += (bMid[1] * fMid[1]);
			accumulatorSample += (bMid[2] * fMid[2]);
			accumulatorSample += (bMid[3] * fMid[3]);
			accumulatorSample += (bMid[4] * fMid[4]);
			accumulatorSample += (bMid[5] * fMid[5]);
			accumulatorSample += (bMid[6] * fMid[6]);
			accumulatorSample += (bMid[7] * fMid[7]);
			accumulatorSample += (bMid[8] * fMid[8]);
			accumulatorSample += (bMid[9] * fMid[9]);
			accumulatorSample += (bMid[10] * fMid[10]);
			accumulatorSample += (bMid[11] * fMid[11]);
			accumulatorSample += (bMid[12] * fMid[12]);
			accumulatorSample += (bMid[13] * fMid[13]);
			accumulatorSample += (bMid[14] * fMid[14]);
			accumulatorSample += (bMid[15] * fMid[15]);
			accumulatorSample += (bMid[16] * fMid[16]);
			accumulatorSample += (bMid[17] * fMid[17]);
			accumulatorSample += (bMid[18] * fMid[18]);
			accumulatorSample += (bMid[19] * fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-bMidPrev) - accumulatorSample;
			bMidPrev = inputSample;
			inputSample -= correction;
			inputSample = (inputSample * bWet) + (drySample * bDry);
			drySample = inputSample;		
		}
		
		if (cWet > 0.0) {
			cMid[19] = cMid[18]; cMid[18] = cMid[17]; cMid[17] = cMid[16]; cMid[16] = cMid[15];
			cMid[15] = cMid[14]; cMid[14] = cMid[13]; cMid[13] = cMid[12]; cMid[12] = cMid[11];
			cMid[11] = cMid[10]; cMid[10] = cMid[9];
			cMid[9] = cMid[8]; cMid[8] = cMid[7]; cMid[7] = cMid[6]; cMid[6] = cMid[5];
			cMid[5] = cMid[4]; cMid[4] = cMid[3]; cMid[3] = cMid[2]; cMid[2] = cMid[1];
			cMid[1] = cMid[0]; cMid[0] = accumulatorSample = (inputSample-cMidPrev);
			
			accumulatorSample *= fMid[0];
			accumulatorSample += (cMid[1] * fMid[1]);
			accumulatorSample += (cMid[2] * fMid[2]);
			accumulatorSample += (cMid[3] * fMid[3]);
			accumulatorSample += (cMid[4] * fMid[4]);
			accumulatorSample += (cMid[5] * fMid[5]);
			accumulatorSample += (cMid[6] * fMid[6]);
			accumulatorSample += (cMid[7] * fMid[7]);
			accumulatorSample += (cMid[8] * fMid[8]);
			accumulatorSample += (cMid[9] * fMid[9]);
			accumulatorSample += (cMid[10] * fMid[10]);
			accumulatorSample += (cMid[11] * fMid[11]);
			accumulatorSample += (cMid[12] * fMid[12]);
			accumulatorSample += (cMid[13] * fMid[13]);
			accumulatorSample += (cMid[14] * fMid[14]);
			accumulatorSample += (cMid[15] * fMid[15]);
			accumulatorSample += (cMid[16] * fMid[16]);
			accumulatorSample += (cMid[17] * fMid[17]);
			accumulatorSample += (cMid[18] * fMid[18]);
			accumulatorSample += (cMid[19] * fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-cMidPrev) - accumulatorSample;
			cMidPrev = inputSample;
			inputSample -= correction;
			inputSample = (inputSample * cWet) + (drySample * cDry);
			drySample = inputSample;		
		}
		
		if (dWet > 0.0) {
			dMid[19] = dMid[18]; dMid[18] = dMid[17]; dMid[17] = dMid[16]; dMid[16] = dMid[15];
			dMid[15] = dMid[14]; dMid[14] = dMid[13]; dMid[13] = dMid[12]; dMid[12] = dMid[11];
			dMid[11] = dMid[10]; dMid[10] = dMid[9];
			dMid[9] = dMid[8]; dMid[8] = dMid[7]; dMid[7] = dMid[6]; dMid[6] = dMid[5];
			dMid[5] = dMid[4]; dMid[4] = dMid[3]; dMid[3] = dMid[2]; dMid[2] = dMid[1];
			dMid[1] = dMid[0]; dMid[0] = accumulatorSample = (inputSample-dMidPrev);
			
			accumulatorSample *= fMid[0];
			accumulatorSample += (dMid[1] * fMid[1]);
			accumulatorSample += (dMid[2] * fMid[2]);
			accumulatorSample += (dMid[3] * fMid[3]);
			accumulatorSample += (dMid[4] * fMid[4]);
			accumulatorSample += (dMid[5] * fMid[5]);
			accumulatorSample += (dMid[6] * fMid[6]);
			accumulatorSample += (dMid[7] * fMid[7]);
			accumulatorSample += (dMid[8] * fMid[8]);
			accumulatorSample += (dMid[9] * fMid[9]);
			accumulatorSample += (dMid[10] * fMid[10]);
			accumulatorSample += (dMid[11] * fMid[11]);
			accumulatorSample += (dMid[12] * fMid[12]);
			accumulatorSample += (dMid[13] * fMid[13]);
			accumulatorSample += (dMid[14] * fMid[14]);
			accumulatorSample += (dMid[15] * fMid[15]);
			accumulatorSample += (dMid[16] * fMid[16]);
			accumulatorSample += (dMid[17] * fMid[17]);
			accumulatorSample += (dMid[18] * fMid[18]);
			accumulatorSample += (dMid[19] * fMid[19]);
			//we are doing our repetitive calculations on a separate value
			correction = (inputSample-dMidPrev) - accumulatorSample;
			dMidPrev = inputSample;
			inputSample -= correction;
			inputSample = (inputSample * dWet) + (drySample * dDry);
		}
		
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
	for(int count = 0; count < 21; count++) {
		aMid[count] = 0.0;
		bMid[count] = 0.0;
		cMid[count] = 0.0;
		dMid[count] = 0.0;
		fMid[count] = 0.0;
	}
	aMidPrev = 0.0;
	bMidPrev = 0.0;
	cMidPrev = 0.0;
	dMidPrev = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
