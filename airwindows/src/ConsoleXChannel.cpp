#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleXChannel"
#define AIRWINDOWS_DESCRIPTION "The fully featured, biggest Airwindows console."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','7' )
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
		hilp_a0, hilp_a1, hilp_b1, hilp_b2,
		hilp_c0, hilp_c1, hilp_d1, hilp_d2,
		hilp_e0, hilp_e1, hilp_f1, hilp_f2,
		hilp_aL1, hilp_aL2, hilp_aR1, hilp_aR2,
		hilp_cL1, hilp_cL2, hilp_cR1, hilp_cR2,
		hilp_eL1, hilp_eL2, hilp_eR1, hilp_eR2,
		hilp_total
	};
	
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

	struct _dram {
		double highpass[hilp_total];
	double lowpass[hilp_total];
	double air[air_total];
	double kal[kal_total];
	double high[biqs_total];
	double hmid[biqs_total];
	double lmid[biqs_total];
	double bass[biqs_total];
	double dBaL[dscBuf+5];
	double dBaR[dscBuf+5];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	dram->highpass[hilp_freq] = ((GetParameter( kParam_HIP )*330.0)+20.0)/GetSampleRate();
	bool highpassEngage = true; if (GetParameter( kParam_HIP ) == 0.0) highpassEngage = false;
	
	dram->lowpass[hilp_freq] = ((pow(1.0-GetParameter( kParam_LOP ),2)*17000.0)+3000.0)/GetSampleRate();
	bool lowpassEngage = true; if (GetParameter( kParam_LOP ) == 0.0) lowpassEngage = false;
	
	double K = tan(M_PI * dram->highpass[hilp_freq]); //highpass
	double norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
	dram->highpass[hilp_a0] = norm;
	dram->highpass[hilp_a1] = -2.0 * dram->highpass[hilp_a0];
	dram->highpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->highpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
	dram->highpass[hilp_c0] = norm;
	dram->highpass[hilp_c1] = -2.0 * dram->highpass[hilp_c0];
	dram->highpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
	dram->highpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
	dram->highpass[hilp_e0] = norm;
	dram->highpass[hilp_e1] = -2.0 * dram->highpass[hilp_e0];
	dram->highpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
	dram->highpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
	
	K = tan(M_PI * dram->lowpass[hilp_freq]); //lowpass
	norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
	dram->lowpass[hilp_a0] = K * K * norm;
	dram->lowpass[hilp_a1] = 2.0 * dram->lowpass[hilp_a0];
	dram->lowpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->lowpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
	dram->lowpass[hilp_c0] = K * K * norm;
	dram->lowpass[hilp_c1] = 2.0 * dram->lowpass[hilp_c0];
	dram->lowpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
	dram->lowpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
	norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
	dram->lowpass[hilp_e0] = K * K * norm;
	dram->lowpass[hilp_e1] = 2.0 * dram->lowpass[hilp_e0];
	dram->lowpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
	dram->lowpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
	
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
		
	dram->high[biqs_freq] = (((pow(GetParameter( kParam_TRF ),3)*14500.0)+1500.0)/GetSampleRate());
	if (dram->high[biqs_freq] < 0.0001) dram->high[biqs_freq] = 0.0001;
	dram->high[biqs_nonlin] = GetParameter( kParam_TRG );
	dram->high[biqs_level] = (dram->high[biqs_nonlin]*2.0)-1.0;
	if (dram->high[biqs_level] > 0.0) dram->high[biqs_level] *= 2.0;
	dram->high[biqs_reso] = ((0.5+(dram->high[biqs_nonlin]*0.5)+sqrt(dram->high[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_TRR ),2.0)))+0.5+(dram->high[biqs_nonlin]*0.5);
	K = tan(M_PI * dram->high[biqs_freq]);
	norm = 1.0 / (1.0 + K / (dram->high[biqs_reso]*1.93185165) + K * K);
	dram->high[biqs_a0] = K / (dram->high[biqs_reso]*1.93185165) * norm;
	dram->high[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->high[biqs_b2] = (1.0 - K / (dram->high[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->high[biqs_reso]*0.70710678) + K * K);
	dram->high[biqs_c0] = K / (dram->high[biqs_reso]*0.70710678) * norm;
	dram->high[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	dram->high[biqs_d2] = (1.0 - K / (dram->high[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->high[biqs_reso]*0.51763809) + K * K);
	dram->high[biqs_e0] = K / (dram->high[biqs_reso]*0.51763809) * norm;
	dram->high[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	dram->high[biqs_f2] = (1.0 - K / (dram->high[biqs_reso]*0.51763809) + K * K) * norm;
	//high
	
	dram->hmid[biqs_freq] = (((pow(GetParameter( kParam_HMF ),3)*6400.0)+600.0)/GetSampleRate());
	if (dram->hmid[biqs_freq] < 0.0001) dram->hmid[biqs_freq] = 0.0001;
	dram->hmid[biqs_nonlin] = GetParameter( kParam_HMG );
	dram->hmid[biqs_level] = (dram->hmid[biqs_nonlin]*2.0)-1.0;
	if (dram->hmid[biqs_level] > 0.0) dram->hmid[biqs_level] *= 2.0;
	dram->hmid[biqs_reso] = ((0.5+(dram->hmid[biqs_nonlin]*0.5)+sqrt(dram->hmid[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_HMR ),2.0)))+0.5+(dram->hmid[biqs_nonlin]*0.5);
	K = tan(M_PI * dram->hmid[biqs_freq]);
	norm = 1.0 / (1.0 + K / (dram->hmid[biqs_reso]*1.93185165) + K * K);
	dram->hmid[biqs_a0] = K / (dram->hmid[biqs_reso]*1.93185165) * norm;
	dram->hmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->hmid[biqs_b2] = (1.0 - K / (dram->hmid[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->hmid[biqs_reso]*0.70710678) + K * K);
	dram->hmid[biqs_c0] = K / (dram->hmid[biqs_reso]*0.70710678) * norm;
	dram->hmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	dram->hmid[biqs_d2] = (1.0 - K / (dram->hmid[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->hmid[biqs_reso]*0.51763809) + K * K);
	dram->hmid[biqs_e0] = K / (dram->hmid[biqs_reso]*0.51763809) * norm;
	dram->hmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	dram->hmid[biqs_f2] = (1.0 - K / (dram->hmid[biqs_reso]*0.51763809) + K * K) * norm;
	//hmid
	
	dram->lmid[biqs_freq] = (((pow(GetParameter( kParam_LMF ),3)*2200.0)+200.0)/GetSampleRate());
	if (dram->lmid[biqs_freq] < 0.0001) dram->lmid[biqs_freq] = 0.0001;
	dram->lmid[biqs_nonlin] = GetParameter( kParam_LMG );
	dram->lmid[biqs_level] = (dram->lmid[biqs_nonlin]*2.0)-1.0;
	if (dram->lmid[biqs_level] > 0.0) dram->lmid[biqs_level] *= 2.0;
	dram->lmid[biqs_reso] = ((0.5+(dram->lmid[biqs_nonlin]*0.5)+sqrt(dram->lmid[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_LMR ),2.0)))+0.5+(dram->lmid[biqs_nonlin]*0.5);
	K = tan(M_PI * dram->lmid[biqs_freq]);
	norm = 1.0 / (1.0 + K / (dram->lmid[biqs_reso]*1.93185165) + K * K);
	dram->lmid[biqs_a0] = K / (dram->lmid[biqs_reso]*1.93185165) * norm;
	dram->lmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->lmid[biqs_b2] = (1.0 - K / (dram->lmid[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->lmid[biqs_reso]*0.70710678) + K * K);
	dram->lmid[biqs_c0] = K / (dram->lmid[biqs_reso]*0.70710678) * norm;
	dram->lmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	dram->lmid[biqs_d2] = (1.0 - K / (dram->lmid[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->lmid[biqs_reso]*0.51763809) + K * K);
	dram->lmid[biqs_e0] = K / (dram->lmid[biqs_reso]*0.51763809) * norm;
	dram->lmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	dram->lmid[biqs_f2] = (1.0 - K / (dram->lmid[biqs_reso]*0.51763809) + K * K) * norm;
	//lmid
	
	dram->bass[biqs_freq] = (((pow(GetParameter( kParam_BSF ),3)*570.0)+30.0)/GetSampleRate());
	if (dram->bass[biqs_freq] < 0.0001) dram->bass[biqs_freq] = 0.0001;
	dram->bass[biqs_nonlin] = GetParameter( kParam_BSG );
	dram->bass[biqs_level] = (dram->bass[biqs_nonlin]*2.0)-1.0;
	if (dram->bass[biqs_level] > 0.0) dram->bass[biqs_level] *= 2.0;
	dram->bass[biqs_reso] = ((0.5+(dram->bass[biqs_nonlin]*0.5)+sqrt(dram->bass[biqs_freq]))-(1.0-pow(1.0-GetParameter( kParam_BSR ),2.0)))+0.5+(dram->bass[biqs_nonlin]*0.5);
	K = tan(M_PI * dram->bass[biqs_freq]);
	norm = 1.0 / (1.0 + K / (dram->bass[biqs_reso]*1.93185165) + K * K);
	dram->bass[biqs_a0] = K / (dram->bass[biqs_reso]*1.93185165) * norm;
	dram->bass[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
	dram->bass[biqs_b2] = (1.0 - K / (dram->bass[biqs_reso]*1.93185165) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->bass[biqs_reso]*0.70710678) + K * K);
	dram->bass[biqs_c0] = K / (dram->bass[biqs_reso]*0.70710678) * norm;
	dram->bass[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
	dram->bass[biqs_d2] = (1.0 - K / (dram->bass[biqs_reso]*0.70710678) + K * K) * norm;
	norm = 1.0 / (1.0 + K / (dram->bass[biqs_reso]*0.51763809) + K * K);
	dram->bass[biqs_e0] = K / (dram->bass[biqs_reso]*0.51763809) * norm;
	dram->bass[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
	dram->bass[biqs_f2] = (1.0 - K / (dram->bass[biqs_reso]*0.51763809) + K * K) * norm;
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
		
		if (highpassEngage) { //distributed Highpass
			dram->highpass[hilp_temp] = (inputSampleL*dram->highpass[hilp_a0])+dram->highpass[hilp_aL1];
			dram->highpass[hilp_aL1] = (inputSampleL*dram->highpass[hilp_a1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b1])+dram->highpass[hilp_aL2];
			dram->highpass[hilp_aL2] = (inputSampleL*dram->highpass[hilp_a0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b2]); inputSampleL = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSampleR*dram->highpass[hilp_a0])+dram->highpass[hilp_aR1];
			dram->highpass[hilp_aR1] = (inputSampleR*dram->highpass[hilp_a1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b1])+dram->highpass[hilp_aR2];
			dram->highpass[hilp_aR2] = (inputSampleR*dram->highpass[hilp_a0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b2]); inputSampleR = dram->highpass[hilp_temp];
		} else dram->highpass[hilp_aR1] = dram->highpass[hilp_aR2] = dram->highpass[hilp_aL1] = dram->highpass[hilp_aL2] = 0.0;
		if (lowpassEngage) { //distributed Lowpass
			dram->lowpass[hilp_temp] = (inputSampleL*dram->lowpass[hilp_a0])+dram->lowpass[hilp_aL1];
			dram->lowpass[hilp_aL1] = (inputSampleL*dram->lowpass[hilp_a1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b1])+dram->lowpass[hilp_aL2];
			dram->lowpass[hilp_aL2] = (inputSampleL*dram->lowpass[hilp_a0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b2]); inputSampleL = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSampleR*dram->lowpass[hilp_a0])+dram->lowpass[hilp_aR1];
			dram->lowpass[hilp_aR1] = (inputSampleR*dram->lowpass[hilp_a1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b1])+dram->lowpass[hilp_aR2];
			dram->lowpass[hilp_aR2] = (inputSampleR*dram->lowpass[hilp_a0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b2]); inputSampleR = dram->lowpass[hilp_temp];
		} else dram->lowpass[hilp_aR1] = dram->lowpass[hilp_aR2] = dram->lowpass[hilp_aL1] = dram->lowpass[hilp_aL2] = 0.0;
		//first Highpass/Lowpass blocks aliasing before the nonlinearity of Parametric
		
		//get all Parametric bands before any other processing is done
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->high[biqs_outL] = inputSampleL * fabs(dram->high[biqs_level]);
		dram->high[biqs_dis] = fabs(dram->high[biqs_a0] * (1.0+(dram->high[biqs_outL]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0) dram->high[biqs_dis] = 1.0;
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_dis]) + dram->high[biqs_aL1];
		dram->high[biqs_aL1] = dram->high[biqs_aL2] - (dram->high[biqs_temp]*dram->high[biqs_b1]);
		dram->high[biqs_aL2] = (dram->high[biqs_outL] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_b2]);
		dram->high[biqs_outL] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_c0] * (1.0+(dram->high[biqs_outL]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0) dram->high[biqs_dis] = 1.0;
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_dis]) + dram->high[biqs_cL1];
		dram->high[biqs_cL1] = dram->high[biqs_cL2] - (dram->high[biqs_temp]*dram->high[biqs_d1]);
		dram->high[biqs_cL2] = (dram->high[biqs_outL] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_d2]);
		dram->high[biqs_outL] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_e0] * (1.0+(dram->high[biqs_outL]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0) dram->high[biqs_dis] = 1.0;
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_dis]) + dram->high[biqs_eL1];
		dram->high[biqs_eL1] = dram->high[biqs_eL2] - (dram->high[biqs_temp]*dram->high[biqs_f1]);
		dram->high[biqs_eL2] = (dram->high[biqs_outL] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_f2]);
		dram->high[biqs_outL] = dram->high[biqs_temp]; dram->high[biqs_outL] *= dram->high[biqs_level];
		if (dram->high[biqs_level] > 1.0) dram->high[biqs_outL] *= dram->high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->hmid[biqs_outL] = inputSampleL * fabs(dram->hmid[biqs_level]);
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_a0] * (1.0+(dram->hmid[biqs_outL]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0) dram->hmid[biqs_dis] = 1.0;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_dis]) + dram->hmid[biqs_aL1];
		dram->hmid[biqs_aL1] = dram->hmid[biqs_aL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_b1]);
		dram->hmid[biqs_aL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_b2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_c0] * (1.0+(dram->hmid[biqs_outL]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0) dram->hmid[biqs_dis] = 1.0;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_dis]) + dram->hmid[biqs_cL1];
		dram->hmid[biqs_cL1] = dram->hmid[biqs_cL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_d1]);
		dram->hmid[biqs_cL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_d2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_e0] * (1.0+(dram->hmid[biqs_outL]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0) dram->hmid[biqs_dis] = 1.0;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_dis]) + dram->hmid[biqs_eL1];
		dram->hmid[biqs_eL1] = dram->hmid[biqs_eL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_f1]);
		dram->hmid[biqs_eL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_f2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp]; dram->hmid[biqs_outL] *= dram->hmid[biqs_level];
		if (dram->hmid[biqs_level] > 1.0) dram->hmid[biqs_outL] *= dram->hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->lmid[biqs_outL] = inputSampleL * fabs(dram->lmid[biqs_level]);
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_a0] * (1.0+(dram->lmid[biqs_outL]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0) dram->lmid[biqs_dis] = 1.0;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_dis]) + dram->lmid[biqs_aL1];
		dram->lmid[biqs_aL1] = dram->lmid[biqs_aL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_b1]);
		dram->lmid[biqs_aL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_b2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_c0] * (1.0+(dram->lmid[biqs_outL]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0) dram->lmid[biqs_dis] = 1.0;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_dis]) + dram->lmid[biqs_cL1];
		dram->lmid[biqs_cL1] = dram->lmid[biqs_cL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_d1]);
		dram->lmid[biqs_cL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_d2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_e0] * (1.0+(dram->lmid[biqs_outL]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0) dram->lmid[biqs_dis] = 1.0;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_dis]) + dram->lmid[biqs_eL1];
		dram->lmid[biqs_eL1] = dram->lmid[biqs_eL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_f1]);
		dram->lmid[biqs_eL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_f2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp]; dram->lmid[biqs_outL] *= dram->lmid[biqs_level];
		if (dram->lmid[biqs_level] > 1.0) dram->lmid[biqs_outL] *= dram->lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->bass[biqs_outL] = inputSampleL * fabs(dram->bass[biqs_level]);
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_a0] * (1.0+(dram->bass[biqs_outL]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0) dram->bass[biqs_dis] = 1.0;
		dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_dis]) + dram->bass[biqs_aL1];
		dram->bass[biqs_aL1] = dram->bass[biqs_aL2] - (dram->bass[biqs_temp]*dram->bass[biqs_b1]);
		dram->bass[biqs_aL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_b2]);
		dram->bass[biqs_outL] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_c0] * (1.0+(dram->bass[biqs_outL]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0) dram->bass[biqs_dis] = 1.0;
		dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_dis]) + dram->bass[biqs_cL1];
		dram->bass[biqs_cL1] = dram->bass[biqs_cL2] - (dram->bass[biqs_temp]*dram->bass[biqs_d1]);
		dram->bass[biqs_cL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_d2]);
		dram->bass[biqs_outL] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_e0] * (1.0+(dram->bass[biqs_outL]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0) dram->bass[biqs_dis] = 1.0;
		dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_dis]) + dram->bass[biqs_eL1];
		dram->bass[biqs_eL1] = dram->bass[biqs_eL2] - (dram->bass[biqs_temp]*dram->bass[biqs_f1]);
		dram->bass[biqs_eL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_f2]);
		dram->bass[biqs_outL] = dram->bass[biqs_temp]; dram->bass[biqs_outL] *= dram->bass[biqs_level];
		if (dram->bass[biqs_level] > 1.0) dram->bass[biqs_outL] *= dram->bass[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->high[biqs_outR] = inputSampleR * fabs(dram->high[biqs_level]);
		dram->high[biqs_dis] = fabs(dram->high[biqs_a0] * (1.0+(dram->high[biqs_outR]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0) dram->high[biqs_dis] = 1.0;
		dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_dis]) + dram->high[biqs_aR1];
		dram->high[biqs_aR1] = dram->high[biqs_aR2] - (dram->high[biqs_temp]*dram->high[biqs_b1]);
		dram->high[biqs_aR2] = (dram->high[biqs_outR] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_b2]);
		dram->high[biqs_outR] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_c0] * (1.0+(dram->high[biqs_outR]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0) dram->high[biqs_dis] = 1.0;
		dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_dis]) + dram->high[biqs_cR1];
		dram->high[biqs_cR1] = dram->high[biqs_cR2] - (dram->high[biqs_temp]*dram->high[biqs_d1]);
		dram->high[biqs_cR2] = (dram->high[biqs_outR] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_d2]);
		dram->high[biqs_outR] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_e0] * (1.0+(dram->high[biqs_outR]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0) dram->high[biqs_dis] = 1.0;
		dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_dis]) + dram->high[biqs_eR1];
		dram->high[biqs_eR1] = dram->high[biqs_eR2] - (dram->high[biqs_temp]*dram->high[biqs_f1]);
		dram->high[biqs_eR2] = (dram->high[biqs_outR] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_f2]);
		dram->high[biqs_outR] = dram->high[biqs_temp]; dram->high[biqs_outR] *= dram->high[biqs_level];
		if (dram->high[biqs_level] > 1.0) dram->high[biqs_outR] *= dram->high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->hmid[biqs_outR] = inputSampleR * fabs(dram->hmid[biqs_level]);
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_a0] * (1.0+(dram->hmid[biqs_outR]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0) dram->hmid[biqs_dis] = 1.0;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_dis]) + dram->hmid[biqs_aR1];
		dram->hmid[biqs_aR1] = dram->hmid[biqs_aR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_b1]);
		dram->hmid[biqs_aR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_b2]);
		dram->hmid[biqs_outR] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_c0] * (1.0+(dram->hmid[biqs_outR]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0) dram->hmid[biqs_dis] = 1.0;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_dis]) + dram->hmid[biqs_cR1];
		dram->hmid[biqs_cR1] = dram->hmid[biqs_cR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_d1]);
		dram->hmid[biqs_cR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_d2]);
		dram->hmid[biqs_outR] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_e0] * (1.0+(dram->hmid[biqs_outR]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0) dram->hmid[biqs_dis] = 1.0;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_dis]) + dram->hmid[biqs_eR1];
		dram->hmid[biqs_eR1] = dram->hmid[biqs_eR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_f1]);
		dram->hmid[biqs_eR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_f2]);
		dram->hmid[biqs_outR] = dram->hmid[biqs_temp]; dram->hmid[biqs_outR] *= dram->hmid[biqs_level];
		if (dram->hmid[biqs_level] > 1.0) dram->hmid[biqs_outR] *= dram->hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->lmid[biqs_outR] = inputSampleR * fabs(dram->lmid[biqs_level]);
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_a0] * (1.0+(dram->lmid[biqs_outR]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0) dram->lmid[biqs_dis] = 1.0;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_dis]) + dram->lmid[biqs_aR1];
		dram->lmid[biqs_aR1] = dram->lmid[biqs_aR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_b1]);
		dram->lmid[biqs_aR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_b2]);
		dram->lmid[biqs_outR] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_c0] * (1.0+(dram->lmid[biqs_outR]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0) dram->lmid[biqs_dis] = 1.0;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_dis]) + dram->lmid[biqs_cR1];
		dram->lmid[biqs_cR1] = dram->lmid[biqs_cR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_d1]);
		dram->lmid[biqs_cR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_d2]);
		dram->lmid[biqs_outR] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_e0] * (1.0+(dram->lmid[biqs_outR]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0) dram->lmid[biqs_dis] = 1.0;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_dis]) + dram->lmid[biqs_eR1];
		dram->lmid[biqs_eR1] = dram->lmid[biqs_eR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_f1]);
		dram->lmid[biqs_eR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_f2]);
		dram->lmid[biqs_outR] = dram->lmid[biqs_temp]; dram->lmid[biqs_outR] *= dram->lmid[biqs_level];
		if (dram->lmid[biqs_level] > 1.0) dram->lmid[biqs_outR] *= dram->lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->bass[biqs_outR] = inputSampleR * fabs(dram->bass[biqs_level]);
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_a0] * (1.0+(dram->bass[biqs_outR]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0) dram->bass[biqs_dis] = 1.0;
		dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_dis]) + dram->bass[biqs_aR1];
		dram->bass[biqs_aR1] = dram->bass[biqs_aR2] - (dram->bass[biqs_temp]*dram->bass[biqs_b1]);
		dram->bass[biqs_aR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_b2]);
		dram->bass[biqs_outR] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_c0] * (1.0+(dram->bass[biqs_outR]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0) dram->bass[biqs_dis] = 1.0;
		dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_dis]) + dram->bass[biqs_cR1];
		dram->bass[biqs_cR1] = dram->bass[biqs_cR2] - (dram->bass[biqs_temp]*dram->bass[biqs_d1]);
		dram->bass[biqs_cR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_d2]);
		dram->bass[biqs_outR] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_e0] * (1.0+(dram->bass[biqs_outR]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0) dram->bass[biqs_dis] = 1.0;
		dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_dis]) + dram->bass[biqs_eR1];
		dram->bass[biqs_eR1] = dram->bass[biqs_eR2] - (dram->bass[biqs_temp]*dram->bass[biqs_f1]);
		dram->bass[biqs_eR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_f2]);
		dram->bass[biqs_outR] = dram->bass[biqs_temp]; dram->bass[biqs_outR] *= dram->bass[biqs_level];
		if (dram->bass[biqs_level] > 1.0) dram->bass[biqs_outR] *= dram->bass[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		double temp = (double)nSampleFrames/inFramesToProcess;		
		double gainR = (panA*temp)+(panB*(1.0-temp));
		double gainL = 1.57079633-gainR;
		gainR = sin(gainR); gainL = sin(gainL);
		double gain = (inTrimA*temp)+(inTrimB*(1.0-temp));
		if (gain > 1.0) gain *= gain; else gain = 1.0-pow(1.0-gain,2);
		gain *= 0.763932022500211;
		double airGain = (airGainA*temp)+(airGainB*(1.0-temp));
		if (airGain > 1.0) airGain *= airGain; else airGain = 1.0-pow(1.0-airGain,2);
		double fireGain = (fireGainA*temp)+(fireGainB*(1.0-temp));
		if (fireGain > 1.0) fireGain *= fireGain; else fireGain = 1.0-pow(1.0-fireGain,2);
		double firePad = fireGain; if (firePad > 1.0) firePad = 1.0;
		double stoneGain = (stoneGainA*temp)+(stoneGainB*(1.0-temp));
		if (stoneGain > 1.0) stoneGain *= stoneGain; else stoneGain = 1.0-pow(1.0-stoneGain,2);
		double stonePad = stoneGain; if (stonePad > 1.0) stonePad = 1.0;
		//set up smoothed gain controls
		
		//begin Air3L
		double drySampleL = inputSampleL;
		dram->air[pvSL4] = dram->air[pvAL4] - dram->air[pvAL3]; dram->air[pvSL3] = dram->air[pvAL3] - dram->air[pvAL2];
		dram->air[pvSL2] = dram->air[pvAL2] - dram->air[pvAL1]; dram->air[pvSL1] = dram->air[pvAL1] - inputSampleL;
		dram->air[accSL3] = dram->air[pvSL4] - dram->air[pvSL3]; dram->air[accSL2] = dram->air[pvSL3] - dram->air[pvSL2];
		dram->air[accSL1] = dram->air[pvSL2] - dram->air[pvSL1];
		dram->air[acc2SL2] = dram->air[accSL3] - dram->air[accSL2]; dram->air[acc2SL1] = dram->air[accSL2] - dram->air[accSL1];		
		dram->air[outAL] = -(dram->air[pvAL1] + dram->air[pvSL3] + dram->air[acc2SL2] - ((dram->air[acc2SL2] + dram->air[acc2SL1])*0.5));
		dram->air[gainAL] *= 0.5; dram->air[gainAL] += fabs(drySampleL-dram->air[outAL])*0.5;
		if (dram->air[gainAL] > 0.3*sqrt(overallscale)) dram->air[gainAL] = 0.3*sqrt(overallscale);
		dram->air[pvAL4] = dram->air[pvAL3]; dram->air[pvAL3] = dram->air[pvAL2];
		dram->air[pvAL2] = dram->air[pvAL1]; dram->air[pvAL1] = (dram->air[gainAL] * dram->air[outAL]) + drySampleL;
		double fireL = drySampleL - ((dram->air[outAL]*0.5)+(drySampleL*(0.457-(0.017*overallscale))));
		temp = (fireL + dram->air[gndavgL])*0.5; dram->air[gndavgL] = fireL; fireL = temp;
		double airL = (drySampleL-fireL)*airGain;
		inputSampleL = fireL;
		//end Air3L
		//begin Air3R
		double drySampleR = inputSampleR;
		dram->air[pvSR4] = dram->air[pvAR4] - dram->air[pvAR3]; dram->air[pvSR3] = dram->air[pvAR3] - dram->air[pvAR2];
		dram->air[pvSR2] = dram->air[pvAR2] - dram->air[pvAR1]; dram->air[pvSR1] = dram->air[pvAR1] - inputSampleR;
		dram->air[accSR3] = dram->air[pvSR4] - dram->air[pvSR3]; dram->air[accSR2] = dram->air[pvSR3] - dram->air[pvSR2];
		dram->air[accSR1] = dram->air[pvSR2] - dram->air[pvSR1];
		dram->air[acc2SR2] = dram->air[accSR3] - dram->air[accSR2]; dram->air[acc2SR1] = dram->air[accSR2] - dram->air[accSR1];		
		dram->air[outAR] = -(dram->air[pvAR1] + dram->air[pvSR3] + dram->air[acc2SR2] - ((dram->air[acc2SR2] + dram->air[acc2SR1])*0.5));
		dram->air[gainAR] *= 0.5; dram->air[gainAR] += fabs(drySampleR-dram->air[outAR])*0.5;
		if (dram->air[gainAR] > 0.3*sqrt(overallscale)) dram->air[gainAR] = 0.3*sqrt(overallscale);
		dram->air[pvAR4] = dram->air[pvAR3]; dram->air[pvAR3] = dram->air[pvAR2];
		dram->air[pvAR2] = dram->air[pvAR1]; dram->air[pvAR1] = (dram->air[gainAR] * dram->air[outAR]) + drySampleR;
		double fireR = drySampleR - ((dram->air[outAR]*0.5)+(drySampleR*(0.457-(0.017*overallscale))));
		temp = (fireR + dram->air[gndavgR])*0.5; dram->air[gndavgR] = fireR; fireR = temp;
		double airR = (drySampleR-fireR)*airGain;
		inputSampleR = fireR;
		//end Air3R
		//begin KalmanL
		temp = inputSampleL = inputSampleL*(1.0-kalmanRange)*0.777;
		inputSampleL *= (1.0-kalmanRange);
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
		dram->kal[kalGainL] += fabs(temp-dram->kal[kalOutL])*kalmanRange*8.0; dram->kal[kalGainL] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainL] > kalmanRange*0.5) dram->kal[kalGainL] = kalmanRange*0.5;
		//attempts to avoid explosions
		dram->kal[kalOutL] += (temp*(1.0-(0.68+(kalmanRange*0.157))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampL3] = dram->kal[prevSampL2]; dram->kal[prevSampL2] = dram->kal[prevSampL1];
		dram->kal[prevSampL1] = (dram->kal[kalGainL] * dram->kal[kalOutL]) + ((1.0-dram->kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampL1] > 1.0) dram->kal[prevSampL1] = 1.0; if (dram->kal[prevSampL1] < -1.0) dram->kal[prevSampL1] = -1.0;
		double stoneL = dram->kal[kalOutL]*0.777;
		fireL -= stoneL;
		//end KalmanL
		//begin KalmanR
		temp = inputSampleR = inputSampleR*(1.0-kalmanRange)*0.777;
		inputSampleR *= (1.0-kalmanRange);
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
		dram->kal[kalGainR] += fabs(temp-dram->kal[kalOutR])*kalmanRange*8.0; dram->kal[kalGainR] *= 0.5;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainR] > kalmanRange*0.5) dram->kal[kalGainR] = kalmanRange*0.5;
		//attempts to avoid explosions
		dram->kal[kalOutR] += (temp*(1.0-(0.68+(kalmanRange*0.157))));	
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
			dram->high[biqs_outL] *= temp;
			dram->high[biqs_outR] *= temp;
			dram->hmid[biqs_outL] *= temp; //if Fire gating, gate Air, high and hmid
			dram->hmid[biqs_outR] *= temp; //note that we aren't compressing these
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
			dram->lmid[biqs_outL] *= temp;
			dram->lmid[biqs_outR] *= temp;
			dram->bass[biqs_outL] *= temp; //if Stone gating, gate lmid and bass
			dram->bass[biqs_outR] *= temp; //note that we aren't compressing these
		}
		inputSampleL = stoneL + fireL + airL;
		inputSampleR = stoneR + fireR + airR;
		//create Stonefire output
		
		if (highpassEngage) { //distributed Highpass
			dram->highpass[hilp_temp] = (inputSampleL*dram->highpass[hilp_c0])+dram->highpass[hilp_cL1];
			dram->highpass[hilp_cL1] = (inputSampleL*dram->highpass[hilp_c1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d1])+dram->highpass[hilp_cL2];
			dram->highpass[hilp_cL2] = (inputSampleL*dram->highpass[hilp_c0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d2]); inputSampleL = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSampleR*dram->highpass[hilp_c0])+dram->highpass[hilp_cR1];
			dram->highpass[hilp_cR1] = (inputSampleR*dram->highpass[hilp_c1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d1])+dram->highpass[hilp_cR2];
			dram->highpass[hilp_cR2] = (inputSampleR*dram->highpass[hilp_c0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d2]); inputSampleR = dram->highpass[hilp_temp];
		} else dram->highpass[hilp_cR1] = dram->highpass[hilp_cR2] = dram->highpass[hilp_cL1] = dram->highpass[hilp_cL2] = 0.0;
		if (lowpassEngage) { //distributed Lowpass
			dram->lowpass[hilp_temp] = (inputSampleL*dram->lowpass[hilp_c0])+dram->lowpass[hilp_cL1];
			dram->lowpass[hilp_cL1] = (inputSampleL*dram->lowpass[hilp_c1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d1])+dram->lowpass[hilp_cL2];
			dram->lowpass[hilp_cL2] = (inputSampleL*dram->lowpass[hilp_c0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d2]); inputSampleL = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSampleR*dram->lowpass[hilp_c0])+dram->lowpass[hilp_cR1];
			dram->lowpass[hilp_cR1] = (inputSampleR*dram->lowpass[hilp_c1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d1])+dram->lowpass[hilp_cR2];
			dram->lowpass[hilp_cR2] = (inputSampleR*dram->lowpass[hilp_c0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d2]); inputSampleR = dram->lowpass[hilp_temp];
		} else dram->lowpass[hilp_cR1] = dram->lowpass[hilp_cR2] = dram->lowpass[hilp_cL1] = dram->lowpass[hilp_cL2] = 0.0;
		//another stage of Highpass/Lowpass before bringing in the parametric bands
		
		inputSampleL += (dram->high[biqs_outL] + dram->hmid[biqs_outL] + dram->lmid[biqs_outL] + dram->bass[biqs_outL]);
		inputSampleR += (dram->high[biqs_outR] + dram->hmid[biqs_outR] + dram->lmid[biqs_outR] + dram->bass[biqs_outR]);
		//add parametric boosts or cuts: clean as possible for maximal rawness and sonority
		
		inputSampleL = inputSampleL * gainL * gain;
		inputSampleR = inputSampleR * gainR * gain;
		//applies pan section, and smoothed fader gain
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222) inputSampleL = -0.222; if (inputSampleL > 0.222) inputSampleL = 0.222;
		dram->dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
		int dBdly = floor(dBaPosL*dscBuf);
		double dBi = (dBaPosL*dscBuf)-dBdly;
		inputSampleL = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleL += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		inputSampleL /= topdB;		
		inputSampleR *= topdB;
		if (inputSampleR < -0.222) inputSampleR = -0.222; if (inputSampleR > 0.222) inputSampleR = 0.222;
		dram->dBaR[dBaXR] = inputSampleR; dBaPosR *= 0.5; dBaPosR += fabs((inputSampleR*((inputSampleR*0.25)-0.5))*0.5);
		dBdly = floor(dBaPosR*dscBuf);
		dBi = (dBaPosR*dscBuf)-dBdly;
		inputSampleR = dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleR += dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
		inputSampleR /= topdB;		
		//top dB processing for distributed discontinuity modeling air nonlinearity
		
		//ConsoleXChannel before final Highpass/Lowpass stages
		inputSampleL *= 0.618033988749895;
		if (inputSampleL > 1.0) inputSampleL = 1.0;
		else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895));
		if (inputSampleL < -1.0) inputSampleL = -1.0;
		else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895));
		inputSampleR *= 0.618033988749895;
		if (inputSampleR > 1.0) inputSampleR = 1.0;
		else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895));
		if (inputSampleR < -1.0) inputSampleR = -1.0;
		else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895));
		//ConsoleXChannel before final Highpass/Lowpass stages
		
		if (highpassEngage) { //distributed Highpass
			dram->highpass[hilp_temp] = (inputSampleL*dram->highpass[hilp_e0])+dram->highpass[hilp_eL1];
			dram->highpass[hilp_eL1] = (inputSampleL*dram->highpass[hilp_e1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f1])+dram->highpass[hilp_eL2];
			dram->highpass[hilp_eL2] = (inputSampleL*dram->highpass[hilp_e0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f2]); inputSampleL = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSampleR*dram->highpass[hilp_e0])+dram->highpass[hilp_eR1];
			dram->highpass[hilp_eR1] = (inputSampleR*dram->highpass[hilp_e1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f1])+dram->highpass[hilp_eR2];
			dram->highpass[hilp_eR2] = (inputSampleR*dram->highpass[hilp_e0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f2]); inputSampleR = dram->highpass[hilp_temp];
		} else dram->highpass[hilp_eR1] = dram->highpass[hilp_eR2] = dram->highpass[hilp_eL1] = dram->highpass[hilp_eL2] = 0.0;
		if (lowpassEngage) { //distributed Lowpass
			dram->lowpass[hilp_temp] = (inputSampleL*dram->lowpass[hilp_e0])+dram->lowpass[hilp_eL1];
			dram->lowpass[hilp_eL1] = (inputSampleL*dram->lowpass[hilp_e1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f1])+dram->lowpass[hilp_eL2];
			dram->lowpass[hilp_eL2] = (inputSampleL*dram->lowpass[hilp_e0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f2]); inputSampleL = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSampleR*dram->lowpass[hilp_e0])+dram->lowpass[hilp_eR1];
			dram->lowpass[hilp_eR1] = (inputSampleR*dram->lowpass[hilp_e1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f1])+dram->lowpass[hilp_eR2];
			dram->lowpass[hilp_eR2] = (inputSampleR*dram->lowpass[hilp_e0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f2]); inputSampleR = dram->lowpass[hilp_temp];
		} else dram->lowpass[hilp_eR1] = dram->lowpass[hilp_eR2] = dram->lowpass[hilp_eL1] = dram->lowpass[hilp_eL2] = 0.0;		
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
		dram->highpass[x] = 0.0;
		dram->lowpass[x] = 0.0;
	}
	
	for (int x = 0; x < air_total; x++) dram->air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) dram->kal[x] = 0.0;
	fireCompL = 1.0;
	fireCompR = 1.0;
	fireGate = 1.0;
	stoneCompL = 1.0;
	stoneCompR = 1.0;
	stoneGate = 1.0;	

	for (int x = 0; x < biqs_total; x++) {
		dram->high[x] = 0.0;
		dram->hmid[x] = 0.0;
		dram->lmid[x] = 0.0;
		dram->bass[x] = 0.0;
	}

	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
		dram->dBaR[count] = 0.0;
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
