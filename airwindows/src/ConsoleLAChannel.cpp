#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleLAChannel"
#define AIRWINDOWS_DESCRIPTION "The Airwindows take on the Quad Eight console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','1' )
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
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double subAL;
	double subAR;
	double subBL;
	double subBR;
	double subCL;
	double subCR;
	int hullp;
	double midA;
	double midB;
	double bassA;
	double bassB;
	double gainA;
	double gainB; //smoothed master fader for channel, from Z2 series filter code
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double hullL[225];	
	double hullR[225];	
	double pearB[22];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate(); //will be over 1.0848 when over 48k
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	
	int limit = 4*cycleEnd;
	double divisor = 2.0/limit;

	double treble = (GetParameter( kParam_One )*6.0)-3.0;
	midA = midB;
	midB = (GetParameter( kParam_Two )*6.0)-3.0;
	bassA = bassB;
	bassB = (GetParameter( kParam_Three )*6.0)-3.0;
	//these should stack to go up to -3.0 to 3.0
	if (treble < 0.0) treble /= 3.0;
	if (midB < 0.0) midB /= 3.0;
	if (bassB < 0.0) bassB /= 3.0;
	//and then become -1.0 to 3.0;
	//there will be successive sin/cos stages w. dry/wet in these
	double freqMid = 0.024;
	switch (cycleEnd)
	{
		case 1: //base sample rate, no change
			break;
		case 2: //96k tier
			freqMid = 0.012;
			break;
		case 3: //192k tier
			freqMid = 0.006;
			break;
	}
	
	
	int bitshiftL = 0;
	int bitshiftR = 0;
	double panControl = (GetParameter( kParam_Four )*2.0)-1.0; //-1.0 to 1.0
	double panAttenuation = (1.0-fabs(panControl));
	int panBits = 20; //start centered
	if (panAttenuation > 0.0) panBits = floor(1.0 / panAttenuation);
	if (panControl > 0.25) bitshiftL += panBits;
	if (panControl < -0.25) bitshiftR += panBits;
	if (bitshiftL < 0) bitshiftL = 0; if (bitshiftL > 17) bitshiftL = 17;
	if (bitshiftR < 0) bitshiftR = 0; if (bitshiftR > 17) bitshiftR = 17;
	double gainL = 1.0;
	double gainR = 1.0;
	switch (bitshiftL)
	{
		case 17: gainL = 0.0; break;
		case 16: gainL = 0.0000152587890625; break;
		case 15: gainL = 0.000030517578125; break;
		case 14: gainL = 0.00006103515625; break;
		case 13: gainL = 0.0001220703125; break;
		case 12: gainL = 0.000244140625; break;
		case 11: gainL = 0.00048828125; break;
		case 10: gainL = 0.0009765625; break;
		case 9: gainL = 0.001953125; break;
		case 8: gainL = 0.00390625; break;
		case 7: gainL = 0.0078125; break;
		case 6: gainL = 0.015625; break;
		case 5: gainL = 0.03125; break;
		case 4: gainL = 0.0625; break;
		case 3: gainL = 0.125; break;
		case 2: gainL = 0.25; break;
		case 1: gainL = 0.5; break;
		case 0: break;
	}
	switch (bitshiftR)
	{
		case 17: gainR = 0.0; break;
		case 16: gainR = 0.0000152587890625; break;
		case 15: gainR = 0.000030517578125; break;
		case 14: gainR = 0.00006103515625; break;
		case 13: gainR = 0.0001220703125; break;
		case 12: gainR = 0.000244140625; break;
		case 11: gainR = 0.00048828125; break;
		case 10: gainR = 0.0009765625; break;
		case 9: gainR = 0.001953125; break;
		case 8: gainR = 0.00390625; break;
		case 7: gainR = 0.0078125; break;
		case 6: gainR = 0.015625; break;
		case 5: gainR = 0.03125; break;
		case 4: gainR = 0.0625; break;
		case 3: gainR = 0.125; break;
		case 2: gainR = 0.25; break;
		case 1: gainR = 0.5; break;
		case 0: break;
	}
	
	gainA = gainB;
	gainB = GetParameter( kParam_Five )*2.0; //smoothed master fader from Z2 filters
	//BitShiftGain pre gain trim goes here
	
	double subTrim = 0.0011 / overallscale;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double gain = (gainA*temp)+(gainB*(1.0-temp));
		double mid = (midA*temp)+(midB*(1.0-temp));
		double bass = (bassA*temp)+(bassB*(1.0-temp));
		
		//begin Hull2 Treble
		hullp--; if (hullp < 0) hullp += 60;
		dram->hullL[hullp] = dram->hullL[hullp+60] = inputSampleL;
		dram->hullR[hullp] = dram->hullR[hullp+60] = inputSampleR;
		
		int x = hullp;
		double bassL = 0.0;
		double bassR = 0.0;
		while (x < hullp+(limit/2)) {
			bassL += dram->hullL[x] * divisor;
			bassR += dram->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125;
		bassR += bassR * 0.125;
		while (x < hullp+limit) {
			bassL -= dram->hullL[x] * 0.125 * divisor;
			bassR -= dram->hullR[x] * 0.125 * divisor;
			x++;
		}
		dram->hullL[hullp+20] = dram->hullL[hullp+80] = bassL;
		dram->hullR[hullp+20] = dram->hullR[hullp+80] = bassR;
		x = hullp+20;
		bassL = bassR = 0.0;
		while (x < hullp+20+(limit/2)) {
			bassL += dram->hullL[x] * divisor;
			bassR += dram->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125;
		bassR += bassR * 0.125;
		while (x < hullp+20+limit) {
			bassL -= dram->hullL[x] * 0.125 * divisor;
			bassR -= dram->hullR[x] * 0.125 * divisor;
			x++;
		}
		dram->hullL[hullp+40] = dram->hullL[hullp+100] = bassL;
		dram->hullR[hullp+40] = dram->hullR[hullp+100] = bassR;
		x = hullp+40;
		bassL = bassR = 0.0;
		while (x < hullp+40+(limit/2)) {
			bassL += dram->hullL[x] * divisor;
			bassR += dram->hullR[x] * divisor;
			x++;
		}
		bassL += bassL * 0.125;
		bassR += bassR * 0.125;
		while (x < hullp+40+limit) {
			bassL -= dram->hullL[x] * 0.125 * divisor;
			bassR -= dram->hullR[x] * 0.125 * divisor;
			x++;
		}
		double trebleL = inputSampleL - bassL; inputSampleL = bassL;
		double trebleR = inputSampleR - bassR; inputSampleR = bassR;
		//end Hull2 treble
		
		//begin Pear filter stages
		//at this point 'bass' is actually still mid and bass
		double slew = ((bassL - dram->pearB[0]) + dram->pearB[1])*freqMid*0.5;
		dram->pearB[0] = bassL = (freqMid * bassL) + ((1.0-freqMid) * (dram->pearB[0] + dram->pearB[1]));
		dram->pearB[1] = slew; slew = ((bassR - dram->pearB[2]) + dram->pearB[3])*freqMid*0.5;
		dram->pearB[2] = bassR = (freqMid * bassR) + ((1.0-freqMid) * (dram->pearB[2] + dram->pearB[3]));
		dram->pearB[3] = slew; slew = ((bassL - dram->pearB[4]) + dram->pearB[5])*freqMid*0.5;
		dram->pearB[4] = bassL = (freqMid * bassL) + ((1.0-freqMid) * (dram->pearB[4] + dram->pearB[5]));
		dram->pearB[5] = slew; slew = ((bassR - dram->pearB[6]) + dram->pearB[7])*freqMid*0.5;
		dram->pearB[6] = bassR = (freqMid * bassR) + ((1.0-freqMid) * (dram->pearB[6] + dram->pearB[7]));
		dram->pearB[7] = slew; slew = ((bassL - dram->pearB[8]) + dram->pearB[9])*freqMid*0.5;
		dram->pearB[8] = bassL = (freqMid * bassL) + ((1.0-freqMid) * (dram->pearB[8] + dram->pearB[9]));
		dram->pearB[9] = slew; slew = ((bassR - dram->pearB[10]) + dram->pearB[11])*freqMid*0.5;
		dram->pearB[10] = bassR = (freqMid * bassR) + ((1.0-freqMid) * (dram->pearB[10] + dram->pearB[11]));
		dram->pearB[11] = slew; slew = ((bassL - dram->pearB[12]) + dram->pearB[13])*freqMid*0.5;
		dram->pearB[12] = bassL = (freqMid * bassL) + ((1.0-freqMid) * (dram->pearB[12] + dram->pearB[13]));
		dram->pearB[13] = slew; slew = ((bassR - dram->pearB[14]) + dram->pearB[15])*freqMid*0.5;
		dram->pearB[14] = bassR = (freqMid * bassR) + ((1.0-freqMid) * (dram->pearB[14] + dram->pearB[15]));
		dram->pearB[15] = slew; slew = ((bassL - dram->pearB[16]) + dram->pearB[17])*freqMid*0.5;
		dram->pearB[16] = bassL = (freqMid * bassL) + ((1.0-freqMid) * (dram->pearB[16] + dram->pearB[17]));
		dram->pearB[17] = slew; slew = ((bassR - dram->pearB[18]) + dram->pearB[19])*freqMid*0.5;
		dram->pearB[18] = bassR = (freqMid * bassR) + ((1.0-freqMid) * (dram->pearB[18] + dram->pearB[19]));
		dram->pearB[19] = slew;
		double midL = inputSampleL - bassL;
		double midR = inputSampleR - bassR;
		//we now have three bands out of hull and pear filters
		
		double w = 0.0; //filter into bands, apply the sin/cos to each band
		if (treble > 0.0) {
			w = treble; if (w > 1.0) w = 1.0;
			trebleL = (trebleL*(1.0-w)) + (sin(trebleL*M_PI_2)*treble);
			trebleR = (trebleR*(1.0-w)) + (sin(trebleR*M_PI_2)*treble);
		}
		if (treble < 0.0) {
			if (trebleL > 1.0) trebleL = 1.0; if (trebleL < -1.0) trebleL = -1.0;
			if (trebleR > 1.0) trebleR = 1.0; if (trebleR < -1.0) trebleR = -1.0;
			w = -treble; if (w > 1.0) w = 1.0;
			if (trebleL > 0) trebleL = (trebleL*(1.0-w))+((1.0-cos(trebleL*w))*(1.0-w));
			else trebleL = (trebleL*(1.0-w))+((-1.0+cos(-trebleL*w))*(1.0-w));
			if (trebleR > 0) trebleR = (trebleR*(1.0-w))+((1.0-cos(trebleR*w))*(1.0-w));
			else trebleR = (trebleR*(1.0-w))+((-1.0+cos(-trebleR*w))*(1.0-w));
		} //cosine stages for EQ or expansion
		
		if (midL > 1.0) midL = 1.0; if (midL < -1.0) midL = -1.0;
		if (midR > 1.0) midR = 1.0; if (midR < -1.0) midR = -1.0;
		if (mid > 0.0) {
			w = mid; if (w > 1.0) w = 1.0;
			midL = (midL*(1.0-w)) + (sin(midL*M_PI_2)*mid);
			midR = (midR*(1.0-w)) + (sin(midR*M_PI_2)*mid);
		}
		if (mid < 0.0) {
			w = -mid; if (w > 1.0) w = 1.0;	
			if (midL > 0) midL = (midL*(1.0-w))+((1.0-cos(midL*w))*(1.0-w));
			else midL = (midL*(1.0-w))+((-1.0+cos(-midL*w))*(1.0-w));
			if (midR > 0) midR = (midR*(1.0-w))+((1.0-cos(midR*w))*(1.0-w));
			else midR = (midR*(1.0-w))+((-1.0+cos(-midR*w))*(1.0-w));
		} //cosine stages for EQ or expansion
		
		if (bassL > 1.0) bassL = 1.0; if (bassL < -1.0) bassL = -1.0;
		if (bassR > 1.0) bassR = 1.0; if (bassR < -1.0) bassR = -1.0;
		if (bass > 0.0) {
			w = bass; if (w > 1.0) w = 1.0;
			bassL = (bassL*(1.0-w)) + (sin(bassL*M_PI_2)*bass);
			bassR = (bassR*(1.0-w)) + (sin(bassR*M_PI_2)*bass);
		}
		if (bass < 0.0) {
			w = -bass; if (w > 1.0) w = 1.0;
			if (bassL > 0) bassL = (bassL*(1.0-w))+((1.0-cos(bassL*w))*(1.0-w));
			else bassL = (bassL*(1.0-w))+((-1.0+cos(-bassL*w))*(1.0-w));
			if (bassR > 0) bassR = (bassR*(1.0-w))+((1.0-cos(bassR*w))*(1.0-w));
			else bassR = (bassR*(1.0-w))+((-1.0+cos(-bassR*w))*(1.0-w));
		} //cosine stages for EQ or expansion
		
		inputSampleL = (bassL + midL + trebleL)*gainL*gain;
		inputSampleR = (bassR + midR + trebleR)*gainR*gain;
		//applies BitShiftPan pan section, and smoothed fader gain
		
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
		if (subSampleL > 0.25) subSampleL = 0.25;
		if (subSampleL < -0.25) subSampleL = -0.25;
		if (subSampleR > 0.25) subSampleR = 0.25;
		if (subSampleR < -0.25) subSampleR = -0.25;
		inputSampleL += (subSampleL*16.0);
		inputSampleR += (subSampleR*16.0);
		//end SubTight section
		
		//begin Console7 Channel processing
		if (inputSampleL > 1.097) inputSampleL = 1.097;
		if (inputSampleL < -1.097) inputSampleL = -1.097;
		if (inputSampleR > 1.097) inputSampleR = 1.097;
		if (inputSampleR < -1.097) inputSampleR = -1.097;
		inputSampleL = ((sin(inputSampleL*fabs(inputSampleL))/((fabs(inputSampleL) == 0.0) ?1:fabs(inputSampleL)))*0.8)+(sin(inputSampleL)*0.2);
		inputSampleR = ((sin(inputSampleR*fabs(inputSampleR))/((fabs(inputSampleR) == 0.0) ?1:fabs(inputSampleR)))*0.8)+(sin(inputSampleR)*0.2);
		//this is a version of Spiral blended 80/20 with regular Density.
		//It's blending between two different harmonics in the overtones of the algorithm
		
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
	for(int count = 0; count < 222; count++) {dram->hullL[count] = 0.0; dram->hullR[count] = 0.0;}
	hullp = 1;
	for (int x = 0; x < 21; x++) dram->pearB[x] = 0.0;
	subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
	midA = midB = 0.0;
	bassA = bassB = 0.0;
	gainA = gainB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
