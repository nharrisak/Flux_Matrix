#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Elliptical"
#define AIRWINDOWS_DESCRIPTION "Highpasses the side channel."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','l','l' )
enum {

	kParam_A =0,
	kParam_B =1,
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
{ .name = "Cutoff", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Slope", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double iirA;
	double iirB; //first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
	double iirC; //we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
	double iirD;
	double iirE;
	double iirF; //our slope control will have a pow() on it so that the high orders are way to the right side
	double iirG;
	double iirH; //seven poles max, and the final pole is always at 20hz directly.
	bool fpFlip;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double rangescale = 0.1 / overallscale;
	double cutoff = pow(GetParameter( kParam_A ),3);
	double slope = pow(GetParameter( kParam_B ),3) * 6.0;
	double newA = cutoff * rangescale;
	double newB = newA; //other part of interleaved IIR is the same
	double fpOld = 0.618033988749894848204586; //golden ratio!
	double newC = cutoff * rangescale; //first extra pole is the same
	cutoff = (cutoff * fpOld) + (0.00001 * (1.0-fpOld));
	double newD = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * (1.0-fpOld));
	double newE = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * (1.0-fpOld));
	double newF = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * (1.0-fpOld));
	double newG = cutoff * rangescale;
	cutoff = (cutoff * fpOld) + (0.00001 * (1.0-fpOld));
	double newH = cutoff * rangescale;
	//converge toward the unvarying fixed cutoff value
	double polesC = slope; if (slope > 1.0) polesC = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	double polesD = slope; if (slope > 1.0) polesD = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	double polesE = slope; if (slope > 1.0) polesE = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	double polesF = slope; if (slope > 1.0) polesF = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	double polesG = slope; if (slope > 1.0) polesG = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	double polesH = slope; if (slope > 1.0) polesH = 1.0; slope -= 1.0; if (slope < 0.0) slope = 0.0;
	//each one will either be 0.0, the fractional slope value, or 1
		
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double mid = inputSampleL + inputSampleR;
		double side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		double temp = side;
		double correction;
		
		if (fpFlip) {
			iirA = (iirA * (1.0 - newA)) + (temp * newA); temp -= iirA; correction = iirA;
		} else {
			iirB = (iirB * (1.0 - newB)) + (temp * newB); temp -= iirB; correction = iirB;
		}
		iirC = (iirC * (1.0 - newC)) + (temp * newC); temp -= iirC;
		iirD = (iirD * (1.0 - newD)) + (temp * newD); temp -= iirD;
		iirE = (iirE * (1.0 - newE)) + (temp * newE); temp -= iirE;
		iirF = (iirF * (1.0 - newF)) + (temp * newF); temp -= iirF;
		iirG = (iirG * (1.0 - newG)) + (temp * newG); temp -= iirG;
		iirH = (iirH * (1.0 - newH)) + (temp * newH); temp -= iirH;
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
		
		side -= correction;
		fpFlip = !fpFlip;
		
		inputSampleL = (mid+side)/2.0;
		inputSampleR = (mid-side)/2.0;
		//unassign mid and side
		
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
	iirA = 0.0;
	iirB = 0.0;
	iirC = 0.0;
	iirD = 0.0;
	iirE = 0.0;
	iirF = 0.0;
	iirG = 0.0;
	iirH = 0.0;
	fpFlip = true;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
