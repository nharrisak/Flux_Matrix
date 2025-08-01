#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "kBeyond"
#define AIRWINDOWS_DESCRIPTION "A recital hall."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','k','B','e' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
const int predelay = 15000;
const int d3A = 250; const int d3B = 1174; const int d3C = 305; const int d3D = 1267; const int d3E = 1240; const int d3F = 1387; const int d3G = 1461; const int d3H = 1448; const int d3I = 1564; //39 to 101 ms, 587 seat theater  
const int d6A = 2; const int d6B = 3; const int d6C = 536; const int d6D = 190; const int d6E = 504; const int d6F = 609; const int d6G = 8; const int d6H = 574; const int d6I = 558; const int d6J = 516; const int d6K = 12; const int d6L = 13; const int d6M = 488; const int d6N = 560; const int d6O = 416; const int d6P = 585; const int d6Q = 18; const int d6R = 19; const int d6S = 495; const int d6T = 21; const int d6U = 30; const int d6V = 578; const int d6W = 104; const int d6X = 487; const int d6Y = 157; const int d6ZA = 27; const int d6ZB = 280; const int d6ZC = 360; const int d6ZD = 452; const int d6ZE = 31; const int d6ZF = 537; const int d6ZG = 291; const int d6ZH = 34; const int d6ZI = 483; const int d6ZJ = 28; const int d6ZK = 542; //2 to 75 ms, 181 seat club  
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Derez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "EarlyRf", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Predlay", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	int c3AL,c3AR,c3BL,c3BR,c3CL,c3CR,c3DL,c3DR,c3EL,c3ER;
	int c3FL,c3FR,c3GL,c3GR,c3HL,c3HR,c3IL,c3IR;

	int c6AL,c6BL,c6CL,c6DL,c6EL,c6FL,c6GL,c6HL,c6IL;
	int c6JL,c6KL,c6LL,c6ML,c6NL,c6OL,c6PL,c6QL,c6RL;
	int c6SL,c6TL,c6UL,c6VL,c6WL,c6XL,c6YL,c6ZAL,c6ZBL;	
	int c6ZCL,c6ZDL,c6ZEL,c6ZFL,c6ZGL,c6ZHL,c6ZIL,c6ZJL,c6ZKL;		
	int c6AR,c6BR,c6CR,c6DR,c6ER,c6FR,c6GR,c6HR,c6IR;
	int c6JR,c6KR,c6LR,c6MR,c6NR,c6OR,c6PR,c6QR,c6RR;
	int c6SR,c6TR,c6UR,c6VR,c6WR,c6XR,c6YR,c6ZAR,c6ZBR;	
	int c6ZCR,c6ZDR,c6ZER,c6ZFR,c6ZGR,c6ZHR,c6ZIR,c6ZJR,c6ZKR;
	float f6AL,f6BL,f6CL,f6DL,f6EL,f6FL;
	float f6FR,f6LR,f6RR,f6XR,f6ZER,f6ZKR;
	float avg6L,avg6R; 
	
	int countZ;

	enum {
		bez_AL,
		bez_AR,
		bez_BL,
		bez_BR,
		bez_CL,
		bez_CR,	
		bez_InL,
		bez_InR,
		bez_UnInL,
		bez_UnInR,
		bez_SampL,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	
	int firPosition;
			
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float a3AL[d3A+5];
	float a3BL[d3B+5];
	float a3CL[d3C+5];
	float a3DL[d3D+5];
	float a3EL[d3E+5];
	float a3FL[d3F+5];
	float a3GL[d3G+5];
	float a3HL[d3H+5];
	float a3IL[d3I+5];
	float a3AR[d3A+5];
	float a3BR[d3B+5];
	float a3CR[d3C+5];
	float a3DR[d3D+5];
	float a3ER[d3E+5];
	float a3FR[d3F+5];
	float a3GR[d3G+5];
	float a3HR[d3H+5];
	float a3IR[d3I+5];
	float a6AL[d6A+5];
	float a6BL[d6B+5];
	float a6CL[d6C+5];
	float a6DL[d6D+5];
	float a6EL[d6E+5];
	float a6FL[d6F+5];
	float a6GL[d6G+5];
	float a6HL[d6H+5];
	float a6IL[d6I+5];
	float a6JL[d6J+5];
	float a6KL[d6K+5];
	float a6LL[d6L+5];
	float a6ML[d6M+5];
	float a6NL[d6N+5];
	float a6OL[d6O+5];
	float a6PL[d6P+5];
	float a6QL[d6Q+5];
	float a6RL[d6R+5];
	float a6SL[d6S+5];
	float a6TL[d6T+5];
	float a6UL[d6U+5];
	float a6VL[d6V+5];
	float a6WL[d6W+5];
	float a6XL[d6X+5];
	float a6YL[d6Y+5];
	float a6ZAL[d6ZA+5];
	float a6ZBL[d6ZB+5];
	float a6ZCL[d6ZC+5];
	float a6ZDL[d6ZD+5];
	float a6ZEL[d6ZE+5];
	float a6ZFL[d6ZF+5];
	float a6ZGL[d6ZG+5];
	float a6ZHL[d6ZH+5];
	float a6ZIL[d6ZI+5];
	float a6ZJL[d6ZJ+5];
	float a6ZKL[d6ZK+5];
	float a6AR[d6A+5];
	float a6BR[d6B+5];
	float a6CR[d6C+5];
	float a6DR[d6D+5];
	float a6ER[d6E+5];
	float a6FR[d6F+5];
	float a6GR[d6G+5];
	float a6HR[d6H+5];
	float a6IR[d6I+5];
	float a6JR[d6J+5];
	float a6KR[d6K+5];
	float a6LR[d6L+5];
	float a6MR[d6M+5];
	float a6NR[d6N+5];
	float a6OR[d6O+5];
	float a6PR[d6P+5];
	float a6QR[d6Q+5];
	float a6RR[d6R+5];
	float a6SR[d6S+5];
	float a6TR[d6T+5];
	float a6UR[d6U+5];
	float a6VR[d6V+5];
	float a6WR[d6W+5];
	float a6XR[d6X+5];
	float a6YR[d6Y+5];
	float a6ZAR[d6ZA+5];
	float a6ZBR[d6ZB+5];
	float a6ZCR[d6ZC+5];
	float a6ZDR[d6ZD+5];
	float a6ZER[d6ZE+5];
	float a6ZFR[d6ZF+5];
	float a6ZGR[d6ZG+5];
	float a6ZHR[d6ZH+5];
	float a6ZIR[d6ZI+5];
	float a6ZJR[d6ZJ+5];
	float a6ZKR[d6ZK+5];
	float aZL[predelay+5];
	float aZR[predelay+5];
	float bez[bez_total];
	float firBufferL[32768];
	float firBufferR[32768];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float fdb6ck = (0.0009765625f+0.0009765625f+0.001953125f)*0.3333333f;
	float reg6n = (1.0f-powf(1.0f-GetParameter( kParam_A ),3.0f))*fdb6ck;
	//start this but pad it in the loop by volume of output?
	
	float derez = GetParameter( kParam_B )/overallscale;
	derez = 1.0f / ((int)(1.0f/derez));
	if (derez < 0.0005f) derez = 0.0005f; if (derez > 1.0f) derez = 1.0f;
	float freq = GetParameter( kParam_C )*M_PI_2; if (freq < 0.5f) freq = 0.5f;
	float earlyLoudness = GetParameter( kParam_D );
	int adjPredelay = predelay*GetParameter( kParam_E )*derez;	
	float wet = GetParameter( kParam_F );
	
	float fir[50]; fir[24] = 1.0f;
	for(int fip = 0; fip < 24; fip++) {
		fir[fip] = (fip-24)*freq;
		fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
	}
	for(int fip = 25; fip < 48; fip++) {
		fir[fip] = (fip-24)*freq;
		fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
	} //setting up the filter which will run inside DeRez
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0f;
			
			//predelay
			dram->aZL[countZ] = dram->bez[bez_SampL];
			dram->aZR[countZ] = dram->bez[bez_SampR];
			countZ++; if (countZ < 0 || countZ > adjPredelay) countZ = 0;
			dram->bez[bez_SampL] = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			dram->bez[bez_SampR] = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay
			
			inputSampleL = dram->bez[bez_SampL];
			inputSampleR = dram->bez[bez_SampR];
			
			dram->a3AL[c3AL] = inputSampleL;
			dram->a3BL[c3BL] = inputSampleL;
			dram->a3CL[c3CL] = inputSampleL;
			
			dram->a3CR[c3CR] = inputSampleR;
			dram->a3FR[c3FR] = inputSampleR;
			dram->a3IR[c3IR] = inputSampleR;
			
			c3AL++; if (c3AL < 0 || c3AL > d3A) c3AL = 0;
			c3BL++; if (c3BL < 0 || c3BL > d3B) c3BL = 0;
			c3CL++; if (c3CL < 0 || c3CL > d3C) c3CL = 0;
			c3CR++; if (c3CR < 0 || c3CR > d3C) c3CR = 0;
			c3FR++; if (c3FR < 0 || c3FR > d3F) c3FR = 0;
			c3IR++; if (c3IR < 0 || c3IR > d3I) c3IR = 0;
			
			float o3AL = dram->a3AL[c3AL-((c3AL > d3A)?c3AL+1:0)];
			float o3BL = dram->a3BL[c3BL-((c3BL > d3B)?c3BL+1:0)];
			float o3CL = dram->a3CL[c3CL-((c3CL > d3C)?c3CL+1:0)];
			float o3CR = dram->a3CR[c3CR-((c3CR > d3C)?c3CR+1:0)];
			float o3FR = dram->a3FR[c3FR-((c3FR > d3F)?c3FR+1:0)];
			float o3IR = dram->a3IR[c3IR-((c3IR > d3I)?c3IR+1:0)];
			
			dram->a3DL[c3DL] = ((o3BL + o3CL) - o3AL);
			dram->a3EL[c3EL] = ((o3AL + o3CL) - o3BL);
			dram->a3FL[c3FL] = ((o3AL + o3BL) - o3CL);
			dram->a3BR[c3BR] = ((o3FR + o3IR) - o3CR);
			dram->a3ER[c3ER] = ((o3CR + o3IR) - o3FR);
			dram->a3HR[c3HR] = ((o3CR + o3FR) - o3IR);
			
			c3DL++; if (c3DL < 0 || c3DL > d3D) c3DL = 0;
			c3EL++; if (c3EL < 0 || c3EL > d3E) c3EL = 0;
			c3FL++; if (c3FL < 0 || c3FL > d3F) c3FL = 0;
			c3BR++; if (c3BR < 0 || c3BR > d3B) c3BR = 0;
			c3ER++; if (c3ER < 0 || c3ER > d3E) c3ER = 0;
			c3HR++; if (c3HR < 0 || c3HR > d3H) c3HR = 0;
			
			float o3DL = dram->a3DL[c3DL-((c3DL > d3D)?c3DL+1:0)];
			float o3EL = dram->a3EL[c3EL-((c3EL > d3E)?c3EL+1:0)];
			float o3FL = dram->a3FL[c3FL-((c3FL > d3F)?c3FL+1:0)];
			float o3BR = dram->a3BR[c3BR-((c3BR > d3B)?c3BR+1:0)];
			float o3ER = dram->a3ER[c3ER-((c3ER > d3E)?c3ER+1:0)];
			float o3HR = dram->a3HR[c3HR-((c3HR > d3H)?c3HR+1:0)];
			
			dram->a3GL[c3GL] = ((o3EL + o3FL) - o3DL);
			dram->a3HL[c3HL] = ((o3DL + o3FL) - o3EL);
			dram->a3IL[c3IL] = ((o3DL + o3EL) - o3FL);
			dram->a3AR[c3AR] = ((o3ER + o3HR) - o3BR);
			dram->a3DR[c3DR] = ((o3BR + o3HR) - o3ER);
			dram->a3GR[c3GR] = ((o3BR + o3ER) - o3HR);
			
			c3GL++; if (c3GL < 0 || c3GL > d3G) c3GL = 0;
			c3HL++; if (c3HL < 0 || c3HL > d3H) c3HL = 0;
			c3IL++; if (c3IL < 0 || c3IL > d3I) c3IL = 0;
			c3AR++; if (c3AR < 0 || c3AR > d3A) c3AR = 0;
			c3DR++; if (c3DR < 0 || c3DR > d3D) c3DR = 0;
			c3GR++; if (c3GR < 0 || c3GR > d3G) c3GR = 0;
			
			float o3GL = dram->a3GL[c3GL-((c3GL > d3G)?c3GL+1:0)];
			float o3HL = dram->a3HL[c3HL-((c3HL > d3H)?c3HL+1:0)];
			float o3IL = dram->a3IL[c3IL-((c3IL > d3I)?c3IL+1:0)];
			float o3AR = dram->a3AR[c3AR-((c3AR > d3A)?c3AR+1:0)];
			float o3DR = dram->a3DR[c3DR-((c3DR > d3D)?c3DR+1:0)];
			float o3GR = dram->a3GR[c3GR-((c3GR > d3G)?c3GR+1:0)];
			
			float inputSampleL = (o3GL + o3HL + o3IL)*0.125f;
			float inputSampleR = (o3AR + o3DR + o3GR)*0.125f;
			
			float earlyReflectionL = inputSampleL;
			float earlyReflectionR = inputSampleR;
			
			if (firPosition < 0 || firPosition > 32767) firPosition = 32767; int firp = firPosition;		
			dram->firBufferL[firp] = inputSampleL; inputSampleL = 0.0f;
			dram->firBufferR[firp] = inputSampleR; inputSampleR = 0.0f;
			if (firp + 48 < 32767) {
				for(int fip = 1; fip < 48; fip++) {
					inputSampleL += dram->firBufferL[firp+fip] * fir[fip];
					inputSampleR += dram->firBufferR[firp+fip] * fir[fip];
				}
			} else {
				for(int fip = 1; fip < 48; fip++) {
					inputSampleL += dram->firBufferL[firp+fip - ((firp+fip > 32767)?32768:0)] * fir[fip];
					inputSampleR += dram->firBufferR[firp+fip - ((firp+fip > 32767)?32768:0)] * fir[fip];
				}
			}
			inputSampleL *= 0.25f; inputSampleR *= 0.25f;
			inputSampleL *= sqrt(freq); inputSampleR *= sqrt(freq);
			firPosition--;//here's the brickwall FIR filter, running in front of the Householder matrix
			
			dram->a6AL[c6AL] = inputSampleL + (f6BL * reg6n);
			dram->a6BL[c6BL] = inputSampleL + (f6CL * reg6n);
			dram->a6CL[c6CL] = inputSampleL + (f6DL * reg6n);
			dram->a6DL[c6DL] = inputSampleL + (f6EL * reg6n);
			dram->a6EL[c6EL] = inputSampleL + (f6FL * reg6n);
			dram->a6FL[c6FL] = inputSampleL + (f6AL * reg6n);
			
			c6AL++; if (c6AL < 0 || c6AL > d6A) c6AL = 0;
			c6BL++; if (c6BL < 0 || c6BL > d6B) c6BL = 0;
			c6CL++; if (c6CL < 0 || c6CL > d6C) c6CL = 0;
			c6DL++; if (c6DL < 0 || c6DL > d6D) c6DL = 0;
			c6EL++; if (c6EL < 0 || c6EL > d6E) c6EL = 0;
			c6FL++; if (c6FL < 0 || c6FL > d6F) c6FL = 0;
			
			float o6AL = dram->a6AL[c6AL-((c6AL > d6A)?d6A+1:0)];
			float o6BL = dram->a6BL[c6BL-((c6BL > d6B)?d6B+1:0)];
			float o6CL = dram->a6CL[c6CL-((c6CL > d6C)?d6C+1:0)];
			float o6DL = dram->a6DL[c6DL-((c6DL > d6D)?d6D+1:0)];
			float o6EL = dram->a6EL[c6EL-((c6EL > d6E)?d6E+1:0)];
			float o6FL = dram->a6FL[c6FL-((c6FL > d6F)?d6F+1:0)];
			
			dram->a6FR[c6FR] = inputSampleR + (f6LR * reg6n);
			dram->a6LR[c6LR] = inputSampleR + (f6RR * reg6n);
			dram->a6RR[c6RR] = inputSampleR + (f6XR * reg6n);
			dram->a6XR[c6XR] = inputSampleR + (f6ZER * reg6n);
			dram->a6ZER[c6ZER] = inputSampleR + (f6ZKR * reg6n);
			dram->a6ZKR[c6ZKR] = inputSampleR + (f6FR * reg6n);
			
			c6FR++; if (c6FR < 0 || c6FR > d6F) c6FR = 0;
			c6LR++; if (c6LR < 0 || c6LR > d6L) c6LR = 0;
			c6RR++; if (c6RR < 0 || c6RR > d6R) c6RR = 0;
			c6XR++; if (c6XR < 0 || c6XR > d6X) c6XR = 0;
			c6ZER++; if (c6ZER < 0 || c6ZER > d6ZE) c6ZER = 0;
			c6ZKR++; if (c6ZKR < 0 || c6ZKR > d6ZK) c6ZKR = 0;
			
			float o6FR = dram->a6FR[c6FR-((c6FR > d6F)?d6F+1:0)];
			float o6LR = dram->a6LR[c6LR-((c6LR > d6L)?d6L+1:0)];
			float o6RR = dram->a6RR[c6RR-((c6RR > d6R)?d6R+1:0)];
			float o6XR = dram->a6XR[c6XR-((c6XR > d6X)?d6X+1:0)];
			float o6ZER = dram->a6ZER[c6ZER-((c6ZER > d6ZE)?d6ZE+1:0)];
			float o6ZKR = dram->a6ZKR[c6ZKR-((c6ZKR > d6ZK)?d6ZK+1:0)];
			
			//-------- one
			
			dram->a6GL[c6GL] = ((o6AL*2.0f) - (o6BL + o6CL + o6DL + o6EL + o6FL));
			dram->a6HL[c6HL] = ((o6BL*2.0f) - (o6AL + o6CL + o6DL + o6EL + o6FL));
			dram->a6IL[c6IL] = ((o6CL*2.0f) - (o6AL + o6BL + o6DL + o6EL + o6FL));
			dram->a6JL[c6JL] = ((o6DL*2.0f) - (o6AL + o6BL + o6CL + o6EL + o6FL));
			dram->a6KL[c6KL] = ((o6EL*2.0f) - (o6AL + o6BL + o6CL + o6DL + o6FL));
			dram->a6LL[c6LL] = ((o6FL*2.0f) - (o6AL + o6BL + o6CL + o6DL + o6EL));
			
			c6GL++; if (c6GL < 0 || c6GL > d6G) c6GL = 0;
			c6HL++; if (c6HL < 0 || c6HL > d6H) c6HL = 0;
			c6IL++; if (c6IL < 0 || c6IL > d6I) c6IL = 0;
			c6JL++; if (c6JL < 0 || c6JL > d6J) c6JL = 0;
			c6KL++; if (c6KL < 0 || c6KL > d6K) c6KL = 0;
			c6LL++; if (c6LL < 0 || c6LL > d6L) c6LL = 0;
			
			float o6GL = dram->a6GL[c6GL-((c6GL > d6G)?d6G+1:0)];
			float o6HL = dram->a6HL[c6HL-((c6HL > d6H)?d6H+1:0)];
			float o6IL = dram->a6IL[c6IL-((c6IL > d6I)?d6I+1:0)];
			float o6JL = dram->a6JL[c6JL-((c6JL > d6J)?d6J+1:0)];
			float o6KL = dram->a6KL[c6KL-((c6KL > d6K)?d6K+1:0)];
			float o6LL = dram->a6LL[c6LL-((c6LL > d6L)?d6L+1:0)];
			
			dram->a6ER[c6ER] = ((o6FR*2.0f) - (o6LR + o6RR + o6XR + o6ZER + o6ZKR));
			dram->a6KR[c6KR] = ((o6LR*2.0f) - (o6FR + o6RR + o6XR + o6ZER + o6ZKR));
			dram->a6QR[c6QR] = ((o6RR*2.0f) - (o6FR + o6LR + o6XR + o6ZER + o6ZKR));
			dram->a6WR[c6WR] = ((o6XR*2.0f) - (o6FR + o6LR + o6RR + o6ZER + o6ZKR));
			dram->a6ZDR[c6ZDR] = ((o6ZER*2.0f) - (o6FR + o6LR + o6RR + o6XR + o6ZKR));			
			dram->a6ZJR[c6ZJR] = ((o6ZKR*2.0f) - (o6FR + o6LR + o6RR + o6XR + o6ZER));			
			
			c6ER++; if (c6ER < 0 || c6ER > d6E) c6ER = 0;
			c6KR++; if (c6KR < 0 || c6KR > d6K) c6KR = 0;
			c6QR++; if (c6QR < 0 || c6QR > d6Q) c6QR = 0;
			c6WR++; if (c6WR < 0 || c6WR > d6W) c6WR = 0;
			c6ZDR++; if (c6ZDR < 0 || c6ZDR > d6ZD) c6ZDR = 0;
			c6ZJR++; if (c6ZJR < 0 || c6ZJR > d6ZJ) c6ZJR = 0;
			
			float o6ER = dram->a6ER[c6ER-((c6ER > d6E)?d6E+1:0)];
			float o6KR = dram->a6KR[c6KR-((c6KR > d6K)?d6K+1:0)];
			float o6QR = dram->a6QR[c6QR-((c6QR > d6Q)?d6Q+1:0)];
			float o6WR = dram->a6WR[c6WR-((c6WR > d6W)?d6W+1:0)];
			float o6ZDR = dram->a6ZDR[c6ZDR-((c6ZDR > d6ZD)?d6ZD+1:0)];
			float o6ZJR = dram->a6ZJR[c6ZJR-((c6ZJR > d6ZJ)?d6ZJ+1:0)];
			
			//-------- two
			
			dram->a6ML[c6ML] = ((o6GL*2.0f) - (o6HL + o6IL + o6JL + o6KL + o6LL));
			dram->a6NL[c6NL] = ((o6HL*2.0f) - (o6GL + o6IL + o6JL + o6KL + o6LL));
			dram->a6OL[c6OL] = ((o6IL*2.0f) - (o6GL + o6HL + o6JL + o6KL + o6LL));
			dram->a6PL[c6PL] = ((o6JL*2.0f) - (o6GL + o6HL + o6IL + o6KL + o6LL));
			dram->a6QL[c6QL] = ((o6KL*2.0f) - (o6GL + o6HL + o6IL + o6JL + o6LL));
			dram->a6RL[c6RL] = ((o6LL*2.0f) - (o6GL + o6HL + o6IL + o6JL + o6KL));
			
			c6ML++; if (c6ML < 0 || c6ML > d6M) c6ML = 0;
			c6NL++; if (c6NL < 0 || c6NL > d6N) c6NL = 0;
			c6OL++; if (c6OL < 0 || c6OL > d6O) c6OL = 0;
			c6PL++; if (c6PL < 0 || c6PL > d6P) c6PL = 0;
			c6QL++; if (c6QL < 0 || c6QL > d6Q) c6QL = 0;
			c6RL++; if (c6RL < 0 || c6RL > d6R) c6RL = 0;
			
			float o6ML = dram->a6ML[c6ML-((c6ML > d6M)?d6M+1:0)];
			float o6NL = dram->a6NL[c6NL-((c6NL > d6N)?d6N+1:0)];
			float o6OL = dram->a6OL[c6OL-((c6OL > d6O)?d6O+1:0)];
			float o6PL = dram->a6PL[c6PL-((c6PL > d6P)?d6P+1:0)];
			float o6QL = dram->a6QL[c6QL-((c6QL > d6Q)?d6Q+1:0)];
			float o6RL = dram->a6RL[c6RL-((c6RL > d6R)?d6R+1:0)];
			
			dram->a6DR[c6DR] = ((o6ER*2.0f) - (o6KR + o6QR + o6WR + o6ZDR + o6ZJR));
			dram->a6JR[c6JR] = ((o6KR*2.0f) - (o6ER + o6QR + o6WR + o6ZDR + o6ZJR));
			dram->a6PR[c6PR] = ((o6QR*2.0f) - (o6ER + o6KR + o6WR + o6ZDR + o6ZJR));
			dram->a6VR[c6VR] = ((o6WR*2.0f) - (o6ER + o6KR + o6QR + o6ZDR + o6ZJR));
			dram->a6ZCR[c6ZCR] = ((o6ZDR*2.0f) - (o6ER + o6KR + o6QR + o6WR + o6ZJR));
			dram->a6ZIR[c6ZIR] = ((o6ZJR*2.0f) - (o6ER + o6KR + o6QR + o6WR + o6ZDR));
			
			c6DR++; if (c6DR < 0 || c6DR > d6D) c6DR = 0;
			c6JR++; if (c6JR < 0 || c6JR > d6J) c6JR = 0;
			c6PR++; if (c6PR < 0 || c6PR > d6P) c6PR = 0;
			c6VR++; if (c6VR < 0 || c6VR > d6V) c6VR = 0;
			c6ZCR++; if (c6ZCR < 0 || c6ZCR > d6ZC) c6ZCR = 0;
			c6ZIR++; if (c6ZIR < 0 || c6ZIR > d6ZI) c6ZIR = 0;
			
			float o6DR = dram->a6DR[c6DR-((c6DR > d6D)?d6D+1:0)];
			float o6JR = dram->a6JR[c6JR-((c6JR > d6J)?d6J+1:0)];
			float o6PR = dram->a6PR[c6PR-((c6PR > d6P)?d6P+1:0)];
			float o6VR = dram->a6VR[c6VR-((c6VR > d6V)?d6V+1:0)];
			float o6ZCR = dram->a6ZCR[c6ZCR-((c6ZCR > d6ZC)?d6ZC+1:0)];
			float o6ZIR = dram->a6ZIR[c6ZIR-((c6ZIR > d6ZI)?d6ZI+1:0)];
			
			//-------- three
			
			dram->a6SL[c6SL] = ((o6ML*2.0f) - (o6NL + o6OL + o6PL + o6QL + o6RL));
			dram->a6TL[c6TL] = ((o6NL*2.0f) - (o6ML + o6OL + o6PL + o6QL + o6RL));
			dram->a6UL[c6UL] = ((o6OL*2.0f) - (o6ML + o6NL + o6PL + o6QL + o6RL));
			dram->a6VL[c6VL] = ((o6PL*2.0f) - (o6ML + o6NL + o6OL + o6QL + o6RL));
			dram->a6WL[c6WL] = ((o6QL*2.0f) - (o6ML + o6NL + o6OL + o6PL + o6RL));
			dram->a6XL[c6XL] = ((o6RL*2.0f) - (o6ML + o6NL + o6OL + o6PL + o6QL));
			
			c6SL++; if (c6SL < 0 || c6SL > d6S) c6SL = 0;
			c6TL++; if (c6TL < 0 || c6TL > d6T) c6TL = 0;
			c6UL++; if (c6UL < 0 || c6UL > d6U) c6UL = 0;
			c6VL++; if (c6VL < 0 || c6VL > d6V) c6VL = 0;
			c6WL++; if (c6WL < 0 || c6WL > d6W) c6WL = 0;
			c6XL++; if (c6XL < 0 || c6XL > d6X) c6XL = 0;
			
			float o6SL = dram->a6SL[c6SL-((c6SL > d6S)?d6S+1:0)];
			float o6TL = dram->a6TL[c6TL-((c6TL > d6T)?d6T+1:0)];
			float o6UL = dram->a6UL[c6UL-((c6UL > d6U)?d6U+1:0)];
			float o6VL = dram->a6VL[c6VL-((c6VL > d6V)?d6V+1:0)];
			float o6WL = dram->a6WL[c6WL-((c6WL > d6W)?d6W+1:0)];
			float o6XL = dram->a6XL[c6XL-((c6XL > d6X)?d6X+1:0)];
			
			dram->a6CR[c6CR] = ((o6DR*2.0f) - (o6JR + o6PR + o6VR + o6ZCR + o6ZIR));
			dram->a6IR[c6IR] = ((o6JR*2.0f) - (o6DR + o6PR + o6VR + o6ZCR + o6ZIR));
			dram->a6OR[c6OR] = ((o6PR*2.0f) - (o6DR + o6JR + o6VR + o6ZCR + o6ZIR));
			dram->a6UR[c6UR] = ((o6VR*2.0f) - (o6DR + o6JR + o6PR + o6ZCR + o6ZIR));
			dram->a6ZBR[c6ZBR] = ((o6ZCR*2.0f) - (o6DR + o6JR + o6PR + o6VR + o6ZIR));
			dram->a6ZHR[c6ZHR] = ((o6ZIR*2.0f) - (o6DR + o6JR + o6PR + o6VR + o6ZCR));
			
			c6CR++; if (c6CR < 0 || c6CR > d6C) c6CR = 0;
			c6IR++; if (c6IR < 0 || c6IR > d6I) c6IR = 0;
			c6OR++; if (c6OR < 0 || c6OR > d6O) c6OR = 0;
			c6UR++; if (c6UR < 0 || c6UR > d6U) c6UR = 0;
			c6ZBR++; if (c6ZBR < 0 || c6ZBR > d6ZB) c6ZBR = 0;
			c6ZHR++; if (c6ZHR < 0 || c6ZHR > d6ZH) c6ZHR = 0;
			
			float o6CR = dram->a6CR[c6CR-((c6CR > d6C)?d6C+1:0)];
			float o6IR = dram->a6IR[c6IR-((c6IR > d6I)?d6I+1:0)];
			float o6OR = dram->a6OR[c6OR-((c6OR > d6O)?d6O+1:0)];
			float o6UR = dram->a6UR[c6UR-((c6UR > d6U)?d6U+1:0)];
			float o6ZBR = dram->a6ZBR[c6ZBR-((c6ZBR > d6ZB)?d6ZB+1:0)];
			float o6ZHR = dram->a6ZHR[c6ZHR-((c6ZHR > d6ZH)?d6ZH+1:0)];
			
			//-------- four
			
			dram->a6YL[c6YL] = ((o6SL*2.0f) - (o6TL + o6UL + o6VL + o6WL + o6XL));
			dram->a6ZAL[c6ZAL] = ((o6TL*2.0f) - (o6SL + o6UL + o6VL + o6WL + o6XL));
			dram->a6ZBL[c6ZBL] = ((o6UL*2.0f) - (o6SL + o6TL + o6VL + o6WL + o6XL));
			dram->a6ZCL[c6ZCL] = ((o6VL*2.0f) - (o6SL + o6TL + o6UL + o6WL + o6XL));
			dram->a6ZDL[c6ZDL] = ((o6WL*2.0f) - (o6SL + o6TL + o6UL + o6VL + o6XL));
			dram->a6ZEL[c6ZEL] = ((o6XL*2.0f) - (o6SL + o6TL + o6UL + o6VL + o6WL));
			
			c6YL++; if (c6YL < 0 || c6YL > d6Y) c6YL = 0;
			c6ZAL++; if (c6ZAL < 0 || c6ZAL > d6ZA) c6ZAL = 0;
			c6ZBL++; if (c6ZBL < 0 || c6ZBL > d6ZB) c6ZBL = 0;
			c6ZCL++; if (c6ZCL < 0 || c6ZCL > d6ZC) c6ZCL = 0;
			c6ZDL++; if (c6ZDL < 0 || c6ZDL > d6ZD) c6ZDL = 0;
			c6ZEL++; if (c6ZEL < 0 || c6ZEL > d6ZE) c6ZEL = 0;
			
			float o6YL = dram->a6YL[c6YL-((c6YL > d6Y)?d6Y+1:0)];
			float o6ZAL = dram->a6ZAL[c6ZAL-((c6ZAL > d6ZA)?d6ZA+1:0)];
			float o6ZBL = dram->a6ZBL[c6ZBL-((c6ZBL > d6ZB)?d6ZB+1:0)];
			float o6ZCL = dram->a6ZCL[c6ZCL-((c6ZCL > d6ZC)?d6ZC+1:0)];
			float o6ZDL = dram->a6ZDL[c6ZDL-((c6ZDL > d6ZD)?d6ZD+1:0)];
			float o6ZEL = dram->a6ZEL[c6ZEL-((c6ZEL > d6ZE)?d6ZE+1:0)];
			
			dram->a6BR[c6BR] = ((o6CR*2.0f) - (o6IR + o6OR + o6UR + o6ZBR + o6ZHR));
			dram->a6HR[c6HR] = ((o6IR*2.0f) - (o6CR + o6OR + o6UR + o6ZBR + o6ZHR));
			dram->a6NR[c6NR] = ((o6OR*2.0f) - (o6CR + o6IR + o6UR + o6ZBR + o6ZHR));
			dram->a6TR[c6TR] = ((o6UR*2.0f) - (o6CR + o6IR + o6OR + o6ZBR + o6ZHR));
			dram->a6ZAR[c6ZAR] = ((o6ZBR*2.0f) - (o6CR + o6IR + o6OR + o6UR + o6ZHR));
			dram->a6ZGR[c6ZGR] = ((o6ZHR*2.0f) - (o6CR + o6IR + o6OR + o6UR + o6ZBR));
			
			c6BR++; if (c6BR < 0 || c6BR > d6B) c6BR = 0;
			c6HR++; if (c6HR < 0 || c6HR > d6H) c6HR = 0;
			c6NR++; if (c6NR < 0 || c6NR > d6N) c6NR = 0;
			c6TR++; if (c6TR < 0 || c6TR > d6T) c6TR = 0;
			c6ZBR++; if (c6ZBR < 0 || c6ZBR > d6ZB) c6ZBR = 0;
			c6ZGR++; if (c6ZGR < 0 || c6ZGR > d6ZG) c6ZGR = 0;
			
			float o6BR = dram->a6BR[c6BR-((c6BR > d6B)?d6B+1:0)];
			float o6HR = dram->a6HR[c6HR-((c6HR > d6H)?d6H+1:0)];
			float o6NR = dram->a6NR[c6NR-((c6NR > d6N)?d6N+1:0)];
			float o6TR = dram->a6TR[c6TR-((c6TR > d6T)?d6T+1:0)];
			float o6ZAR = dram->a6ZAR[c6ZAR-((c6ZAR > d6ZA)?d6ZA+1:0)];
			float o6ZGR = dram->a6ZGR[c6ZGR-((c6ZGR > d6ZG)?d6ZG+1:0)];
			
			//-------- five
			
			dram->a6ZFL[c6ZFL] = ((o6YL*2.0f) - (o6ZAL + o6ZBL + o6ZCL + o6ZDL + o6ZEL));
			dram->a6ZGL[c6ZGL] = ((o6ZAL*2.0f) - (o6YL + o6ZBL + o6ZCL + o6ZDL + o6ZEL));
			dram->a6ZHL[c6ZHL] = ((o6ZBL*2.0f) - (o6YL + o6ZAL + o6ZCL + o6ZDL + o6ZEL));
			dram->a6ZIL[c6ZIL] = ((o6ZCL*2.0f) - (o6YL + o6ZAL + o6ZBL + o6ZDL + o6ZEL));
			dram->a6ZJL[c6ZJL] = ((o6ZDL*2.0f) - (o6YL + o6ZAL + o6ZBL + o6ZCL + o6ZEL));
			dram->a6ZKL[c6ZKL] = ((o6ZEL*2.0f) - (o6YL + o6ZAL + o6ZBL + o6ZCL + o6ZDL));
			
			c6ZFL++; if (c6ZFL < 0 || c6ZFL > d6ZF) c6ZFL = 0;
			c6ZGL++; if (c6ZGL < 0 || c6ZGL > d6ZG) c6ZGL = 0;
			c6ZHL++; if (c6ZHL < 0 || c6ZHL > d6ZH) c6ZHL = 0;
			c6ZIL++; if (c6ZIL < 0 || c6ZIL > d6ZI) c6ZIL = 0;
			c6ZJL++; if (c6ZJL < 0 || c6ZJL > d6ZJ) c6ZJL = 0;
			c6ZKL++; if (c6ZKL < 0 || c6ZKL > d6ZK) c6ZKL = 0;
			
			float o6ZFL = dram->a6ZFL[c6ZFL-((c6ZFL > d6ZF)?d6ZF+1:0)];
			float o6ZGL = dram->a6ZGL[c6ZGL-((c6ZGL > d6ZG)?d6ZG+1:0)];
			float o6ZHL = dram->a6ZHL[c6ZHL-((c6ZHL > d6ZH)?d6ZH+1:0)];
			float o6ZIL = dram->a6ZIL[c6ZIL-((c6ZIL > d6ZI)?d6ZI+1:0)];
			float o6ZJL = dram->a6ZJL[c6ZJL-((c6ZJL > d6ZJ)?d6ZJ+1:0)];
			float o6ZKL = dram->a6ZKL[c6ZKL-((c6ZKL > d6ZK)?d6ZK+1:0)];
			
			dram->a6AR[c6AR] = ((o6BR*2.0f) - (o6HR + o6NR + o6TR + o6ZAR + o6ZGR));
			dram->a6GR[c6GR] = ((o6HR*2.0f) - (o6BR + o6NR + o6TR + o6ZAR + o6ZGR));
			dram->a6MR[c6MR] = ((o6NR*2.0f) - (o6BR + o6HR + o6TR + o6ZAR + o6ZGR));
			dram->a6SR[c6SR] = ((o6TR*2.0f) - (o6BR + o6HR + o6NR + o6ZAR + o6ZGR));
			dram->a6YR[c6YR] = ((o6ZAR*2.0f) - (o6BR + o6HR + o6NR + o6TR + o6ZGR));
			dram->a6ZFR[c6ZFR] = ((o6ZGR*2.0f) - (o6BR + o6HR + o6NR + o6TR + o6ZAR));
			
			c6AR++; if (c6AR < 0 || c6AR > d6A) c6AR = 0;
			c6GR++; if (c6GR < 0 || c6GR > d6G) c6GR = 0;
			c6MR++; if (c6MR < 0 || c6MR > d6M) c6MR = 0;
			c6SR++; if (c6SR < 0 || c6SR > d6S) c6SR = 0;
			c6YR++; if (c6YR < 0 || c6YR > d6Y) c6YR = 0;
			c6ZFR++; if (c6ZFR < 0 || c6ZFR > d6ZF) c6ZFR = 0;
			
			float o6AR = dram->a6AR[c6AR-((c6AR > d6A)?d6A+1:0)];
			float o6GR = dram->a6GR[c6GR-((c6GR > d6G)?d6G+1:0)];
			float o6MR = dram->a6MR[c6MR-((c6MR > d6M)?d6M+1:0)];
			float o6SR = dram->a6SR[c6SR-((c6SR > d6S)?d6S+1:0)];
			float o6YR = dram->a6YR[c6YR-((c6YR > d6Y)?d6Y+1:0)];
			float o6ZFR = dram->a6ZFR[c6ZFR-((c6ZFR > d6ZF)?d6ZF+1:0)];
			
			//-------- six
			
			f6AL = ((o6AR*2.0f) - (o6GR + o6MR + o6SR + o6YR + o6ZFR));
			f6BL = ((o6GR*2.0f) - (o6AR + o6MR + o6SR + o6YR + o6ZFR));
			f6CL = ((o6MR*2.0f) - (o6AR + o6GR + o6SR + o6YR + o6ZFR));
			f6DL = ((o6SR*2.0f) - (o6AR + o6GR + o6MR + o6YR + o6ZFR));
			f6EL = ((o6YR*2.0f) - (o6AR + o6GR + o6MR + o6SR + o6ZFR));
			f6FL = ((o6ZFR*2.0f) - (o6AR + o6GR + o6MR + o6SR + o6YR));
			
			f6FR = ((o6ZFL*2.0f) - (o6ZGL + o6ZHL + o6ZIL + o6ZJL + o6ZKL));
			f6LR = ((o6ZGL*2.0f) - (o6ZFL + o6ZHL + o6ZIL + o6ZJL + o6ZKL));
			f6RR = ((o6ZHL*2.0f) - (o6ZFL + o6ZGL + o6ZIL + o6ZJL + o6ZKL));
			f6XR = ((o6ZIL*2.0f) - (o6ZFL + o6ZGL + o6ZHL + o6ZJL + o6ZKL));
			f6ZER = ((o6ZJL*2.0f) - (o6ZFL + o6ZGL + o6ZHL + o6ZIL + o6ZKL));
			f6ZKR = ((o6ZKL*2.0f) - (o6ZFL + o6ZGL + o6ZHL + o6ZIL + o6ZJL));
			
			inputSampleL = (o6ZFL + o6ZGL + o6ZHL + o6ZIL + o6ZJL + o6ZKL)*0.001953125f;
			inputSampleR = (o6AR + o6GR + o6MR + o6SR + o6YR + o6ZFR)*0.001953125f;
			
			f6AL = (f6AL+f6AL+f6AL+avg6L)*0.25f; avg6L = f6AL;
			f6FR = (f6FR+f6FR+f6FR+avg6R)*0.25f; avg6R = f6FR;
			//manipulating deep reverb tail for realism
			
			inputSampleL += (earlyReflectionL * earlyLoudness);
			inputSampleR += (earlyReflectionR * earlyLoudness);
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0f;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = inputSampleR;
			dram->bez[bez_SampR] = 0.0f;
		}
		float CBL = (dram->bez[bez_CL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BL]*dram->bez[bez_cycle]);
		float CBR = (dram->bez[bez_CR]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BR]*dram->bez[bez_cycle]);
		float BAL = (dram->bez[bez_BL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AL]*dram->bez[bez_cycle]);
		float BAR = (dram->bez[bez_BR]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AR]*dram->bez[bez_cycle]);
		float CBAL = (dram->bez[bez_BL]+(CBL*(1.0f-dram->bez[bez_cycle]))+(BAL*dram->bez[bez_cycle]))*0.125f;
		float CBAR = (dram->bez[bez_BR]+(CBR*(1.0f-dram->bez[bez_cycle]))+(BAR*dram->bez[bez_cycle]))*0.125f;
		inputSampleL = CBAL;
		inputSampleR = CBAR;
		
		inputSampleL = (inputSampleL * wet)+(drySampleL * (1.0f-wet));
		inputSampleR = (inputSampleR * wet)+(drySampleR * (1.0f-wet));
		
		
		
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
	for(int x = 0; x < d3A+2; x++) {dram->a3AL[x] = 0.0; dram->a3AR[x] = 0.0;}
	for(int x = 0; x < d3B+2; x++) {dram->a3BL[x] = 0.0; dram->a3BR[x] = 0.0;}
	for(int x = 0; x < d3C+2; x++) {dram->a3CL[x] = 0.0; dram->a3CR[x] = 0.0;}
	for(int x = 0; x < d3D+2; x++) {dram->a3DL[x] = 0.0; dram->a3DR[x] = 0.0;}
	for(int x = 0; x < d3E+2; x++) {dram->a3EL[x] = 0.0; dram->a3ER[x] = 0.0;}
	for(int x = 0; x < d3F+2; x++) {dram->a3FL[x] = 0.0; dram->a3FR[x] = 0.0;}
	for(int x = 0; x < d3G+2; x++) {dram->a3GL[x] = 0.0; dram->a3GR[x] = 0.0;}
	for(int x = 0; x < d3H+2; x++) {dram->a3HL[x] = 0.0; dram->a3HR[x] = 0.0;}
	for(int x = 0; x < d3I+2; x++) {dram->a3IL[x] = 0.0; dram->a3IR[x] = 0.0;}
	c3AL = c3BL = c3CL = c3DL = c3EL = c3FL = c3GL = c3HL = c3IL = 1;
	c3AR = c3BR = c3CR = c3DR = c3ER = c3FR = c3GR = c3HR = c3IR = 1;
	
	for(int x = 0; x < d6A+2; x++) {dram->a6AL[x] = 0.0; dram->a6AR[x] = 0.0;}
	for(int x = 0; x < d6B+2; x++) {dram->a6BL[x] = 0.0; dram->a6BR[x] = 0.0;}
	for(int x = 0; x < d6C+2; x++) {dram->a6CL[x] = 0.0; dram->a6CR[x] = 0.0;}
	for(int x = 0; x < d6D+2; x++) {dram->a6DL[x] = 0.0; dram->a6DR[x] = 0.0;}
	for(int x = 0; x < d6E+2; x++) {dram->a6EL[x] = 0.0; dram->a6ER[x] = 0.0;}
	for(int x = 0; x < d6F+2; x++) {dram->a6FL[x] = 0.0; dram->a6FR[x] = 0.0;}
	for(int x = 0; x < d6G+2; x++) {dram->a6GL[x] = 0.0; dram->a6GR[x] = 0.0;}
	for(int x = 0; x < d6H+2; x++) {dram->a6HL[x] = 0.0; dram->a6HR[x] = 0.0;}
	for(int x = 0; x < d6I+2; x++) {dram->a6IL[x] = 0.0; dram->a6IR[x] = 0.0;}
	for(int x = 0; x < d6J+2; x++) {dram->a6JL[x] = 0.0; dram->a6JR[x] = 0.0;}
	for(int x = 0; x < d6K+2; x++) {dram->a6KL[x] = 0.0; dram->a6KR[x] = 0.0;}
	for(int x = 0; x < d6L+2; x++) {dram->a6LL[x] = 0.0; dram->a6LR[x] = 0.0;}
	for(int x = 0; x < d6M+2; x++) {dram->a6ML[x] = 0.0; dram->a6MR[x] = 0.0;}
	for(int x = 0; x < d6N+2; x++) {dram->a6NL[x] = 0.0; dram->a6NR[x] = 0.0;}
	for(int x = 0; x < d6O+2; x++) {dram->a6OL[x] = 0.0; dram->a6OR[x] = 0.0;}
	for(int x = 0; x < d6P+2; x++) {dram->a6PL[x] = 0.0; dram->a6PR[x] = 0.0;}
	for(int x = 0; x < d6Q+2; x++) {dram->a6QL[x] = 0.0; dram->a6QR[x] = 0.0;}
	for(int x = 0; x < d6R+2; x++) {dram->a6RL[x] = 0.0; dram->a6RR[x] = 0.0;}
	for(int x = 0; x < d6S+2; x++) {dram->a6SL[x] = 0.0; dram->a6SR[x] = 0.0;}
	for(int x = 0; x < d6T+2; x++) {dram->a6TL[x] = 0.0; dram->a6TR[x] = 0.0;}
	for(int x = 0; x < d6U+2; x++) {dram->a6UL[x] = 0.0; dram->a6UR[x] = 0.0;}
	for(int x = 0; x < d6V+2; x++) {dram->a6VL[x] = 0.0; dram->a6VR[x] = 0.0;}
	for(int x = 0; x < d6W+2; x++) {dram->a6WL[x] = 0.0; dram->a6WR[x] = 0.0;}
	for(int x = 0; x < d6X+2; x++) {dram->a6XL[x] = 0.0; dram->a6XR[x] = 0.0;}
	for(int x = 0; x < d6Y+2; x++) {dram->a6YL[x] = 0.0; dram->a6YR[x] = 0.0;}
	for(int x = 0; x < d6ZA+2; x++) {dram->a6ZAL[x] = 0.0; dram->a6ZAR[x] = 0.0;}
	for(int x = 0; x < d6ZB+2; x++) {dram->a6ZBL[x] = 0.0; dram->a6ZBR[x] = 0.0;}
	for(int x = 0; x < d6ZC+2; x++) {dram->a6ZCL[x] = 0.0; dram->a6ZCR[x] = 0.0;}
	for(int x = 0; x < d6ZD+2; x++) {dram->a6ZDL[x] = 0.0; dram->a6ZDR[x] = 0.0;}
	for(int x = 0; x < d6ZE+2; x++) {dram->a6ZEL[x] = 0.0; dram->a6ZER[x] = 0.0;}
	for(int x = 0; x < d6ZF+2; x++) {dram->a6ZFL[x] = 0.0; dram->a6ZFR[x] = 0.0;}
	for(int x = 0; x < d6ZG+2; x++) {dram->a6ZGL[x] = 0.0; dram->a6ZGR[x] = 0.0;}
	for(int x = 0; x < d6ZH+2; x++) {dram->a6ZHL[x] = 0.0; dram->a6ZHR[x] = 0.0;}
	for(int x = 0; x < d6ZI+2; x++) {dram->a6ZIL[x] = 0.0; dram->a6ZIR[x] = 0.0;}
	for(int x = 0; x < d6ZJ+2; x++) {dram->a6ZJL[x] = 0.0; dram->a6ZJR[x] = 0.0;}
	for(int x = 0; x < d6ZK+2; x++) {dram->a6ZKL[x] = 0.0; dram->a6ZKR[x] = 0.0;}
	c6AL = c6BL = c6CL = c6DL = c6EL = c6FL = c6GL = c6HL = c6IL = 1;
	c6JL = c6KL = c6LL = c6ML = c6NL = c6OL = c6PL = c6QL = c6RL = 1;
	c6SL = c6TL = c6UL = c6VL = c6WL = c6XL = c6YL = c6ZAL = c6ZBL = 1;
	c6ZCL = c6ZDL = c6ZEL = c6ZFL = c6ZGL = c6ZHL = c6ZIL = c6ZJL = c6ZKL = 1;
	c6AR = c6BR = c6CR = c6DR = c6ER = c6FR = c6GR = c6HR = c6IR = 1;
	c6JR = c6KR = c6LR = c6MR = c6NR = c6OR = c6PR = c6QR = c6RR = 1;
	c6SR = c6TR = c6UR = c6VR = c6WR = c6XR = c6YR = c6ZAR = c6ZBR = 1;
	c6ZCR = c6ZDR = c6ZER = c6ZFR = c6ZGR = c6ZHR = c6ZIR = c6ZJR = c6ZKR = 1;
	f6AL = f6BL = f6CL = f6DL = f6EL = f6FL = 0.0;
	f6FR = f6LR = f6RR = f6XR = f6ZER = f6ZKR = 0.0;
	avg6L = avg6R = 0.0;	
	
	for(int count = 0; count < predelay+2; count++) {dram->aZL[count] = 0.0; dram->aZR[count] = 0.0;}
	countZ = 1;	
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	for(int count = 0; count < 32767; count++) {dram->firBufferL[count] = 0.0; dram->firBufferR[count] = 0.0;}
	firPosition = 0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
