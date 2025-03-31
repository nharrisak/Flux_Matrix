#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleXBuss"
#define AIRWINDOWS_DESCRIPTION "The fully featured, biggest Airwindows console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','6' )
enum {

	kParam_HIP = 0,
	kParam_LOP = 1,
	kParam_AIR = 2,
	kParam_FIR = 3,
	kParam_STO = 4,
	kParam_RNG = 5,
	kParam_FCT = 6,
	kParam_SCT = 7,
	kParam_FCR = 8,
	kParam_SCR = 9,
	kParam_FCA = 10,
	kParam_SCA = 11,
	kParam_FCL = 12,
	kParam_SCL = 13,
	kParam_FGT = 14,
	kParam_SGT = 15,
	kParam_FGR = 16,
	kParam_SGR = 17,
	kParam_FGS = 18,
	kParam_SGS = 19,
	kParam_FGL = 20,
	kParam_SGL = 21,	
	kParam_TRF = 22,
	kParam_TRG = 23,
	kParam_TRR = 24,
	kParam_HMF = 25,
	kParam_HMG = 26,
	kParam_HMR = 27,
	kParam_LMF = 28,
	kParam_LMG = 29,
	kParam_LMR = 30,
	kParam_BSF = 31,
	kParam_BSG = 32,
	kParam_BSR = 33,
	kParam_DSC = 34,
	kParam_PAN = 35,
	kParam_FAD = 36,
	//Add your parameters here...
	kNumberOfParameters=37
};
const int dscBuf = 90;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23, kParam24, kParam25, kParam26, kParam27, kParam28, kParam29, kParam30, kParam31, kParam32, kParam33, kParam34, kParam35, kParam36, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Highpas", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Air", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fire", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Thrs", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Thrs", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Atk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Atk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Thrs", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Thrs", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Sust", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Sust", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tr Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tr Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HM Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HighMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HM Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LM Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LowMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LM Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bs Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bs Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Top dB", .min = 7000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23, kParam24, kParam25, kParam26, kParam27, kParam28, kParam29, kParam30, kParam31, kParam32, kParam33, kParam34, kParam35, kParam36, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	
	enum {
		hilp_freq, hilp_temp,
		hilp_a0, hilp_aA0, hilp_aB0, hilp_a1, hilp_aA1, hilp_aB1, hilp_b1, hilp_bA1, hilp_bB1, hilp_b2, hilp_bA2, hilp_bB2,
		hilp_c0, hilp_cA0, hilp_cB0, hilp_c1, hilp_cA1, hilp_cB1, hilp_d1, hilp_dA1, hilp_dB1, hilp_d2, hilp_dA2, hilp_dB2,
		hilp_e0, hilp_eA0, hilp_eB0, hilp_e1, hilp_eA1, hilp_eB1, hilp_f1, hilp_fA1, hilp_fB1, hilp_f2, hilp_fA2, hilp_fB2,
		hilp_aL1, hilp_aL2, hilp_aR1, hilp_aR2,
		hilp_cL1, hilp_cL2, hilp_cR1, hilp_cR2,
		hilp_eL1, hilp_eL2, hilp_eR1, hilp_eR2,
		hilp_total
	};
	double highpass[hilp_total];
	double lowpass[hilp_total];
	
	enum {
		pvAL1, pvSL1, accSL1, acc2SL1,
		pvAL2, pvSL2, accSL2, acc2SL2,
		pvAL3, pvSL3, accSL3,
		pvAL4, pvSL4,
		gndavgL, outAL, gainAL,
		pvAR1, pvSR1, accSR1, acc2SR1,
		pvAR2, pvSR2, accSR2, acc2SR2,
		pvAR3, pvSR3, accSR3,
		pvAR4, pvSR4,
		gndavgR, outAR, gainAR,
		air_total
	};
	double air[air_total];
	
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
	double kal[kal_total];
	double fireCompL;
	double fireCompR;
	double fireGate;
	double stoneCompL;
	double stoneCompR;
	double stoneGate;	
	double airGainA;
	double airGainB;
	double fireGainA;
	double fireGainB;
	double stoneGainA;
	double stoneGainB;
	
	enum { 
		biqs_freq, biqs_reso, biqs_level,
		biqs_nonlin, biqs_temp, biqs_dis,
		biqs_a0, biqs_a1, biqs_b1, biqs_b2,
		biqs_c0, biqs_c1, biqs_d1, biqs_d2,
		biqs_e0, biqs_e1, biqs_f1, biqs_f2,
		biqs_aL1, biqs_aL2, biqs_aR1, biqs_aR2,
		biqs_cL1, biqs_cL2, biqs_cR1, biqs_cR2,
		biqs_eL1, biqs_eL2, biqs_eR1, biqs_eR2,
		biqs_outL, biqs_outR, biqs_total
	};
	double high[biqs_total];
	double hmid[biqs_total];
	double lmid[biqs_total];
	double bass[biqs_total];
	
	double dBaL[dscBuf+5];
	double dBaR[dscBuf+5];
	double dBaPosL;
	double dBaPosR;
	int dBaXL;
	int dBaXR;
	
	double panA;
	double panB;
	double inTrimA;
	double inTrimB;
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
struct _dram {
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	highpass[hilp_freq] = ((pow(GetParameter( kParam_HIP ),3)*24000.0)+10.0)/GetSampleRate();
	if (highpass[hilp_freq] > 0.495) highpass[hilp_freq] = 0.495;
	bool highpassEngage = true; if (GetParameter( kParam_HIP ) == 0.0) highpassEngage = false;
	
	lowpass[hilp_freq] = ((pow(1.0-GetParameter( kParam_LOP ),3)*24000.0)+10.0)/GetSampleRate();
	if (lowpass[hilp_freq] > 0.495) lowpass[hilp_freq] = 0.495;
	bool lowpassEngage = true; if (GetParameter( kParam_LOP ) == 0.0) lowpassEngage = false;
		
	highpass[hilp_aA0] = highpass[hilp_aB0];
	highpass[hilp_aA1] = highpass[hilp_aB1];
	highpass[hilp_bA1] = highpass[hilp_bB1];
	highpass[hilp_bA2] = highpass[hilp_bB2];
	highpass[hilp_cA0] = highpass[hilp_cB0];
	highpass[hilp_cA1] = highpass[hilp_cB1];
	highpass[hilp_dA1] = highpass[hilp_dB1];
	highpass[hilp_dA2] = highpass[hilp_dB2];
	highpass[hilp_eA0] = highpass[hilp_eB0];
	highpass[hilp_eA1] = highpass[hilp_eB1];
	highpass[hilp_fA1] = highpass[hilp_fB1];
	highpass[hilp_fA2] = highpass[hilp_fB2];
	lowpass[hilp_aA0] = lowpass[hilp_aB0];
	lowpass[hilp_aA1] = lowpass[hilp_aB1];
	lowpass[hilp_bA1] = lowpass[hilp_bB1];
	lowpass[hilp_bA2] = lowpass[hilp_bB2];
	lowpass[hilp_cA0] = lowpass[hilp_cB0];
	lowpass[hilp_cA1] = lowpass[hilp_cB1];
	lowpass[hilp_dA1] = lowpass[hilp_dB1];
	lowpass[hilp_dA2] = lowpass[hilp_dB2];
	lowpass[hilp_eA0] = lowpass[hilp_eB0];
	lowpass[hilp_eA1] = lowpass[hilp_eB1];
	lowpass[hilp_fA1] = lowpass[hilp_fB1];
	lowpass[hilp_fA2] = lowpass[hilp_fB2];
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.
	//On the buss, highpass and lowpass are isolators meant to be moved,
	//so they are interpolated where the channels are not
	
	double K = tan(M_PI * highpass[hilp_freq]); //highpass
	double norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
	highpass[hilp_aB0] = norm;
	highpass[hilp_aB1] = -2.0 * highpass[hilp_aB0];
	highpass[hilp_bB1] = 2.0 * (K * K - 1.0) * norm;
	highpass[hilp_bB2] = (1.0 - K / 1.93185165 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
	highpass[hilp_cB0] = norm;
	highpass[hilp_cB1] = -2.0 * highpass[hilp_cB0];
	highpass[hilp_dB1] = 2.0 * (K * K - 1.0) * norm;
	highpass[hilp_dB2] = (1.0 - K / 0.70710678 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
	highpass[hilp_eB0] = norm;
	highpass[hilp_eB1] = -2.0 * highpass[hilp_eB0];
	highpass[hilp_fB1] = 2.0 * (K * K - 1.0) * norm;
	highpass[hilp_fB2] = (1.0 - K / 0.51763809 + K * K) * norm;
	
	K = tan(M_PI * lowpass[hilp_freq]); //lowpass
	norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
	lowpass[hilp_aB0] = K * K * norm;
	lowpass[hilp_aB1] = 2.0 * lowpass[hilp_aB0];
	lowpass[hilp_bB1] = 2.0 * (K * K - 1.0) * norm;
	lowpass[hilp_bB2] = (1.0 - K / 1.93185165 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
	lowpass[hilp_cB0] = K * K * norm;
	lowpass[hilp_cB1] = 2.0 * lowpass[hilp_cB0];
	lowpass[hilp_dB1] = 2.0 * (K * K - 1.0) * norm;
	lowpass[hilp_dB2] = (1.0 - K / 0.70710678 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
	lowpass[hilp_eB0] = K * K * norm;
	lowpass[hilp_eB1] = 2.0 * lowpass[hilp_eB0];
	lowpass[hilp_fB1] = 2.0 * (K * K - 1.0) * norm;
	lowpass[hilp_fB2] = (1.0 - K / 0.51763809 + K * K) * norm;
	
	if (highpass[hilp_aA0] == 0.0) { // if we have just started, start directly with raw info
		highpass[hilp_aA0] = highpass[hilp_aB0];
		highpass[hilp_aA1] = highpass[hilp_aB1];
		highpass[hilp_bA1] = highpass[hilp_bB1];
		highpass[hilp_bA2] = highpass[hilp_bB2];
		highpass[hilp_cA0] = highpass[hilp_cB0];
		highpass[hilp_cA1] = highpass[hilp_cB1];
		highpass[hilp_dA1] = highpass[hilp_dB1];
		highpass[hilp_dA2] = highpass[hilp_dB2];
		highpass[hilp_eA0] = highpass[hilp_eB0];
		highpass[hilp_eA1] = highpass[hilp_eB1];
		highpass[hilp_fA1] = highpass[hilp_fB1];
		highpass[hilp_fA2] = highpass[hilp_fB2];
		lowpass[hilp_aA0] = lowpass[hilp_aB0];
		lowpass[hilp_aA1] = lowpass[hilp_aB1];
		lowpass[hilp_bA1] = lowpass[hilp_bB1];
		lowpass[hilp_bA2] = lowpass[hilp_bB2];
		lowpass[hilp_cA0] = lowpass[hilp_cB0];
		lowpass[hilp_cA1] = lowpass[hilp_cB1];
		lowpass[hilp_dA1] = lowpass[hilp_dB1];
		lowpass[hilp_dA2] = lowpass[hilp_dB2];
		lowpass[hilp_eA0] = lowpass[hilp_eB0];
		lowpass[hilp_eA1] = lowpass[hilp_eB1];
		lowpass[hilp_fA1] = lowpass[hilp_fB1];
		lowpass[hilp_fA2] = lowpass[hilp_fB2];
	}
	
	airGainA = airGainB; airGainB = GetParameter( kParam_AIR )*2.0;
	fireGainA = fireGainB; fireGainB = GetParameter( kParam_FIR )*2.0;
	stoneGainA = stoneGainB; stoneGainB = GetParameter( kParam_STO )*2.0;
	//simple three band to adjust
	double kalmanRange = 1.0-(pow(GetParameter( kParam_RNG ),2)/overallscale);
	//crossover frequency between mid/bass
	
	double compFThresh = pow(GetParameter( kParam_FCT ),4);
	double compSThresh = pow(GetParameter( kParam_SCT ),4);
	double compFRatio = 1.0-pow(1.0-GetParameter( kParam_FCR ),2);
	double compSRatio = 1.0-pow(1.0-GetParameter( kParam_SCR ),2);
	double compFAttack = 1.0/(((pow(GetParameter( kParam_FCA ),3)*5000.0)+500.0)*overallscale);
	double compSAttack = 1.0/(((pow(GetParameter( kParam_SCA ),3)*5000.0)+500.0)*overallscale);
	double compFRelease = 1.0/(((pow(GetParameter( kParam_FCL ),5)*50000.0)+500.0)*overallscale);
	double compSRelease = 1.0/(((pow(GetParameter( kParam_SCL ),5)*50000.0)+500.0)*overallscale);
	double gateFThresh = pow(GetParameter( kParam_FGT ),4);
	double gateSThresh = pow(GetParameter( kParam_SGT ),4);
	double gateFRatio = 1.0-pow(1.0-GetParameter( kParam_FGR ),2);
	double gateSRatio = 1.0-pow(1.0-GetParameter( kParam_SGR ),2);
	double gateFSustain = M_PI_2 * pow(GetParameter( kParam_FGS )+1.0,4.0);
	double gateSSustain = M_PI_2 * pow(GetParameter( kParam_SGS )+1.0,4.0);
	double gateFRelease = 1.0/(((pow(GetParameter( kParam_FGL ),5)*500000.0)+500.0)*overallscale);
	double gateSRelease = 1.0/(((pow(GetParameter( kParam_SGL ),5)*500000.0)+500.0)*overallscale);
		
	high[biqs_freq] = (((pow(GetParameter( kParam_TRF ),3)*14500.0)+1500.0)/GetSampleRate());
	if (high[biqs_freq] < 0.0001) high[biqs_freq] = 0.0001;
	high[biqs_nonlin] = GetParameter( kParam_TRG );
	high[biqs_level] = (high[biqs_nonlin]*2.0)-1.0;
	if (high[biqs_level] > 0.0) high[biqs_level] *= 2.0;
	high[biqs_reso] = ((0.5+(high[biqs_nonlin]*0.5)+sqrt(high[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_TRR ),2.0)))+0.5+(high[biqs_nonlin]*0.5);
	K = tan(M_PI * high[biqs_freq]);
	norm = 1.0 / (1.0 + K / (high[biqs_reso]*1.93185165) + K * K);
	high[biqs_a0] = K / (high[biqs_reso]*1.93185165) * norm;
	high[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	high[biqs_b2] = (1.0 - K / (high[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (high[biqs_reso]*0.70710678) + K * K);
	high[biqs_c0] = K / (high[biqs_reso]*0.70710678) * norm;
	high[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	high[biqs_d2] = (1.0 - K / (high[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (high[biqs_reso]*0.51763809) + K * K);
	high[biqs_e0] = K / (high[biqs_reso]*0.51763809) * norm;
	high[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	high[biqs_f2] = (1.0 - K / (high[biqs_reso]*0.51763809) + K * K) * norm;
	//high
	
	hmid[biqs_freq] = (((pow(GetParameter( kParam_HMF ),3)*6400.0)+600.0)/GetSampleRate());
	if (hmid[biqs_freq] < 0.0001) hmid[biqs_freq] = 0.0001;
	hmid[biqs_nonlin] = GetParameter( kParam_HMG );
	hmid[biqs_level] = (hmid[biqs_nonlin]*2.0)-1.0;
	if (hmid[biqs_level] > 0.0) hmid[biqs_level] *= 2.0;
	hmid[biqs_reso] = ((0.5+(hmid[biqs_nonlin]*0.5)+sqrt(hmid[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_HMR ),2.0)))+0.5+(hmid[biqs_nonlin]*0.5);
	K = tan(M_PI * hmid[biqs_freq]);
	norm = 1.0 / (1.0 + K / (hmid[biqs_reso]*1.93185165) + K * K);
	hmid[biqs_a0] = K / (hmid[biqs_reso]*1.93185165) * norm;
	hmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	hmid[biqs_b2] = (1.0 - K / (hmid[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (hmid[biqs_reso]*0.70710678) + K * K);
	hmid[biqs_c0] = K / (hmid[biqs_reso]*0.70710678) * norm;
	hmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	hmid[biqs_d2] = (1.0 - K / (hmid[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (hmid[biqs_reso]*0.51763809) + K * K);
	hmid[biqs_e0] = K / (hmid[biqs_reso]*0.51763809) * norm;
	hmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	hmid[biqs_f2] = (1.0 - K / (hmid[biqs_reso]*0.51763809) + K * K) * norm;
	//hmid
	
	lmid[biqs_freq] = (((pow(GetParameter( kParam_LMF ),3)*2200.0)+200.0)/GetSampleRate());
	if (lmid[biqs_freq] < 0.0001) lmid[biqs_freq] = 0.0001;
	lmid[biqs_nonlin] = GetParameter( kParam_LMG );
	lmid[biqs_level] = (lmid[biqs_nonlin]*2.0)-1.0;
	if (lmid[biqs_level] > 0.0) lmid[biqs_level] *= 2.0;
	lmid[biqs_reso] = ((0.5+(lmid[biqs_nonlin]*0.5)+sqrt(lmid[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_LMR ),2.0)))+0.5+(lmid[biqs_nonlin]*0.5);
	K = tan(M_PI * lmid[biqs_freq]);
	norm = 1.0 / (1.0 + K / (lmid[biqs_reso]*1.93185165) + K * K);
	lmid[biqs_a0] = K / (lmid[biqs_reso]*1.93185165) * norm;
	lmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	lmid[biqs_b2] = (1.0 - K / (lmid[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (lmid[biqs_reso]*0.70710678) + K * K);
	lmid[biqs_c0] = K / (lmid[biqs_reso]*0.70710678) * norm;
	lmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	lmid[biqs_d2] = (1.0 - K / (lmid[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (lmid[biqs_reso]*0.51763809) + K * K);
	lmid[biqs_e0] = K / (lmid[biqs_reso]*0.51763809) * norm;
	lmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	lmid[biqs_f2] = (1.0 - K / (lmid[biqs_reso]*0.51763809) + K * K) * norm;
	//lmid
	
	bass[biqs_freq] = (((pow(GetParameter( kParam_BSF ),3)*570.0)+30.0)/GetSampleRate());
	if (bass[biqs_freq] < 0.0001) bass[biqs_freq] = 0.0001;
	bass[biqs_nonlin] = GetParameter( kParam_BSG );
	bass[biqs_level] = (bass[biqs_nonlin]*2.0)-1.0;
	if (bass[biqs_level] > 0.0) bass[biqs_level] *= 2.0;
	bass[biqs_reso] = ((0.5+(bass[biqs_nonlin]*0.5)+sqrt(bass[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_BSR ),2.0)))+0.5+(bass[biqs_nonlin]*0.5);
	K = tan(M_PI * bass[biqs_freq]);
	norm = 1.0 / (1.0 + K / (bass[biqs_reso]*1.93185165) + K * K);
	bass[biqs_a0] = K / (bass[biqs_reso]*1.93185165) * norm;
	bass[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	bass[biqs_b2] = (1.0 - K / (bass[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (bass[biqs_reso]*0.70710678) + K * K);
	bass[biqs_c0] = K / (bass[biqs_reso]*0.70710678) * norm;
	bass[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	bass[biqs_d2] = (1.0 - K / (bass[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (bass[biqs_reso]*0.51763809) + K * K);
	bass[biqs_e0] = K / (bass[biqs_reso]*0.51763809) * norm;
	bass[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	bass[biqs_f2] = (1.0 - K / (bass[biqs_reso]*0.51763809) + K * K) * norm;
	//bass

	double refdB = GetParameter( kParam_DSC );
	double topdB = 0.000000075 * pow(10.0,refdB/20.0) * overallscale;
	
	panA = panB; panB = GetParameter( kParam_PAN )*1.57079633;
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_FAD )*2.0;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double temp = (double)nSampleFrames/inFramesToProcess;
		highpass[hilp_a0] = (highpass[hilp_aA0]*temp)+(highpass[hilp_aB0]*(1.0-temp));
		highpass[hilp_a1] = (highpass[hilp_aA1]*temp)+(highpass[hilp_aB1]*(1.0-temp));
		highpass[hilp_b1] = (highpass[hilp_bA1]*temp)+(highpass[hilp_bB1]*(1.0-temp));
		highpass[hilp_b2] = (highpass[hilp_bA2]*temp)+(highpass[hilp_bB2]*(1.0-temp));
		highpass[hilp_c0] = (highpass[hilp_cA0]*temp)+(highpass[hilp_cB0]*(1.0-temp));
		highpass[hilp_c1] = (highpass[hilp_cA1]*temp)+(highpass[hilp_cB1]*(1.0-temp));
		highpass[hilp_d1] = (highpass[hilp_dA1]*temp)+(highpass[hilp_dB1]*(1.0-temp));
		highpass[hilp_d2] = (highpass[hilp_dA2]*temp)+(highpass[hilp_dB2]*(1.0-temp));
		highpass[hilp_e0] = (highpass[hilp_eA0]*temp)+(highpass[hilp_eB0]*(1.0-temp));
		highpass[hilp_e1] = (highpass[hilp_eA1]*temp)+(highpass[hilp_eB1]*(1.0-temp));
		highpass[hilp_f1] = (highpass[hilp_fA1]*temp)+(highpass[hilp_fB1]*(1.0-temp));
		highpass[hilp_f2] = (highpass[hilp_fA2]*temp)+(highpass[hilp_fB2]*(1.0-temp));
		lowpass[hilp_a0] = (lowpass[hilp_aA0]*temp)+(lowpass[hilp_aB0]*(1.0-temp));
		lowpass[hilp_a1] = (lowpass[hilp_aA1]*temp)+(lowpass[hilp_aB1]*(1.0-temp));
		lowpass[hilp_b1] = (lowpass[hilp_bA1]*temp)+(lowpass[hilp_bB1]*(1.0-temp));
		lowpass[hilp_b2] = (lowpass[hilp_bA2]*temp)+(lowpass[hilp_bB2]*(1.0-temp));
		lowpass[hilp_c0] = (lowpass[hilp_cA0]*temp)+(lowpass[hilp_cB0]*(1.0-temp));
		lowpass[hilp_c1] = (lowpass[hilp_cA1]*temp)+(lowpass[hilp_cB1]*(1.0-temp));
		lowpass[hilp_d1] = (lowpass[hilp_dA1]*temp)+(lowpass[hilp_dB1]*(1.0-temp));
		lowpass[hilp_d2] = (lowpass[hilp_dA2]*temp)+(lowpass[hilp_dB2]*(1.0-temp));
		lowpass[hilp_e0] = (lowpass[hilp_eA0]*temp)+(lowpass[hilp_eB0]*(1.0-temp));
		lowpass[hilp_e1] = (lowpass[hilp_eA1]*temp)+(lowpass[hilp_eB1]*(1.0-temp));
		lowpass[hilp_f1] = (lowpass[hilp_fA1]*temp)+(lowpass[hilp_fB1]*(1.0-temp));
		lowpass[hilp_f2] = (lowpass[hilp_fA2]*temp)+(lowpass[hilp_fB2]*(1.0-temp));
		double gainR = (panA*temp)+(panB*(1.0-temp));
		double gainL = 1.57079633-gainR;
		gainR = sin(gainR); gainL = sin(gainL);
		double gain = (inTrimA*temp)+(inTrimB*(1.0-temp));
		if (gain > 1.0) gain *= gain; else gain = 1.0-pow(1.0-gain,2);
		gain *= 1.527864045000421;
		double airGain = (airGainA*temp)+(airGainB*(1.0-temp));
		if (airGain > 1.0) airGain *= airGain; else airGain = 1.0-pow(1.0-airGain,2);
		double fireGain = (fireGainA*temp)+(fireGainB*(1.0-temp));
		if (fireGain > 1.0) fireGain *= fireGain; else fireGain = 1.0-pow(1.0-fireGain,2);
		double firePad = fireGain; if (firePad > 1.0) firePad = 1.0;
		double stoneGain = (stoneGainA*temp)+(stoneGainB*(1.0-temp));
		if (stoneGain > 1.0) stoneGain *= stoneGain; else stoneGain = 1.0-pow(1.0-stoneGain,2);
		double stonePad = stoneGain; if (stonePad > 1.0) stonePad = 1.0;
		//set up smoothed gain controls		
		
		if (highpassEngage) { //distributed Highpass
			highpass[hilp_temp] = (inputSampleL*highpass[hilp_a0])+highpass[hilp_aL1];
			highpass[hilp_aL1] = (inputSampleL*highpass[hilp_a1])-(highpass[hilp_temp]*highpass[hilp_b1])+highpass[hilp_aL2];
			highpass[hilp_aL2] = (inputSampleL*highpass[hilp_a0])-(highpass[hilp_temp]*highpass[hilp_b2]); inputSampleL = highpass[hilp_temp];
			highpass[hilp_temp] = (inputSampleR*highpass[hilp_a0])+highpass[hilp_aR1];
			highpass[hilp_aR1] = (inputSampleR*highpass[hilp_a1])-(highpass[hilp_temp]*highpass[hilp_b1])+highpass[hilp_aR2];
			highpass[hilp_aR2] = (inputSampleR*highpass[hilp_a0])-(highpass[hilp_temp]*highpass[hilp_b2]); inputSampleR = highpass[hilp_temp];
		} else highpass[hilp_aR1] = highpass[hilp_aR2] = highpass[hilp_aL1] = highpass[hilp_aL2] = 0.0;
		if (lowpassEngage) { //distributed Lowpass
			lowpass[hilp_temp] = (inputSampleL*lowpass[hilp_a0])+lowpass[hilp_aL1];
			lowpass[hilp_aL1] = (inputSampleL*lowpass[hilp_a1])-(lowpass[hilp_temp]*lowpass[hilp_b1])+lowpass[hilp_aL2];
			lowpass[hilp_aL2] = (inputSampleL*lowpass[hilp_a0])-(lowpass[hilp_temp]*lowpass[hilp_b2]); inputSampleL = lowpass[hilp_temp];
			lowpass[hilp_temp] = (inputSampleR*lowpass[hilp_a0])+lowpass[hilp_aR1];
			lowpass[hilp_aR1] = (inputSampleR*lowpass[hilp_a1])-(lowpass[hilp_temp]*lowpass[hilp_b1])+lowpass[hilp_aR2];
			lowpass[hilp_aR2] = (inputSampleR*lowpass[hilp_a0])-(lowpass[hilp_temp]*lowpass[hilp_b2]); inputSampleR = lowpass[hilp_temp];
		} else lowpass[hilp_aR1] = lowpass[hilp_aR2] = lowpass[hilp_aL1] = lowpass[hilp_aL2] = 0.0;
		//first Highpass/Lowpass blocks aliasing before the nonlinearity of ConsoleXBuss and Parametric

		//ConsoleXBuss after initial Highpass/Lowpass stages
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949));
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949));
		inputSampleL *= 1.6180339887498949;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 0.6180339887498949));
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 0.6180339887498949));
		inputSampleR *= 1.6180339887498949;
		//ConsoleXBuss after initial Highpass/Lowpass stages
		
		//get all Parametric bands before any other processing is done
		//begin Stacked Biquad With Reversed Neutron Flow L
		high[biqs_outL] = inputSampleL * fabs(high[biqs_level]);
		high[biqs_dis] = fabs(high[biqs_a0] * (1.0+(high[biqs_outL]*high[biqs_nonlin])));
		if (high[biqs_dis] > 1.0) high[biqs_dis] = 1.0;
		high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_aL1];
		high[biqs_aL1] = high[biqs_aL2] - (high[biqs_temp]*high[biqs_b1]);
		high[biqs_aL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp]*high[biqs_b2]);
		high[biqs_outL] = high[biqs_temp];
		high[biqs_dis] = fabs(high[biqs_c0] * (1.0+(high[biqs_outL]*high[biqs_nonlin])));
		if (high[biqs_dis] > 1.0) high[biqs_dis] = 1.0;
		high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_cL1];
		high[biqs_cL1] = high[biqs_cL2] - (high[biqs_temp]*high[biqs_d1]);
		high[biqs_cL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp]*high[biqs_d2]);
		high[biqs_outL] = high[biqs_temp];
		high[biqs_dis] = fabs(high[biqs_e0] * (1.0+(high[biqs_outL]*high[biqs_nonlin])));
		if (high[biqs_dis] > 1.0) high[biqs_dis] = 1.0;
		high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_eL1];
		high[biqs_eL1] = high[biqs_eL2] - (high[biqs_temp]*high[biqs_f1]);
		high[biqs_eL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp]*high[biqs_f2]);
		high[biqs_outL] = high[biqs_temp]; high[biqs_outL] *= high[biqs_level];
		if (high[biqs_level] > 1.0) high[biqs_outL] *= high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		hmid[biqs_outL] = inputSampleL * fabs(hmid[biqs_level]);
		hmid[biqs_dis] = fabs(hmid[biqs_a0] * (1.0+(hmid[biqs_outL]*hmid[biqs_nonlin])));
		if (hmid[biqs_dis] > 1.0) hmid[biqs_dis] = 1.0;
		hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_aL1];
		hmid[biqs_aL1] = hmid[biqs_aL2] - (hmid[biqs_temp]*hmid[biqs_b1]);
		hmid[biqs_aL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp]*hmid[biqs_b2]);
		hmid[biqs_outL] = hmid[biqs_temp];
		hmid[biqs_dis] = fabs(hmid[biqs_c0] * (1.0+(hmid[biqs_outL]*hmid[biqs_nonlin])));
		if (hmid[biqs_dis] > 1.0) hmid[biqs_dis] = 1.0;
		hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_cL1];
		hmid[biqs_cL1] = hmid[biqs_cL2] - (hmid[biqs_temp]*hmid[biqs_d1]);
		hmid[biqs_cL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp]*hmid[biqs_d2]);
		hmid[biqs_outL] = hmid[biqs_temp];
		hmid[biqs_dis] = fabs(hmid[biqs_e0] * (1.0+(hmid[biqs_outL]*hmid[biqs_nonlin])));
		if (hmid[biqs_dis] > 1.0) hmid[biqs_dis] = 1.0;
		hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_eL1];
		hmid[biqs_eL1] = hmid[biqs_eL2] - (hmid[biqs_temp]*hmid[biqs_f1]);
		hmid[biqs_eL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp]*hmid[biqs_f2]);
		hmid[biqs_outL] = hmid[biqs_temp]; hmid[biqs_outL] *= hmid[biqs_level];
		if (hmid[biqs_level] > 1.0) hmid[biqs_outL] *= hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		lmid[biqs_outL] = inputSampleL * fabs(lmid[biqs_level]);
		lmid[biqs_dis] = fabs(lmid[biqs_a0] * (1.0+(lmid[biqs_outL]*lmid[biqs_nonlin])));
		if (lmid[biqs_dis] > 1.0) lmid[biqs_dis] = 1.0;
		lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_aL1];
		lmid[biqs_aL1] = lmid[biqs_aL2] - (lmid[biqs_temp]*lmid[biqs_b1]);
		lmid[biqs_aL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp]*lmid[biqs_b2]);
		lmid[biqs_outL] = lmid[biqs_temp];
		lmid[biqs_dis] = fabs(lmid[biqs_c0] * (1.0+(lmid[biqs_outL]*lmid[biqs_nonlin])));
		if (lmid[biqs_dis] > 1.0) lmid[biqs_dis] = 1.0;
		lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_cL1];
		lmid[biqs_cL1] = lmid[biqs_cL2] - (lmid[biqs_temp]*lmid[biqs_d1]);
		lmid[biqs_cL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp]*lmid[biqs_d2]);
		lmid[biqs_outL] = lmid[biqs_temp];
		lmid[biqs_dis] = fabs(lmid[biqs_e0] * (1.0+(lmid[biqs_outL]*lmid[biqs_nonlin])));
		if (lmid[biqs_dis] > 1.0) lmid[biqs_dis] = 1.0;
		lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_eL1];
		lmid[biqs_eL1] = lmid[biqs_eL2] - (lmid[biqs_temp]*lmid[biqs_f1]);
		lmid[biqs_eL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp]*lmid[biqs_f2]);
		lmid[biqs_outL] = lmid[biqs_temp]; lmid[biqs_outL] *= lmid[biqs_level];
		if (lmid[biqs_level] > 1.0) lmid[biqs_outL] *= lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		bass[biqs_outL] = inputSampleL * fabs(bass[biqs_level]);
		bass[biqs_dis] = fabs(bass[biqs_a0] * (1.0+(bass[biqs_outL]*bass[biqs_nonlin])));
		if (bass[biqs_dis] > 1.0) bass[biqs_dis] = 1.0;
		bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_dis]) + bass[biqs_aL1];
		bass[biqs_aL1] = bass[biqs_aL2] - (bass[biqs_temp]*bass[biqs_b1]);
		bass[biqs_aL2] = (bass[biqs_outL] * -bass[biqs_dis]) - (bass[biqs_temp]*bass[biqs_b2]);
		bass[biqs_outL] = bass[biqs_temp];
		bass[biqs_dis] = fabs(bass[biqs_c0] * (1.0+(bass[biqs_outL]*bass[biqs_nonlin])));
		if (bass[biqs_dis] > 1.0) bass[biqs_dis] = 1.0;
		bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_dis]) + bass[biqs_cL1];
		bass[biqs_cL1] = bass[biqs_cL2] - (bass[biqs_temp]*bass[biqs_d1]);
		bass[biqs_cL2] = (bass[biqs_outL] * -bass[biqs_dis]) - (bass[biqs_temp]*bass[biqs_d2]);
		bass[biqs_outL] = bass[biqs_temp];
		bass[biqs_dis] = fabs(bass[biqs_e0] * (1.0+(bass[biqs_outL]*bass[biqs_nonlin])));
		if (bass[biqs_dis] > 1.0) bass[biqs_dis] = 1.0;
		bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_dis]) + bass[biqs_eL1];
		bass[biqs_eL1] = bass[biqs_eL2] - (bass[biqs_temp]*bass[biqs_f1]);
		bass[biqs_eL2] = (bass[biqs_outL] * -bass[biqs_dis]) - (bass[biqs_temp]*bass[biqs_f2]);
		bass[biqs_outL] = bass[biqs_temp]; bass[biqs_outL] *= bass[biqs_level];
		if (bass[biqs_level] > 1.0) bass[biqs_outL] *= bass[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		high[biqs_outR] = inputSampleR * fabs(high[biqs_level]);
		high[biqs_dis] = fabs(high[biqs_a0] * (1.0+(high[biqs_outR]*high[biqs_nonlin])));
		if (high[biqs_dis] > 1.0) high[biqs_dis] = 1.0;
		high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_aR1];
		high[biqs_aR1] = high[biqs_aR2] - (high[biqs_temp]*high[biqs_b1]);
		high[biqs_aR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp]*high[biqs_b2]);
		high[biqs_outR] = high[biqs_temp];
		high[biqs_dis] = fabs(high[biqs_c0] * (1.0+(high[biqs_outR]*high[biqs_nonlin])));
		if (high[biqs_dis] > 1.0) high[biqs_dis] = 1.0;
		high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_cR1];
		high[biqs_cR1] = high[biqs_cR2] - (high[biqs_temp]*high[biqs_d1]);
		high[biqs_cR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp]*high[biqs_d2]);
		high[biqs_outR] = high[biqs_temp];
		high[biqs_dis] = fabs(high[biqs_e0] * (1.0+(high[biqs_outR]*high[biqs_nonlin])));
		if (high[biqs_dis] > 1.0) high[biqs_dis] = 1.0;
		high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_eR1];
		high[biqs_eR1] = high[biqs_eR2] - (high[biqs_temp]*high[biqs_f1]);
		high[biqs_eR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp]*high[biqs_f2]);
		high[biqs_outR] = high[biqs_temp]; high[biqs_outR] *= high[biqs_level];
		if (high[biqs_level] > 1.0) high[biqs_outR] *= high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		hmid[biqs_outR] = inputSampleR * fabs(hmid[biqs_level]);
		hmid[biqs_dis] = fabs(hmid[biqs_a0] * (1.0+(hmid[biqs_outR]*hmid[biqs_nonlin])));
		if (hmid[biqs_dis] > 1.0) hmid[biqs_dis] = 1.0;
		hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_aR1];
		hmid[biqs_aR1] = hmid[biqs_aR2] - (hmid[biqs_temp]*hmid[biqs_b1]);
		hmid[biqs_aR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp]*hmid[biqs_b2]);
		hmid[biqs_outR] = hmid[biqs_temp];
		hmid[biqs_dis] = fabs(hmid[biqs_c0] * (1.0+(hmid[biqs_outR]*hmid[biqs_nonlin])));
		if (hmid[biqs_dis] > 1.0) hmid[biqs_dis] = 1.0;
		hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_cR1];
		hmid[biqs_cR1] = hmid[biqs_cR2] - (hmid[biqs_temp]*hmid[biqs_d1]);
		hmid[biqs_cR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp]*hmid[biqs_d2]);
		hmid[biqs_outR] = hmid[biqs_temp];
		hmid[biqs_dis] = fabs(hmid[biqs_e0] * (1.0+(hmid[biqs_outR]*hmid[biqs_nonlin])));
		if (hmid[biqs_dis] > 1.0) hmid[biqs_dis] = 1.0;
		hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_eR1];
		hmid[biqs_eR1] = hmid[biqs_eR2] - (hmid[biqs_temp]*hmid[biqs_f1]);
		hmid[biqs_eR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp]*hmid[biqs_f2]);
		hmid[biqs_outR] = hmid[biqs_temp]; hmid[biqs_outR] *= hmid[biqs_level];
		if (hmid[biqs_level] > 1.0) hmid[biqs_outR] *= hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		lmid[biqs_outR] = inputSampleR * fabs(lmid[biqs_level]);
		lmid[biqs_dis] = fabs(lmid[biqs_a0] * (1.0+(lmid[biqs_outR]*lmid[biqs_nonlin])));
		if (lmid[biqs_dis] > 1.0) lmid[biqs_dis] = 1.0;
		lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_aR1];
		lmid[biqs_aR1] = lmid[biqs_aR2] - (lmid[biqs_temp]*lmid[biqs_b1]);
		lmid[biqs_aR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp]*lmid[biqs_b2]);
		lmid[biqs_outR] = lmid[biqs_temp];
		lmid[biqs_dis] = fabs(lmid[biqs_c0] * (1.0+(lmid[biqs_outR]*lmid[biqs_nonlin])));
		if (lmid[biqs_dis] > 1.0) lmid[biqs_dis] = 1.0;
		lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_cR1];
		lmid[biqs_cR1] = lmid[biqs_cR2] - (lmid[biqs_temp]*lmid[biqs_d1]);
		lmid[biqs_cR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp]*lmid[biqs_d2]);
		lmid[biqs_outR] = lmid[biqs_temp];
		lmid[biqs_dis] = fabs(lmid[biqs_e0] * (1.0+(lmid[biqs_outR]*lmid[biqs_nonlin])));
		if (lmid[biqs_dis] > 1.0) lmid[biqs_dis] = 1.0;
		lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_eR1];
		lmid[biqs_eR1] = lmid[biqs_eR2] - (lmid[biqs_temp]*lmid[biqs_f1]);
		lmid[biqs_eR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp]*lmid[biqs_f2]);
		lmid[biqs_outR] = lmid[biqs_temp]; lmid[biqs_outR] *= lmid[biqs_level];
		if (lmid[biqs_level] > 1.0) lmid[biqs_outR] *= lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		bass[biqs_outR] = inputSampleR * fabs(bass[biqs_level]);
		bass[biqs_dis] = fabs(bass[biqs_a0] * (1.0+(bass[biqs_outR]*bass[biqs_nonlin])));
		if (bass[biqs_dis] > 1.0) bass[biqs_dis] = 1.0;
		bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_dis]) + bass[biqs_aR1];
		bass[biqs_aR1] = bass[biqs_aR2] - (bass[biqs_temp]*bass[biqs_b1]);
		bass[biqs_aR2] = (bass[biqs_outR] * -bass[biqs_dis]) - (bass[biqs_temp]*bass[biqs_b2]);
		bass[biqs_outR] = bass[biqs_temp];
		bass[biqs_dis] = fabs(bass[biqs_c0] * (1.0+(bass[biqs_outR]*bass[biqs_nonlin])));
		if (bass[biqs_dis] > 1.0) bass[biqs_dis] = 1.0;
		bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_dis]) + bass[biqs_cR1];
		bass[biqs_cR1] = bass[biqs_cR2] - (bass[biqs_temp]*bass[biqs_d1]);
		bass[biqs_cR2] = (bass[biqs_outR] * -bass[biqs_dis]) - (bass[biqs_temp]*bass[biqs_d2]);
		bass[biqs_outR] = bass[biqs_temp];
		bass[biqs_dis] = fabs(bass[biqs_e0] * (1.0+(bass[biqs_outR]*bass[biqs_nonlin])));
		if (bass[biqs_dis] > 1.0) bass[biqs_dis] = 1.0;
		bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_dis]) + bass[biqs_eR1];
		bass[biqs_eR1] = bass[biqs_eR2] - (bass[biqs_temp]*bass[biqs_f1]);
		bass[biqs_eR2] = (bass[biqs_outR] * -bass[biqs_dis]) - (bass[biqs_temp]*bass[biqs_f2]);
		bass[biqs_outR] = bass[biqs_temp]; bass[biqs_outR] *= bass[biqs_level];
		if (bass[biqs_level] > 1.0) bass[biqs_outR] *= bass[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
				
		//begin Air3L
		double drySampleL = inputSampleL;
		air[pvSL4] = air[pvAL4] - air[pvAL3]; air[pvSL3] = air[pvAL3] - air[pvAL2];
		air[pvSL2] = air[pvAL2] - air[pvAL1]; air[pvSL1] = air[pvAL1] - inputSampleL;
		air[accSL3] = air[pvSL4] - air[pvSL3]; air[accSL2] = air[pvSL3] - air[pvSL2];
		air[accSL1] = air[pvSL2] - air[pvSL1];
		air[acc2SL2] = air[accSL3] - air[accSL2]; air[acc2SL1] = air[accSL2] - air[accSL1];		
		air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1])*0.5));
		air[gainAL] *= 0.5; air[gainAL] += fabs(drySampleL-air[outAL])*0.5;
		if (air[gainAL] > 0.3*sqrt(overallscale)) air[gainAL] = 0.3*sqrt(overallscale);
		air[pvAL4] = air[pvAL3]; air[pvAL3] = air[pvAL2];
		air[pvAL2] = air[pvAL1]; air[pvAL1] = (air[gainAL] * air[outAL]) + drySampleL;
		double fireL = drySampleL - ((air[outAL]*0.5)+(drySampleL*(0.457-(0.017*overallscale))));
		temp = (fireL + air[gndavgL])*0.5; air[gndavgL] = fireL; fireL = temp;
		double airL = (drySampleL-fireL)*airGain;
		inputSampleL = fireL;
		//end Air3L
		//begin Air3R
		double drySampleR = inputSampleR;
		air[pvSR4] = air[pvAR4] - air[pvAR3]; air[pvSR3] = air[pvAR3] - air[pvAR2];
		air[pvSR2] = air[pvAR2] - air[pvAR1]; air[pvSR1] = air[pvAR1] - inputSampleR;
		air[accSR3] = air[pvSR4] - air[pvSR3]; air[accSR2] = air[pvSR3] - air[pvSR2];
		air[accSR1] = air[pvSR2] - air[pvSR1];
		air[acc2SR2] = air[accSR3] - air[accSR2]; air[acc2SR1] = air[accSR2] - air[accSR1];		
		air[outAR] = -(air[pvAR1] + air[pvSR3] + air[acc2SR2] - ((air[acc2SR2] + air[acc2SR1])*0.5));
		air[gainAR] *= 0.5; air[gainAR] += fabs(drySampleR-air[outAR])*0.5;
		if (air[gainAR] > 0.3*sqrt(overallscale)) air[gainAR] = 0.3*sqrt(overallscale);
		air[pvAR4] = air[pvAR3]; air[pvAR3] = air[pvAR2];
		air[pvAR2] = air[pvAR1]; air[pvAR1] = (air[gainAR] * air[outAR]) + drySampleR;
		double fireR = drySampleR - ((air[outAR]*0.5)+(drySampleR*(0.457-(0.017*overallscale))));
		temp = (fireR + air[gndavgR])*0.5; air[gndavgR] = fireR; fireR = temp;
		double airR = (drySampleR-fireR)*airGain;
		inputSampleR = fireR;
		//end Air3R
		//begin KalmanL
		temp = inputSampleL = inputSampleL*(1.0-kalmanRange)*0.777;
		inputSampleL *= (1.0-kalmanRange);
		//set up gain levels to control the beast
		kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2]; kal[prevSlewL3] *= 0.5;
		kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1]; kal[prevSlewL2] *= 0.5;
		kal[prevSlewL1] += kal[prevSampL1] - inputSampleL; kal[prevSlewL1] *= 0.5;
		//make slews from each set of samples used
		kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2]; kal[accSlewL2] *= 0.5;
		kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1]; kal[accSlewL1] *= 0.5;
		//differences between slews: rate of change of rate of change
		kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]); kal[accSlewL3] *= 0.5;
		//entering the abyss, what even is this
		kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3]; kal[kalOutL] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		kal[kalGainL] += fabs(temp-kal[kalOutL])*kalmanRange*8.0; kal[kalGainL] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (kal[kalGainL] > kalmanRange*0.5) kal[kalGainL] = kalmanRange*0.5;
		//attempts to avoid explosions
		kal[kalOutL] += (temp*(1.0-(0.68+(kalmanRange*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		kal[prevSampL3] = kal[prevSampL2]; kal[prevSampL2] = kal[prevSampL1];
		kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0-kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (kal[prevSampL1] > 1.0) kal[prevSampL1] = 1.0; if (kal[prevSampL1] < -1.0) kal[prevSampL1] = -1.0;
		double stoneL = kal[kalOutL]*0.777;
		fireL -= stoneL;
		//end KalmanL
		//begin KalmanR
		temp = inputSampleR = inputSampleR*(1.0-kalmanRange)*0.777;
		inputSampleR *= (1.0-kalmanRange);
		//set up gain levels to control the beast
		kal[prevSlewR3] += kal[prevSampR3] - kal[prevSampR2]; kal[prevSlewR3] *= 0.5;
		kal[prevSlewR2] += kal[prevSampR2] - kal[prevSampR1]; kal[prevSlewR2] *= 0.5;
		kal[prevSlewR1] += kal[prevSampR1] - inputSampleR; kal[prevSlewR1] *= 0.5;
		//make slews from each set of samples used
		kal[accSlewR2] += kal[prevSlewR3] - kal[prevSlewR2]; kal[accSlewR2] *= 0.5;
		kal[accSlewR1] += kal[prevSlewR2] - kal[prevSlewR1]; kal[accSlewR1] *= 0.5;
		//differences between slews: rate of change of rate of change
		kal[accSlewR3] += (kal[accSlewR2] - kal[accSlewR1]); kal[accSlewR3] *= 0.5;
		//entering the abyss, what even is this
		kal[kalOutR] += kal[prevSampR1] + kal[prevSlewR2] + kal[accSlewR3]; kal[kalOutR] *= 0.5;
		//resynthesizing predicted result (all iir smoothed)
		kal[kalGainR] += fabs(temp-kal[kalOutR])*kalmanRange*8.0; kal[kalGainR] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (kal[kalGainR] > kalmanRange*0.5) kal[kalGainR] = kalmanRange*0.5;
		//attempts to avoid explosions
		kal[kalOutR] += (temp*(1.0-(0.68+(kalmanRange*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		kal[prevSampR3] = kal[prevSampR2]; kal[prevSampR2] = kal[prevSampR1];
		kal[prevSampR1] = (kal[kalGainR] * kal[kalOutR]) + ((1.0-kal[kalGainR])*temp);
		//feed the chain of previous samples
		if (kal[prevSampR1] > 1.0) kal[prevSampR1] = 1.0; if (kal[prevSampR1] < -1.0) kal[prevSampR1] = -1.0;
		double stoneR = kal[kalOutR]*0.777;
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
		if (fabs(fireL) > gateFThresh) fireGate = gateFSustain;
		else if (fabs(fireR) > gateFThresh) fireGate = gateFSustain;
		else fireGate *= (1.0-gateFRelease);
		if (fireGate < 0.0) fireGate = 0.0;
		fireCompL = fmax(fmin(fireCompL,1.0),0.0);
		fireCompR = fmax(fmin(fireCompR,1.0),0.0);
		fireL *= (((1.0-compFRatio)*firePad)+(fireCompL*compFRatio*fireGain));
		fireR *= (((1.0-compFRatio)*firePad)+(fireCompR*compFRatio*fireGain));
		if (fireGate < M_PI_2) {
			temp = ((1.0-gateFRatio)+(sin(fireGate)*gateFRatio));
			airL *= temp;
			airR *= temp;
			fireL *= temp;
			fireR *= temp;
			high[biqs_outL] *= temp;
			high[biqs_outR] *= temp;
			hmid[biqs_outL] *= temp; //if Fire gating, gate Air, high and hmid
			hmid[biqs_outR] *= temp; //note that we aren't compressing these
		}
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
		if (fabs(stoneL) > gateSThresh) stoneGate = gateSSustain;
		else if (fabs(stoneR) > gateSThresh) stoneGate = gateSSustain;
		else stoneGate *= (1.0-gateSRelease);
		if (stoneGate < 0.0) stoneGate = 0.0;
		stoneCompL = fmax(fmin(stoneCompL,1.0),0.0);
		stoneCompR = fmax(fmin(stoneCompR,1.0),0.0);
		stoneL *= (((1.0-compSRatio)*stonePad)+(stoneCompL*compSRatio*stoneGain));
		stoneR *= (((1.0-compSRatio)*stonePad)+(stoneCompR*compSRatio*stoneGain));
		if (stoneGate < M_PI_2) {
			temp = ((1.0-gateSRatio)+(sin(stoneGate)*gateSRatio));
			stoneL *= temp;
			stoneR *= temp;
			lmid[biqs_outL] *= temp;
			lmid[biqs_outR] *= temp;
			bass[biqs_outL] *= temp; //if Stone gating, gate lmid and bass
			bass[biqs_outR] *= temp; //note that we aren't compressing these
		}
		inputSampleL = stoneL + fireL + airL;
		inputSampleR = stoneR + fireR + airR;
		//create Stonefire output
		
		if (highpassEngage) { //distributed Highpass
			highpass[hilp_temp] = (inputSampleL*highpass[hilp_c0])+highpass[hilp_cL1];
			highpass[hilp_cL1] = (inputSampleL*highpass[hilp_c1])-(highpass[hilp_temp]*highpass[hilp_d1])+highpass[hilp_cL2];
			highpass[hilp_cL2] = (inputSampleL*highpass[hilp_c0])-(highpass[hilp_temp]*highpass[hilp_d2]); inputSampleL = highpass[hilp_temp];
			highpass[hilp_temp] = (inputSampleR*highpass[hilp_c0])+highpass[hilp_cR1];
			highpass[hilp_cR1] = (inputSampleR*highpass[hilp_c1])-(highpass[hilp_temp]*highpass[hilp_d1])+highpass[hilp_cR2];
			highpass[hilp_cR2] = (inputSampleR*highpass[hilp_c0])-(highpass[hilp_temp]*highpass[hilp_d2]); inputSampleR = highpass[hilp_temp];
		} else highpass[hilp_cR1] = highpass[hilp_cR2] = highpass[hilp_cL1] = highpass[hilp_cL2] = 0.0;
		if (lowpassEngage) { //distributed Lowpass
			lowpass[hilp_temp] = (inputSampleL*lowpass[hilp_c0])+lowpass[hilp_cL1];
			lowpass[hilp_cL1] = (inputSampleL*lowpass[hilp_c1])-(lowpass[hilp_temp]*lowpass[hilp_d1])+lowpass[hilp_cL2];
			lowpass[hilp_cL2] = (inputSampleL*lowpass[hilp_c0])-(lowpass[hilp_temp]*lowpass[hilp_d2]); inputSampleL = lowpass[hilp_temp];
			lowpass[hilp_temp] = (inputSampleR*lowpass[hilp_c0])+lowpass[hilp_cR1];
			lowpass[hilp_cR1] = (inputSampleR*lowpass[hilp_c1])-(lowpass[hilp_temp]*lowpass[hilp_d1])+lowpass[hilp_cR2];
			lowpass[hilp_cR2] = (inputSampleR*lowpass[hilp_c0])-(lowpass[hilp_temp]*lowpass[hilp_d2]); inputSampleR = lowpass[hilp_temp];
		} else lowpass[hilp_cR1] = lowpass[hilp_cR2] = lowpass[hilp_cL1] = lowpass[hilp_cL2] = 0.0;
		//another stage of Highpass/Lowpass before bringing in the parametric bands
		
		inputSampleL += (high[biqs_outL] + hmid[biqs_outL] + lmid[biqs_outL] + bass[biqs_outL]);
		inputSampleR += (high[biqs_outR] + hmid[biqs_outR] + lmid[biqs_outR] + bass[biqs_outR]);
		//add parametric boosts or cuts: clean as possible for maximal rawness and sonority		
		
		inputSampleL = inputSampleL * gainL * gain;
		inputSampleR = inputSampleR * gainR * gain;
		//applies pan section, and smoothed fader gain
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222) inputSampleL = -0.222; if (inputSampleL > 0.222) inputSampleL = 0.222;
		dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
		int dBdly = floor(dBaPosL*dscBuf);
		double dBi = (dBaPosL*dscBuf)-dBdly;
		inputSampleL = dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleL += dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		inputSampleL /= topdB;		
		inputSampleR *= topdB;
		if (inputSampleR < -0.222) inputSampleR = -0.222; if (inputSampleR > 0.222) inputSampleR = 0.222;
		dBaR[dBaXR] = inputSampleR; dBaPosR *= 0.5; dBaPosR += fabs((inputSampleR*((inputSampleR*0.25)-0.5))*0.5);
		dBdly = floor(dBaPosR*dscBuf);
		dBi = (dBaPosR*dscBuf)-dBdly;
		inputSampleR = dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleR += dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
		inputSampleR /= topdB;		
		//top dB processing for distributed discontinuity modeling air nonlinearity		
		
		if (highpassEngage) { //distributed Highpass
			highpass[hilp_temp] = (inputSampleL*highpass[hilp_e0])+highpass[hilp_eL1];
			highpass[hilp_eL1] = (inputSampleL*highpass[hilp_e1])-(highpass[hilp_temp]*highpass[hilp_f1])+highpass[hilp_eL2];
			highpass[hilp_eL2] = (inputSampleL*highpass[hilp_e0])-(highpass[hilp_temp]*highpass[hilp_f2]); inputSampleL = highpass[hilp_temp];
			highpass[hilp_temp] = (inputSampleR*highpass[hilp_e0])+highpass[hilp_eR1];
			highpass[hilp_eR1] = (inputSampleR*highpass[hilp_e1])-(highpass[hilp_temp]*highpass[hilp_f1])+highpass[hilp_eR2];
			highpass[hilp_eR2] = (inputSampleR*highpass[hilp_e0])-(highpass[hilp_temp]*highpass[hilp_f2]); inputSampleR = highpass[hilp_temp];
		} else highpass[hilp_eR1] = highpass[hilp_eR2] = highpass[hilp_eL1] = highpass[hilp_eL2] = 0.0;
		if (lowpassEngage) { //distributed Lowpass
			lowpass[hilp_temp] = (inputSampleL*lowpass[hilp_e0])+lowpass[hilp_eL1];
			lowpass[hilp_eL1] = (inputSampleL*lowpass[hilp_e1])-(lowpass[hilp_temp]*lowpass[hilp_f1])+lowpass[hilp_eL2];
			lowpass[hilp_eL2] = (inputSampleL*lowpass[hilp_e0])-(lowpass[hilp_temp]*lowpass[hilp_f2]); inputSampleL = lowpass[hilp_temp];
			lowpass[hilp_temp] = (inputSampleR*lowpass[hilp_e0])+lowpass[hilp_eR1];
			lowpass[hilp_eR1] = (inputSampleR*lowpass[hilp_e1])-(lowpass[hilp_temp]*lowpass[hilp_f1])+lowpass[hilp_eR2];
			lowpass[hilp_eR2] = (inputSampleR*lowpass[hilp_e0])-(lowpass[hilp_temp]*lowpass[hilp_f2]); inputSampleR = lowpass[hilp_temp];
		} else lowpass[hilp_eR1] = lowpass[hilp_eR2] = lowpass[hilp_eL1] = lowpass[hilp_eL2] = 0.0;		
		//final Highpass/Lowpass continues to address aliasing
		//final stacked biquad section is the softest Q for smoothness

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
	for (int x = 0; x < hilp_total; x++) {
		highpass[x] = 0.0;
		lowpass[x] = 0.0;
	}
	
	for (int x = 0; x < air_total; x++) air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) kal[x] = 0.0;
	fireCompL = 1.0;
	fireCompR = 1.0;
	fireGate = 1.0;
	stoneCompL = 1.0;
	stoneCompR = 1.0;
	stoneGate = 1.0;	
	
	for (int x = 0; x < biqs_total; x++) {
		high[x] = 0.0;
		hmid[x] = 0.0;
		lmid[x] = 0.0;
		bass[x] = 0.0;
	}

	for(int count = 0; count < dscBuf+2; count++) {
		dBaL[count] = 0.0;
		dBaR[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBaPosR = 0.0;
	dBaXL = 1;
	dBaXR = 1;

	airGainA = 0.5; airGainB = 0.5;
	fireGainA = 0.5; fireGainB = 0.5;
	stoneGainA = 0.5; stoneGainB = 0.5;
	panA = 0.5; panB = 0.5;
	inTrimA = 1.0; inTrimB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
