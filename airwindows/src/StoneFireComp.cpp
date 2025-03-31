#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "StoneFireComp"
#define AIRWINDOWS_DESCRIPTION "Some of the dynamics for ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','t','o' )
enum {

	kParam_B =0,
	kParam_C =1,
	kParam_D =2,
	kParam_E =3,
	kParam_F =4,
	kParam_G =5,
	kParam_H =6,
	kParam_I =7,
	kParam_J =8,
	kParam_K =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Fire Th", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fire", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "StoneTh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	
	enum {
		prevSampL1, prevSlewL1, accSlewL1,
		prevSampL2, prevSlewL2, accSlewL2,
		prevSampL3, prevSlewL3, accSlewL3,
		kalGainL, kalOutL,
		prevSampR1, prevSlewR1, accSlewR1,
		prevSampR2, prevSlewR2, accSlewR2,
		prevSampR3, prevSlewR3, accSlewR3,
		kalGainR, kalOutR,
		kal_total
	};
	double fireCompL;
	double fireCompR;
	double stoneCompL;
	double stoneCompR;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		double kal[kal_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double compFThresh = pow(GetParameter( kParam_B ),4);
	double compFAttack = 1.0/(((pow(GetParameter( kParam_C ),3)*5000.0)+500.0)*overallscale);
	double compFRelease = 1.0/(((pow(GetParameter( kParam_D ),5)*50000.0)+500.0)*overallscale);
	double fireGain = GetParameter( kParam_E )*2.0;
	if (fireGain > 1.0) fireGain *= fireGain; else fireGain = 1.0-pow(1.0-fireGain,2);
	double firePad = fireGain; if (firePad > 1.0) firePad = 1.0;
	
	double compSThresh = pow(GetParameter( kParam_F ),4);
	double compSAttack = 1.0/(((pow(GetParameter( kParam_G ),3)*5000.0)+500.0)*overallscale);
	double compSRelease = 1.0/(((pow(GetParameter( kParam_H ),5)*50000.0)+500.0)*overallscale);
	double stoneGain = GetParameter( kParam_I )*2.0;
	if (stoneGain > 1.0) stoneGain *= stoneGain; else stoneGain = 1.0-pow(1.0-stoneGain,2);
	double stonePad = stoneGain; if (stonePad > 1.0) stonePad = 1.0;
	
	double kalman = 1.0-(pow(GetParameter( kParam_J ),2)/overallscale);
	//crossover frequency between mid/bass
	double compRatio = 1.0-pow(1.0-GetParameter( kParam_K ),2);
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		//begin KalmanL
		double fireL = inputSampleL;
		double temp = inputSampleL = inputSampleL*(1.0-kalman)*0.777;
		inputSampleL *= (1.0-kalman);
		//set up gain levels to control the beast
		dram->kal[prevSlewL3] += dram->kal[prevSampL3] - dram->kal[prevSampL2]; dram->kal[prevSlewL3] *= 0.5;
		dram->kal[prevSlewL2] += dram->kal[prevSampL2] - dram->kal[prevSampL1]; dram->kal[prevSlewL2] *= 0.5;
		dram->kal[prevSlewL1] += dram->kal[prevSampL1] - inputSampleL; dram->kal[prevSlewL1] *= 0.5;
		//make slews from each set of samples used
		dram->kal[accSlewL2] += dram->kal[prevSlewL3] - dram->kal[prevSlewL2]; dram->kal[accSlewL2] *= 0.5;
		dram->kal[accSlewL1] += dram->kal[prevSlewL2] - dram->kal[prevSlewL1]; dram->kal[accSlewL1] *= 0.5;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewL3] += (dram->kal[accSlewL2] - dram->kal[accSlewL1]); dram->kal[accSlewL3] *= 0.5;
		//entering the abyss, what even is this
		dram->kal[kalOutL] += dram->kal[prevSampL1] + dram->kal[prevSlewL2] + dram->kal[accSlewL3]; dram->kal[kalOutL] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainL] += fabs(temp-dram->kal[kalOutL])*kalman*8.0; dram->kal[kalGainL] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainL] > kalman*0.5) dram->kal[kalGainL] = kalman*0.5;
		//attempts to avoid explosions
		dram->kal[kalOutL] += (temp*(1.0-(0.68+(kalman*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampL3] = dram->kal[prevSampL2]; dram->kal[prevSampL2] = dram->kal[prevSampL1];
		dram->kal[prevSampL1] = (dram->kal[kalGainL] * dram->kal[kalOutL]) + ((1.0-dram->kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampL1] > 1.0) dram->kal[prevSampL1] = 1.0; if (dram->kal[prevSampL1] < -1.0) dram->kal[prevSampL1] = -1.0;
		double stoneL = dram->kal[kalOutL]*0.777;
		fireL -= stoneL;
		//end KalmanL
		
		//begin KalmanR
		double fireR = inputSampleR;
		temp = inputSampleR = inputSampleR*(1.0-kalman)*0.777;
		inputSampleR *= (1.0-kalman);
		//set up gain levels to control the beast
		dram->kal[prevSlewR3] += dram->kal[prevSampR3] - dram->kal[prevSampR2]; dram->kal[prevSlewR3] *= 0.5;
		dram->kal[prevSlewR2] += dram->kal[prevSampR2] - dram->kal[prevSampR1]; dram->kal[prevSlewR2] *= 0.5;
		dram->kal[prevSlewR1] += dram->kal[prevSampR1] - inputSampleR; dram->kal[prevSlewR1] *= 0.5;
		//make slews from each set of samples used
		dram->kal[accSlewR2] += dram->kal[prevSlewR3] - dram->kal[prevSlewR2]; dram->kal[accSlewR2] *= 0.5;
		dram->kal[accSlewR1] += dram->kal[prevSlewR2] - dram->kal[prevSlewR1]; dram->kal[accSlewR1] *= 0.5;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewR3] += (dram->kal[accSlewR2] - dram->kal[accSlewR1]); dram->kal[accSlewR3] *= 0.5;
		//entering the abyss, what even is this
		dram->kal[kalOutR] += dram->kal[prevSampR1] + dram->kal[prevSlewR2] + dram->kal[accSlewR3]; dram->kal[kalOutR] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainR] += fabs(temp-dram->kal[kalOutR])*kalman*8.0; dram->kal[kalGainR] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainR] > kalman*0.5) dram->kal[kalGainR] = kalman*0.5;
		//attempts to avoid explosions
		dram->kal[kalOutR] += (temp*(1.0-(0.68+(kalman*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampR3] = dram->kal[prevSampR2]; dram->kal[prevSampR2] = dram->kal[prevSampR1];
		dram->kal[prevSampR1] = (dram->kal[kalGainR] * dram->kal[kalOutR]) + ((1.0-dram->kal[kalGainR])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampR1] > 1.0) dram->kal[prevSampR1] = 1.0; if (dram->kal[prevSampR1] < -1.0) dram->kal[prevSampR1] = -1.0;
		double stoneR = dram->kal[kalOutR]*0.777;
		fireR -= stoneR;
		//end KalmanR
		
		//fire dynamics
		if (fabs(fireL) > compFThresh) { //compression L
			fireCompL -= (fireCompL * compFAttack);
			fireCompL += ((compFThresh / fabs(fireL))*compFAttack);
		} else fireCompL = (fireCompL*(1.0-compFRelease))+compFRelease;
		if (fabs(fireR) > compFThresh) { //compression R
			fireCompR -= (fireCompR * compFAttack);
			fireCompR += ((compFThresh / fabs(fireR))*compFAttack);
		} else fireCompR = (fireCompR*(1.0-compFRelease))+compFRelease;
		if (fireCompL > fireCompR) fireCompL -= (fireCompL * compFAttack);
		if (fireCompR > fireCompL) fireCompR -= (fireCompR * compFAttack);
		fireCompL = fmax(fmin(fireCompL,1.0),0.0);
		fireCompR = fmax(fmin(fireCompR,1.0),0.0);
		fireL *= (((1.0-compRatio)*firePad)+(fireCompL*compRatio*fireGain));
		fireR *= (((1.0-compRatio)*firePad)+(fireCompR*compRatio*fireGain));
		
		//stone dynamics
		if (fabs(stoneL) > compSThresh) { //compression L
			stoneCompL -= (stoneCompL * compSAttack);
			stoneCompL += ((compSThresh / fabs(stoneL))*compSAttack);
		} else stoneCompL = (stoneCompL*(1.0-compSRelease))+compSRelease;
		if (fabs(stoneR) > compSThresh) { //compression R
			stoneCompR -= (stoneCompR * compSAttack);
			stoneCompR += ((compSThresh / fabs(stoneR))*compSAttack);
		} else stoneCompR = (stoneCompR*(1.0-compSRelease))+compSRelease;
		if (stoneCompL > stoneCompR) stoneCompL -= (stoneCompL * compSAttack);
		if (stoneCompR > stoneCompL) stoneCompR -= (stoneCompR * compSAttack);
		stoneCompL = fmax(fmin(stoneCompL,1.0),0.0);
		stoneCompR = fmax(fmin(stoneCompR,1.0),0.0);
		stoneL *= (((1.0-compRatio)*stonePad)+(stoneCompL*compRatio*stoneGain));
		stoneR *= (((1.0-compRatio)*stonePad)+(stoneCompR*compRatio*stoneGain));
		
		inputSampleL = stoneL+fireL;
		inputSampleR = stoneR+fireR;
		
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
	for (int x = 0; x < kal_total; x++) dram->kal[x] = 0.0;
	fireCompL = 1.0;
	fireCompR = 1.0;
	stoneCompL = 1.0;
	stoneCompR = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
