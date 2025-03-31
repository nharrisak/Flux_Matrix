#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleMDChannel"
#define AIRWINDOWS_DESCRIPTION "Goes for the tone shaping of the classic MCI console!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','5' )
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
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MidFreq", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MidPeak", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	double prevfreqMPeak;
	double prevamountMPeak;
	int mpc;
	double bassA;
	double bassB;
	double gainA;
	double gainB; //smoothed master fader for channel, from Z2 series filter code
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double pearA[18];
	double pearB[22];
	double mpkL[2005];
	double mpkR[2005];
	double f[66];
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
	if (cycleEnd > 3) cycleEnd = 3;
	
	double fatTreble = (GetParameter( kParam_One )*6.0)-3.0;
	bassA = bassB;
	bassB = (GetParameter( kParam_Four )*6.0)-3.0;
	//these should stack to go up to -3.0 to 3.0
	if (fatTreble < 0.0) fatTreble /= 3.0;
	if (bassB < 0.0) bassB /= 3.0;
	//and then become -1.0 to 3.0;
	//there will be successive sin/cos stages w. dry/wet in these
	double freqTreble = 0.853;
	double freqMid = 0.026912;
	switch (cycleEnd)
	{
		case 1: //base sample rate, no change
			break;
		case 2: //96k tier
			freqTreble = 0.4265;
			freqMid = 0.013456;
			break;
		case 3: //192k tier
			freqTreble = 0.21325;
			freqMid = 0.006728;
			break;
	}
	
	//begin ResEQ2 Mid Boost
	double freqMPeak = pow(GetParameter( kParam_Two )+0.16,3);
	double amountMPeak = pow(GetParameter( kParam_Three ),2);
	int maxMPeak = (amountMPeak*63.0)+1;
	if ((freqMPeak != prevfreqMPeak)||(amountMPeak != prevamountMPeak)) {
		for (int x = 0; x < maxMPeak; x++) {
			if (((double)x*freqMPeak) < M_PI_4) dram->f[x] = sin(((double)x*freqMPeak)*4.0)*freqMPeak*sin(((double)(maxMPeak-x)/(double)maxMPeak)*M_PI_2);
			else dram->f[x] = cos((double)x*freqMPeak)*freqMPeak*sin(((double)(maxMPeak-x)/(double)maxMPeak)*M_PI_2);
		}
		prevfreqMPeak = freqMPeak; prevamountMPeak = amountMPeak;
	}//end ResEQ2 Mid Boost
	
	int bitshiftL = 0;
	int bitshiftR = 0;
	double panControl = (GetParameter( kParam_Five )*2.0)-1.0; //-1.0 to 1.0
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
	gainB = GetParameter( kParam_Six )*2.0; //smoothed master fader from Z2 filters
	//BitShiftGain pre gain trim goes here
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		double gain = (gainA*temp)+(gainB*(1.0-temp));
		double bass = (bassA*temp)+(bassB*(1.0-temp));
		inputSampleL *= gain;
		inputSampleR *= gain;
		//for MCI consoles, the fader is before the EQ, which overdrives easily.
		//so we put the main fader here.
		
		//begin Pear filter stages
		double bassL = inputSampleL;
		double bassR = inputSampleR;
		double slew = ((bassL - dram->pearA[0]) + dram->pearA[1])*freqTreble*0.5;
		dram->pearA[0] = bassL = (freqTreble * bassL) + ((1.0-freqTreble) * (dram->pearA[0] + dram->pearA[1]));
		dram->pearA[1] = slew; slew = ((bassR - dram->pearA[2]) + dram->pearA[3])*freqTreble*0.5;
		dram->pearA[2] = bassR = (freqTreble * bassR) + ((1.0-freqTreble) * (dram->pearA[2] + dram->pearA[3]));
		dram->pearA[3] = slew; slew = ((bassL - dram->pearA[4]) + dram->pearA[5])*freqTreble*0.5;
		dram->pearA[4] = bassL = (freqTreble * bassL) + ((1.0-freqTreble) * (dram->pearA[4] + dram->pearA[5]));
		dram->pearA[5] = slew; slew = ((bassR - dram->pearA[6]) + dram->pearA[7])*freqTreble*0.5;
		dram->pearA[6] = bassR = (freqTreble * bassR) + ((1.0-freqTreble) * (dram->pearA[6] + dram->pearA[7]));
		dram->pearA[7] = slew; slew = ((bassL - dram->pearA[8]) + dram->pearA[9])*freqTreble*0.5;
		dram->pearA[8] = bassL = (freqTreble * bassL) + ((1.0-freqTreble) * (dram->pearA[8] + dram->pearA[9]));
		dram->pearA[9] = slew; slew = ((bassR - dram->pearA[10]) + dram->pearA[11])*freqTreble*0.5;
		dram->pearA[10] = bassR = (freqTreble * bassR) + ((1.0-freqTreble) * (dram->pearA[10] + dram->pearA[11]));
		dram->pearA[11] = slew; slew = ((bassL - dram->pearA[12]) + dram->pearA[13])*freqTreble*0.5;
		dram->pearA[12] = bassL = (freqTreble * bassL) + ((1.0-freqTreble) * (dram->pearA[12] + dram->pearA[13]));
		dram->pearA[13] = slew; slew = ((bassR - dram->pearA[14]) + dram->pearA[15])*freqTreble*0.5;
		dram->pearA[14] = bassR = (freqTreble * bassR) + ((1.0-freqTreble) * (dram->pearA[14] + dram->pearA[15]));
		dram->pearA[15] = slew;
		//unrolled mid/treble crossover (called bass to use fewer variables)		
		double trebleL = inputSampleL - bassL; inputSampleL = bassL;
		double trebleR = inputSampleR - bassR; inputSampleR = bassR;
		//at this point 'bass' is actually still mid and bass
		slew = ((bassL - dram->pearB[0]) + dram->pearB[1])*freqMid*0.5;
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
		//we now have three bands out of two pear filters
		
		double w = 0.0; //filter into bands, apply the sin/cos to each band
		if (fatTreble > 0.0) {
			w = fatTreble; if (w > 1.0) w = 1.0;
			trebleL = (trebleL*(1.0-w)) + (sin(trebleL*M_PI_2)*w);
			trebleR = (trebleR*(1.0-w)) + (sin(trebleR*M_PI_2)*w);
			if (fatTreble > 1.0) {
				w = fatTreble-1.0; if (w > 1.0) w = 1.0;
				trebleL = (trebleL*(1.0-w)) + (sin(trebleL*M_PI_2)*w);
				trebleR = (trebleR*(1.0-w)) + (sin(trebleR*M_PI_2)*w);
				if (fatTreble > 2.0) {
					w = fatTreble-2.0;
					trebleL = (trebleL*(1.0-w)) + (sin(trebleL*M_PI_2)*w);
					trebleR = (trebleR*(1.0-w)) + (sin(trebleR*M_PI_2)*w);
				} //sine stages for EQ or compression
			}
		}
		if (fatTreble < 0.0) {
			if (trebleL > 1.0) trebleL = 1.0; if (trebleL < -1.0) trebleL = -1.0;
			if (trebleR > 1.0) trebleR = 1.0; if (trebleR < -1.0) trebleR = -1.0;
			w = -fatTreble; if (w > 1.0) w = 1.0;
			if (trebleL > 0) trebleL = (trebleL*(1.0-w))+((1.0-cos(trebleL))*sin(w));
			else trebleL = (trebleL*(1.0-w))+((-1.0+cos(-trebleL))*sin(w));
			if (trebleR > 0) trebleR = (trebleR*(1.0-w))+((1.0-cos(trebleR))*sin(w));
			else trebleR = (trebleR*(1.0-w))+((-1.0+cos(-trebleR))*sin(w));
		} //cosine stages for EQ or expansion
		
		//begin ResEQ2 Mid Boost
		mpc++; if (mpc < 1 || mpc > 2001) mpc = 1;
		dram->mpkL[mpc] = midL;
		dram->mpkR[mpc] = midR;
		double midMPeakL = 0.0;
		double midMPeakR = 0.0;
		for (int x = 0; x < maxMPeak; x++) {
			int y = x*cycleEnd;
			switch (cycleEnd)
			{
				case 1: 
					midMPeakL += (dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x]);
					midMPeakR += (dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x]); break;
				case 2: 
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.5); break;
				case 3: 
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.333); break;
				case 4: 
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25); y--;
					midMPeakL += ((dram->mpkL[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25);
					midMPeakR += ((dram->mpkR[(mpc-y)+((mpc-y < 1)?2001:0)] * dram->f[x])*0.25); //break
			}
		}
		midL = (midMPeakL*amountMPeak)+((1.5-amountMPeak>1.0)?midL:midL*(1.5-amountMPeak));
		midR = (midMPeakR*amountMPeak)+((1.5-amountMPeak>1.0)?midR:midR*(1.5-amountMPeak));
		//end ResEQ2 Mid Boost
		
		if (bassL > 1.0) bassL = 1.0; if (bassL < -1.0) bassL = -1.0;
		if (bassR > 1.0) bassR = 1.0; if (bassR < -1.0) bassR = -1.0;
		if (bass > 0.0) {
			w = bass; if (w > 1.0) w = 1.0;
			bassL = (bassL*(1.0-w)) + (sin(bassL*M_PI_2)*w*1.6);
			bassR = (bassR*(1.0-w)) + (sin(bassR*M_PI_2)*w*1.6);
			if (bass > 1.0) {
				w = bass-1.0; if (w > 1.0) w = 1.0;
				bassL = (bassL*(1.0-w)) + (sin(bassL*M_PI_2)*w*1.4);
				bassR = (bassR*(1.0-w)) + (sin(bassR*M_PI_2)*w*1.4);
				if (bass > 2.0) {
					w = bass-2.0;
					bassL = (bassL*(1.0-w)) + (sin(bassL*M_PI_2)*w*1.2);
					bassR = (bassR*(1.0-w)) + (sin(bassR*M_PI_2)*w*1.2);
				} //sine stages for EQ or compression
			}
		}
		if (bass < 0.0) {
			w = -bass; if (w > 1.0) w = 1.0;	
			if (bassL > 0) bassL = (bassL*(1.0-w))+((1.0-cos(bassL))*sin(w));
			else bassL = (bassL*(1.0-w))+((-1.0+cos(-bassL))*sin(w));
			if (bassR > 0) bassR = (bassR*(1.0-w))+((1.0-cos(bassR))*sin(w));
			else bassR = (bassR*(1.0-w))+((-1.0+cos(-bassR))*sin(w));
		} //cosine stages for EQ or expansion
		//the sin() is further restricting output when fully attenuated
				
		inputSampleL = (bassL + midL + trebleL)*gainL;
		inputSampleR = (bassR + midR + trebleR)*gainR;
		//applies BitShiftPan pan section
		
		//begin sin() style Channel processing
		if (inputSampleL > 1.57079633) inputSampleL = 1.57079633;
		if (inputSampleL < -1.57079633) inputSampleL = -1.57079633;
		if (inputSampleR > 1.57079633) inputSampleR = 1.57079633;
		if (inputSampleR < -1.57079633) inputSampleR = -1.57079633;
		inputSampleL = sin(inputSampleL);
		inputSampleR = sin(inputSampleR);
		
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
	for (int x = 0; x < 17; x++) dram->pearA[x] = 0.0;
	for (int x = 0; x < 21; x++) dram->pearB[x] = 0.0;
	for(int count = 0; count < 2004; count++) {dram->mpkL[count] = 0.0; dram->mpkR[count] = 0.0;}
	for(int count = 0; count < 65; count++) {dram->f[count] = 0.0;}
	prevfreqMPeak = -1;
	prevamountMPeak = -1;
	mpc = 1;	
	bassA = bassB = 0.0;
	gainA = gainB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
