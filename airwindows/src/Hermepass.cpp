#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hermepass"
#define AIRWINDOWS_DESCRIPTION "A mastering highpass to set by ear only."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','e','s' )
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
{ .name = "Cutoff", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Slope", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		
		Float64 iirA;
		Float64 iirB; //first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
		Float64 iirC; //we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
		Float64 iirD;
		Float64 iirE;
		Float64 iirF; //our slope control will have a pow() on it so that the high orders are way to the right side
		Float64 iirG;
		Float64 iirH; //seven poles max, and the final pole is always at 20hz directly.
		
		
		uint32_t fpd;
		bool fpFlip;
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
	double fpOld = 0.618033988749894848204586; //golden ratio!
	double fpNew = 1.0 - fpOld;

	Float64 rangescale = 0.1 / overallscale;
	
	Float64 cutoff = pow(GetParameter( kParam_One ),3);
	Float64 slope = pow(GetParameter( kParam_Two ),3) * 6.0;
	
	Float64 newA = cutoff * rangescale;
	Float64 newB = newA; //other part of interleaved IIR is the same
	
	Float64 newC = cutoff * rangescale; //first extra pole is the same
	cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
	Float64 newD = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
	Float64 newE = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
	Float64 newF = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
	Float64 newG = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
	Float64 newH = cutoff * rangescale;
	//converge toward the unvarying fixed cutoff value
	
	Float64 oldA = 1.0 - newA;
	Float64 oldB = 1.0 - newB;
	Float64 oldC = 1.0 - newC;
	Float64 oldD = 1.0 - newD;
	Float64 oldE = 1.0 - newE;
	Float64 oldF = 1.0 - newF;
	Float64 oldG = 1.0 - newG;
	Float64 oldH = 1.0 - newH;
	
	Float64 polesC;
	Float64 polesD;
	Float64 polesE;
	Float64 polesF;
	Float64 polesG;
	Float64 polesH;
	
	polesC = slope; if (slope > 1.0) polesC = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	polesD = slope; if (slope > 1.0) polesD = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	polesE = slope; if (slope > 1.0) polesE = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	polesF = slope; if (slope > 1.0) polesF = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	polesG = slope; if (slope > 1.0) polesG = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	polesH = slope; if (slope > 1.0) polesH = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	//each one will either be 0.0, the fractional slope value, or 1
	
	double inputSample;
	Float64 tempSample;
	Float64 correction;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		tempSample = inputSample;

		if (fpFlip) {
			iirA = (iirA * oldA) + (tempSample * newA); tempSample -= iirA; correction = iirA;
		} else {
			iirB = (iirB * oldB) + (tempSample * newB); tempSample -= iirB; correction = iirB;
		}
		iirC = (iirC * oldC) + (tempSample * newC); tempSample -= iirC;
		iirD = (iirD * oldD) + (tempSample * newD); tempSample -= iirD;
		iirE = (iirE * oldE) + (tempSample * newE); tempSample -= iirE;
		iirF = (iirF * oldF) + (tempSample * newF); tempSample -= iirF;
		iirG = (iirG * oldG) + (tempSample * newG); tempSample -= iirG;
		iirH = (iirH * oldH) + (tempSample * newH); tempSample -= iirH;
		//set up all the iir filters in case they are used
		
		if (polesC == 1.0) correction += iirC; if (polesC > 0.0 && polesC < 1.0) correction += (iirC * polesC);
		if (polesD == 1.0) correction += iirD; if (polesD > 0.0 && polesD < 1.0) correction += (iirD * polesD);
		if (polesE == 1.0) correction += iirE; if (polesE > 0.0 && polesE < 1.0) correction += (iirE * polesE);
		if (polesF == 1.0) correction += iirF; if (polesF > 0.0 && polesF < 1.0) correction += (iirF * polesF);
		if (polesG == 1.0) correction += iirG; if (polesG > 0.0 && polesG < 1.0) correction += (iirG * polesG);
		if (polesH == 1.0) correction += iirH; if (polesH > 0.0 && polesH < 1.0) correction += (iirH * polesH);
		//each of these are added directly if they're fully engaged,
		//multiplied by 0-1 if they are the interpolated one, or skipped if they are beyond the interpolated one.
		//the purpose is to do all the math at the floating point exponent nearest to the tiny value in use.
		//also, it's formatted that way to easily substitute the next variable: this could be written as a loop
		//with everything an array value. However, this makes just as much sense for this few poles.
		
		inputSample -= correction;
		fpFlip = !fpFlip;
	
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
	iirD = 0.0;
	iirE = 0.0;
	iirF = 0.0;
	iirG = 0.0;
	iirH = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
