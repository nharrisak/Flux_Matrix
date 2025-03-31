#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "TexturizeMS"
#define AIRWINDOWS_DESCRIPTION "A hidden-noise plugin for adding mid-side sonic texture to things."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','e','y' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
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
{ .name = "BrightM", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "PunchyM", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DryWetM", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BrightS", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "PunchyS", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DryWetS", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	bool polarityM;
	double lastSampleM;
	double iirSampleM;
	double noiseAM;
	double noiseBM;
	double noiseCM;
	bool flipM;
	
	bool polarityS;
	double lastSampleS;
	double iirSampleS;
	double noiseAS;
	double noiseBS;
	double noiseCS;
	bool flipS;
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
	
	double slewAmountM = ((pow(GetParameter( kParam_One ),2.0)*4.0)+0.71)/overallscale;
	double dynAmountM = pow(GetParameter( kParam_Two ),2.0);
	double wetM = pow(GetParameter( kParam_Three ),5);
	
	double slewAmountS = ((pow(GetParameter( kParam_Four ),2.0)*4.0)+0.71)/overallscale;
	double dynAmountS = pow(GetParameter( kParam_Five ),2.0);
	double wetS = pow(GetParameter( kParam_Six ),5);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double mid = inputSampleL + inputSampleR;
		double side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		double drySampleM = mid;
		double drySampleS = side;
		
		//begin mid
		if (mid < 0) {
			if (polarityM == true) {
				if (noiseAM < 0) flipM = true;
				else flipM = false;
			}
			polarityM = false;
		} else polarityM = true;		
		
		if (flipM) noiseAM += (double(fpdL)/UINT32_MAX);
		else noiseAM -= (double(fpdL)/UINT32_MAX);
		//here's the guts of the random walk		
		flipM = !flipM;
		
		if (mid > 1.0) mid = 1.0; if (mid < -1.0) mid = -1.0;
		if (dynAmountM < 0.4999) mid = (mid*dynAmountM*2.0) + (sin(mid)*(1.0-(dynAmountM*2.0)));		
		if (dynAmountM > 0.5001) mid = (asin(mid)*((dynAmountM*2.0)-1.0)) + (mid*(1.0-((dynAmountM*2.0)-1.0)));
		//doing this in two steps means I get to not run an extra sin/asin function per sample
		
		noiseBM = sin(noiseAM*(0.2-(dynAmountM*0.125))*fabs(mid));
		
		double slew = fabs(mid-lastSampleM)*slewAmountM;
		lastSampleM = mid*(0.86-(dynAmountM*0.125));
		
		if (slew > 1.0) slew = 1.0;
		double iirIntensity = slew;
		iirIntensity *= 2.472;
		iirIntensity *= iirIntensity;
		if (iirIntensity > 1.0) iirIntensity = 1.0;
		
		iirSampleM = (iirSampleM * (1.0 - iirIntensity)) + (noiseBM * iirIntensity);
		noiseBM = iirSampleM;
		noiseBM = (noiseBM * slew) + (noiseCM * (1.0-slew));
		noiseCM = noiseBM;
		
		mid = (noiseCM * wetM) + (drySampleM * (1.0-wetM));
		//end mid

		//begin side
		if (side < 0) {
			if (polarityS == true) {
				if (noiseAS < 0) flipS = true;
				else flipS = false;
			}
			polarityS = false;
		} else polarityS = true;		
		
		if (flipS) noiseAS += (double(fpdR)/UINT32_MAX);
		else noiseAS -= (double(fpdR)/UINT32_MAX);
		//here's the guts of the random walk		
		flipS = !flipS;
		
		if (side > 1.0) side = 1.0; if (side < -1.0) side = -1.0;
		if (dynAmountS < 0.4999) side = (side*dynAmountS*2.0) + (sin(side)*(1.0-(dynAmountS*2.0)));		
		if (dynAmountS > 0.5001) side = (asin(side)*((dynAmountS*2.0)-1.0)) + (side*(1.0-((dynAmountS*2.0)-1.0)));
		//doing this in two steps means I get to not run an extra sin/asin function per sample
		
		noiseBS = sin(noiseAS*(0.2-(dynAmountS*0.125))*fabs(side));
		
		slew = fabs(side-lastSampleS)*slewAmountS;
		lastSampleS = side*(0.86-(dynAmountS*0.125));
		
		if (slew > 1.0) slew = 1.0;
		iirIntensity = slew;
		iirIntensity *= 2.472;
		iirIntensity *= iirIntensity;
		if (iirIntensity > 1.0) iirIntensity = 1.0;
		
		iirSampleS = (iirSampleS * (1.0 - iirIntensity)) + (noiseBS * iirIntensity);
		noiseBS = iirSampleS;
		noiseBS = (noiseBS * slew) + (noiseCS * (1.0-slew));
		noiseCS = noiseBS;
		
		side = (noiseCS * wetS) + (drySampleS * (1.0-wetS));
		//end side
		
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
	polarityM = false;
	lastSampleM = 0.0;
	iirSampleM = 0.0;
	noiseAM = 0.0;
	noiseBM = 0.0;
	noiseCM = 0.0;
	flipM = true;
	polarityS = false;
	lastSampleS = 0.0;
	iirSampleS = 0.0;
	noiseAS = 0.0;
	noiseBS = 0.0;
	noiseCS = 0.0;
	flipS = true;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
