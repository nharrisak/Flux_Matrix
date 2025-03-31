#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleMCBuss"
#define AIRWINDOWS_DESCRIPTION "The initial, bright take on the MCI console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','2' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Master", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
	
	double lastSinewL;
	double lastSinewR;

	double subAL;
	double subAR;
	double subBL;
	double subBR;
	double subCL;
	double subCR;
	double subDL;
	double subDR;
	
	double gainA;
	double gainB; //smoothed master fader for channel, from Z2 series filter code
	
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
	
	gainA = gainB;
	gainB = sqrt(GetParameter( kParam_One )); //smoothed master fader from Z2 filters
	//this will be applied three times: this is to make the various tone alterations
	//hit differently at different master fader drive levels.
	//in particular, backing off the master fader tightens the super lows
	//but opens up the modified Sinew, because more of the attentuation happens before
	//you even get to slew clipping :) and if the fader is not active, it bypasses completely.

	double threshSinew = 0.5171104/overallscale;
	double subTrim = 0.001 / overallscale;

	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double gain = (gainA*temp)+(gainB*(1.0-temp));
		//setting up smoothed master fader
		
		//begin SubTight section
		double subSampleL = inputSampleL * subTrim;
		double subSampleR = inputSampleR * subTrim;
		
		double scale = 0.5+fabs(subSampleL*0.5);
		subSampleL = (subAL+(sin(subAL-subSampleL)*scale));
		subAL = subSampleL*scale;
		scale = 0.5+fabs(subSampleR*0.5);
		subSampleR = (subAR+(sin(subAR-subSampleR)*scale));
		subAR = subSampleR*scale;
		scale = 0.5+fabs(subSampleL*0.5);
		subSampleL = (subBL+(sin(subBL-subSampleL)*scale));
		subBL = subSampleL*scale;
		scale = 0.5+fabs(subSampleR*0.5);
		subSampleR = (subBR+(sin(subBR-subSampleR)*scale));
		subBR = subSampleR*scale;
		scale = 0.5+fabs(subSampleL*0.5);
		subSampleL = (subCL+(sin(subCL-subSampleL)*scale));
		subCL = subSampleL*scale;
		scale = 0.5+fabs(subSampleR*0.5);
		subSampleR = (subCR+(sin(subCR-subSampleR)*scale));
		subCR = subSampleR*scale;
		scale = 0.5+fabs(subSampleL*0.5);
		subSampleL = (subDL+(sin(subDL-subSampleL)*scale));
		subDL = subSampleL*scale;
		scale = 0.5+fabs(subSampleR*0.5);
		subSampleR = (subDR+(sin(subDR-subSampleR)*scale));
		subDR = subSampleR*scale;
		if (subSampleL > 0.25) subSampleL = 0.25;
		if (subSampleL < -0.25) subSampleL = -0.25;
		if (subSampleR > 0.25) subSampleR = 0.25;
		if (subSampleR < -0.25) subSampleR = -0.25;
		inputSampleL -= (subSampleL*16.0);
		inputSampleR -= (subSampleR*16.0);
		//end SubTight section
		
		if (gain < 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} //if using the master fader, we are going to attenuate three places.
		//subtight is always fully engaged: tighten response when restraining full console
		
		//begin Console7Buss which is the one we choose for ConsoleMC
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		inputSampleL = ((asin(inputSampleL*fabs(inputSampleL))/((fabs(inputSampleL) == 0.0) ?1:fabs(inputSampleL)))*0.618033988749894848204586)+(asin(inputSampleL)*0.381966011250105);

		if (inputSampleR > 1.0) inputSampleR = 1.0;
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		inputSampleR = ((asin(inputSampleR*fabs(inputSampleR))/((fabs(inputSampleR) == 0.0) ?1:fabs(inputSampleR)))*0.618033988749894848204586)+(asin(inputSampleR)*0.381966011250105);
		//this is an asin version of Spiral blended with regular asin ConsoleBuss.
		//It's blending between two different harmonics in the overtones of the algorithm
		
		if (gain < 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} //if using the master fader, we are going to attenuate three places.
		//after C7Buss but before EverySlew: allow highs to come out a bit more
		//when pulling back master fader. Less drive equals more open
				
		temp = inputSampleL;
		double clamp = inputSampleL - lastSinewL;
		if (lastSinewL > 1.0) lastSinewL = 1.0;
		if (lastSinewL < -1.0) lastSinewL = -1.0;
		double sinew = threshSinew * cos(lastSinewL);
		if (clamp > sinew) temp = lastSinewL + sinew;
		if (-clamp > sinew) temp = lastSinewL - sinew;
		inputSampleL = lastSinewL = temp;
		temp = inputSampleR;
		clamp = inputSampleR - lastSinewR;
		if (lastSinewR > 1.0) lastSinewR = 1.0;
		if (lastSinewR < -1.0) lastSinewR = -1.0;
		sinew = threshSinew * cos(lastSinewR);
		if (clamp > sinew) temp = lastSinewR + sinew;
		if (-clamp > sinew) temp = lastSinewR - sinew;
		inputSampleR = lastSinewR = temp;
		
		if (gain < 1.0) {
			inputSampleL *= gain;
			inputSampleR *= gain;
		} //if using the master fader, we are going to attenuate three places.
		//after EverySlew fades the total output sound: least change in tone here.
		
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
	lastSinewL = lastSinewR = 0.0;
	subAL = subAR = subBL = subBR = subCL = subCR = subDL = subDR = 0.0;
	gainA = gainB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
