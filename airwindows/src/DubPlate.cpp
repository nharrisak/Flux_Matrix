#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DubPlate"
#define AIRWINDOWS_DESCRIPTION "Like a busy mastering house for electronic music."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','c' )
enum {

	kNumberOfParameters=0
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
};
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"

	double iirA;
	double iirB; //first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
	double iirC; //we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
	double iirD; //then there's a Mid highpass.
	bool fpFlip;
	double lastSinewAL;
	double lastSinewAR;
	double lastSinewBL;
	double lastSinewBR;
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double rangescale = 0.1 / overallscale;
	double iirSide = 0.287496 * rangescale;
	double iirMid = 0.20123 * rangescale;
	double threshSinewA = 0.15 / overallscale;
	double threshSinewB = 0.127 / overallscale;
		
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
			iirA = (iirA * (1.0 - iirSide)) + (temp * iirSide); temp -= iirA; correction = iirA;
		} else {
			iirB = (iirB * (1.0 - iirSide)) + (temp * iirSide); temp -= iirB; correction = iirB;
		}
		iirC = (iirC * (1.0 - iirSide)) + (temp * iirSide); temp -= iirC; correction += (iirC * 0.162);

		side -= sin(correction);
		fpFlip = !fpFlip;
		
		iirD = (iirD * (1.0 - iirMid)) + (mid * iirMid);
		mid -= sin(iirD);
		//gonna cut those lows a hair
				
		inputSampleL = (mid+side)/2.0;
		inputSampleR = (mid-side)/2.0;
		//unassign mid and side
		
		temp = inputSampleL;
		double sinew = threshSinewA * cos(lastSinewAL*lastSinewAL);
		if (inputSampleL - lastSinewAL > sinew) temp = lastSinewAL + sinew;
		if (-(inputSampleL - lastSinewAL) > sinew) temp = lastSinewAL - sinew;
		lastSinewAL = temp;
		if (lastSinewAL > 1.0) lastSinewAL = 1.0;
		if (lastSinewAL < -1.0) lastSinewAL = -1.0;
		inputSampleL = (inputSampleL * 0.5)+(lastSinewAL * 0.5);
		
		sinew = threshSinewB * cos(lastSinewBL*lastSinewBL);
		if (inputSampleL - lastSinewBL > sinew) temp = lastSinewBL + sinew;
		if (-(inputSampleL - lastSinewBL) > sinew) temp = lastSinewBL - sinew;
		lastSinewBL = temp;
		if (lastSinewBL > 1.0) lastSinewBL = 1.0;
		if (lastSinewBL < -1.0) lastSinewBL = -1.0;
		inputSampleL = (inputSampleL * 0.414)+(lastSinewBL * 0.586);
				
		temp = inputSampleR;
		sinew = threshSinewA * cos(lastSinewAR*lastSinewAR);
		if (inputSampleR - lastSinewAR > sinew) temp = lastSinewAR + sinew;
		if (-(inputSampleR - lastSinewAR) > sinew) temp = lastSinewAR - sinew;
		lastSinewAR = temp;
		if (lastSinewAR > 1.0) lastSinewAR = 1.0;
		if (lastSinewAR < -1.0) lastSinewAR = -1.0;
		inputSampleR = (inputSampleR * 0.5)+(lastSinewAR * 0.5);
				
		sinew = threshSinewB * cos(lastSinewBR*lastSinewBR);
		if (inputSampleR - lastSinewBR > sinew) temp = lastSinewBR + sinew;
		if (-(inputSampleR - lastSinewBR) > sinew) temp = lastSinewBR - sinew;
		lastSinewBR = temp;
		if (lastSinewBR > 1.0) lastSinewBR = 1.0;
		if (lastSinewBR < -1.0) lastSinewBR = -1.0;
		inputSampleR = (inputSampleR * 0.414)+(lastSinewBR * 0.586);
		//run Sinew to stop excess slews, two layers to make it more audible
		
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
	fpFlip = true;
	lastSinewAL = 0.0;
	lastSinewAR = 0.0;
	lastSinewBL = 0.0;
	lastSinewBR = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
