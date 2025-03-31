#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "CrickBass"
#define AIRWINDOWS_DESCRIPTION "A flexible but aggressive bass tone for dual pickups."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','r','i' )
enum {

	kParam_One =0,
	kParam_Two =1,
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
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 6 };
#include "../include/template1.h"
 
	
	double lastASampleL;
	double lastSlewL;
	double iirSampleAL;
	double iirSampleBL;
	double iirSampleCL;
	double iirSampleDL;
	double iirSampleEL;
	double iirSampleFL;
	double iirSampleGL;
	double iirSampleHL;
	double iirSampleIL;
	double iirSampleJL;
	
	double lastCabSampleL;
	double smoothCabAL;
	double smoothCabBL; //cab
	
	
	double lastASampleR;
	double lastSlewR;
	double iirSampleAR;
	double iirSampleBR;
	double iirSampleCR;
	double iirSampleDR;
	double iirSampleER;
	double OddAR;
	double OddBR;
	double OddCR;
	double OddDR;
	double OddER;
	double EvenAR;
	double EvenBR;
	double EvenCR;
	double EvenDR;
	double EvenER;
	
	double lastCabSampleR;
	double smoothCabAR;
	double smoothCabBR; //cab
	

	double lastRefL[10];
	double lastRefR[10];
	int cycle;	//undersampling
	
	bool flip;
	int count; //amp
	
	enum {
		fix_freq,
		fix_reso,
		fix_a0,
		fix_a1,
		fix_a2,
		fix_b1,
		fix_b2,
		fix_sL1,
		fix_sL2,
		fix_sR1,
		fix_sR2,
		fix_total
	}; //fixed frequency biquad filter for ultrasonics, stereo
	double fixA[fix_total];
	double fixB[fix_total];
	double fixC[fix_total];
	double fixD[fix_total];
	double fixE[fix_total];
	double fixF[fix_total]; //filtering
	
	double lastSampleR;
	double intermediateR[16];
	bool wasPosClipR;
	bool wasNegClipR; //ClipOnly2	
	
	uint32_t fpdL;
	uint32_t fpdR;
#include "../include/template2.h"
struct _dram {
	double OddL[257];
	double EvenL[257]; //amp
	double bL[90];
	double bR[90];
};
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check		
	
	double inputlevelL = pow(GetParameter( kParam_One )+0.5,2);
	double inputlevelH = inputlevelL*4.0;
	double basstrimH = (GetParameter( kParam_Two )*0.6)+0.2;
	double basstrimL = basstrimH*0.618;
	
	double EQL = (basstrimL/GetSampleRate())*22050.0;
	double EQH = (basstrimH/GetSampleRate())*22050.0;
	double BEQ = (0.0625/GetSampleRate())*22050.0;
	
	int diagonal = (int)(0.000861678*GetSampleRate());
	if (diagonal > 127) diagonal = 127;
	int side = (int)(diagonal/1.4142135623730951);
	int down = (side + diagonal)/2;
	//now we've got down, side and diagonal as offsets and we also use three successive samples upfront
	double skewlevel = pow(basstrimH,2);
	
	double cutoff = (15000.0+(basstrimH*10000.0))/GetSampleRate();
	if (cutoff > 0.49) cutoff = 0.49; //don't crash if run at 44.1k
	if (cutoff < 0.001) cutoff = 0.001; //or if cutoff's too low
	
	fixF[fix_freq] = fixE[fix_freq] = fixD[fix_freq] = fixC[fix_freq] = fixB[fix_freq] = fixA[fix_freq] = cutoff;
	
    fixA[fix_reso] = 4.46570214;
	fixB[fix_reso] = 1.51387132;
	fixC[fix_reso] = 0.93979296;
	fixD[fix_reso] = 0.70710678;
	fixE[fix_reso] = 0.52972649;
	fixF[fix_reso] = 0.50316379;
	
	double K = tan(M_PI * fixA[fix_freq]); //lowpass
	double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
	fixA[fix_a0] = K * K * norm;
	fixA[fix_a1] = 2.0 * fixA[fix_a0];
	fixA[fix_a2] = fixA[fix_a0];
	fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixB[fix_freq]);
	norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
	fixB[fix_a0] = K * K * norm;
	fixB[fix_a1] = 2.0 * fixB[fix_a0];
	fixB[fix_a2] = fixB[fix_a0];
	fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixC[fix_freq]);
	norm = 1.0 / (1.0 + K / fixC[fix_reso] + K * K);
	fixC[fix_a0] = K * K * norm;
	fixC[fix_a1] = 2.0 * fixC[fix_a0];
	fixC[fix_a2] = fixC[fix_a0];
	fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixC[fix_b2] = (1.0 - K / fixC[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixD[fix_freq]);
	norm = 1.0 / (1.0 + K / fixD[fix_reso] + K * K);
	fixD[fix_a0] = K * K * norm;
	fixD[fix_a1] = 2.0 * fixD[fix_a0];
	fixD[fix_a2] = fixD[fix_a0];
	fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixD[fix_b2] = (1.0 - K / fixD[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixE[fix_freq]);
	norm = 1.0 / (1.0 + K / fixE[fix_reso] + K * K);
	fixE[fix_a0] = K * K * norm;
	fixE[fix_a1] = 2.0 * fixE[fix_a0];
	fixE[fix_a2] = fixE[fix_a0];
	fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixE[fix_b2] = (1.0 - K / fixE[fix_reso] + K * K) * norm;
	
	K = tan(M_PI * fixF[fix_freq]);
	norm = 1.0 / (1.0 + K / fixF[fix_reso] + K * K);
	fixF[fix_a0] = K * K * norm;
	fixF[fix_a1] = 2.0 * fixF[fix_a0];
	fixF[fix_a2] = fixF[fix_a0];
	fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
	fixF[fix_b2] = (1.0 - K / fixF[fix_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *inputL;
		double inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		double outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
		fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
		fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
		fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
		fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 1
		
		double skewL = (inputSampleL - lastASampleL);
		lastASampleL = inputSampleL;
		//skew will be direction/angle
		double bridgerectifier = fabs(skewL);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewL > 0) skewL = bridgerectifier;
		else skewL = -bridgerectifier;
		//skew is now sined and clamped and then re-amplified again
		skewL *= inputSampleL;
		skewL = (skewL+(skewL*basstrimL))/2.0;
		inputSampleL *= basstrimL;
		double basscut = basstrimL;
		
		double skewR = (inputSampleR - lastASampleR);
		lastASampleR = inputSampleR;
		//skew will be direction/angle
		bridgerectifier = fabs(skewR);
		if (bridgerectifier > 3.1415926) bridgerectifier = 3.1415926;
		//for skew we want it to go to zero effect again, so we use full range of the sine
		bridgerectifier = sin(bridgerectifier);
		if (skewR > 0.0) skewR = bridgerectifier;
		else skewR = -bridgerectifier;
		//skew is now sined and clamped and then re-amplified again
		skewR *= inputSampleR;
		skewR *= skewlevel;
		inputSampleR *= basstrimH;
		inputSampleR *= inputlevelH;
		double offsetR = (1.0 - EQH) + (fabs(inputSampleR)*EQH);
		if (offsetR < 0.0) offsetR = 0.0;
		if (offsetR > 1.0) offsetR = 1.0;
		iirSampleAR = (iirSampleAR * (1.0 - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = inputSampleR - iirSampleAR;
		//highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (inputSampleR > 0) inputSampleR = (inputSampleR*(-0.57079633+skewR))+(bridgerectifier*(1.57079633+skewR));
		else inputSampleR = (inputSampleR*(-0.57079633+skewR))-(bridgerectifier*(1.57079633+skewR));
		//overdrive
		iirSampleCR = (iirSampleCR * (1.0 - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = iirSampleCR;
		//lowpass. Use offset from before gain stage
		//finished first gain stage
		
		outSample = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
		fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
		fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
		fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sR2];
		fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 2
		
		inputSampleL *= inputlevelL;
		double offsetL = (1.0 - EQL) + (fabs(inputSampleL)*EQL);
		if (offsetL < 0.0) offsetL = 0.0;
		if (offsetL > 1.0) offsetL = 1.0;
		iirSampleAL = (iirSampleAL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleAL*basscut);
		//highpass
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL*(-0.57079633+skewL))+(bridgerectifier*(1.57079633+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633+skewL))-(bridgerectifier*(1.57079633+skewL));
		//overdrive
		iirSampleBL = (iirSampleBL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleBL*basscut);
		//highpass. Use offset from before gain stage
		//finished first gain stage
		
		inputSampleR *= inputlevelH;
		offsetR = (1.0 + offsetR) / 2.0;
		iirSampleBR = (iirSampleBR * (1.0 - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = inputSampleR - iirSampleBR;
		//highpass
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewR;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR*(-0.57079633+skewR))+(bridgerectifier*(1.57079633+skewR));
		else inputSampleR = (inputSampleR*(-0.57079633+skewR))-(bridgerectifier*(1.57079633+skewR));
		//overdrive
		iirSampleDR = (iirSampleDR * (1.0 - (offsetR * EQH))) + (inputSampleR * (offsetR * EQH));
		inputSampleR = iirSampleDR;
		//lowpass. Use offset from before gain stage
		if (flip)
		{
			OddDR = OddCR; OddCR = OddBR; OddBR = OddAR; OddAR = inputSampleR;
			inputSampleR = (OddAR + OddBR + OddCR + OddDR) / 4.0;
		}
		else
		{
			EvenDR = EvenCR; EvenCR = EvenBR; EvenBR = EvenAR; EvenAR = inputSampleR;
			inputSampleR = (EvenAR + EvenBR + EvenCR + EvenDR) / 4.0;
		}
		
		outSample = (inputSampleL * fixC[fix_a0]) + fixC[fix_sL1];
		fixC[fix_sL1] = (inputSampleL * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sL2];
		fixC[fix_sL2] = (inputSampleL * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fixC[fix_a0]) + fixC[fix_sR1];
		fixC[fix_sR1] = (inputSampleR * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sR2];
		fixC[fix_sR2] = (inputSampleR * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 3
		
		inputSampleL *= inputlevelL;
		skewL /= 2.0;
		offsetL = (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL*(-0.57079633+skewL))+(bridgerectifier*(1.57079633+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633+skewL))-(bridgerectifier*(1.57079633+skewL));
		//overdrive
		iirSampleCL = (iirSampleCL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleCL*basscut);
		//highpass.
		iirSampleDL = (iirSampleDL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = iirSampleDL;
		//lowpass. Use offset from before gain stage
		
		inputSampleR *= inputlevelH;
		bridgerectifier = fabs(inputSampleR) + skewR;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleR > 0.0) inputSampleR = (inputSampleR*-0.57079633)+(bridgerectifier*1.57079633);
		else inputSampleR = (inputSampleR*-0.57079633)-(bridgerectifier*1.57079633);
		//output stage has less gain, no highpass, straight lowpass
		iirSampleER = (iirSampleER * (1.0 - EQH)) + (inputSampleR * EQH);
		inputSampleR = iirSampleER;
		//lowpass. Use offset from before gain stage

		outSample = (inputSampleL * fixD[fix_a0]) + fixD[fix_sL1];
		fixD[fix_sL1] = (inputSampleL * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sL2];
		fixD[fix_sL2] = (inputSampleL * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fixD[fix_a0]) + fixD[fix_sR1];
		fixD[fix_sR1] = (inputSampleR * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sR2];
		fixD[fix_sR2] = (inputSampleR * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 4
		
		inputSampleL *= inputlevelL;
		skewL /= 2.0;
		offsetL = (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = (sin(bridgerectifier) * 1.57079633) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL*(-0.57079633+skewL))+(bridgerectifier*(1.57079633+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633+skewL))-(bridgerectifier*(1.57079633+skewL));
		//overdrive
		iirSampleEL = (iirSampleEL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = inputSampleL - (iirSampleEL*basscut);
		//we don't need to do basscut again, that was the last one
		//highpass.
		iirSampleFL = (iirSampleFL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = iirSampleFL;
		//lowpass. Use offset from before gain stage
		
		inputSampleR = sin(inputSampleR);
		
		outSample = (inputSampleL * fixE[fix_a0]) + fixE[fix_sL1];
		fixE[fix_sL1] = (inputSampleL * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sL2];
		fixE[fix_sL2] = (inputSampleL * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fixE[fix_a0]) + fixE[fix_sR1];
		fixE[fix_sR1] = (inputSampleR * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sR2];
		fixE[fix_sR2] = (inputSampleR * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 5
		
		inputSampleL *= inputlevelL;
		skewL /= 2.0;
		offsetL= (1.0 + offsetL) / 2.0;
		bridgerectifier = fabs(inputSampleL) + skewL;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier) * 1.57079633;
		if (inputSampleL > 0.0) inputSampleL = (inputSampleL*(-0.57079633+skewL))+(bridgerectifier*(1.57079633+skewL));
		else inputSampleL = (inputSampleL*(-0.57079633+skewL))-(bridgerectifier*(1.57079633+skewL));
		//output stage has less gain, no highpass, straight lowpass
		iirSampleGL = (iirSampleGL * (1.0 - (offsetL * EQL))) + (inputSampleL * (offsetL * EQL));
		inputSampleL = iirSampleGL;
		//lowpass. Use offset from before gain stage
		iirSampleHL = (iirSampleHL * (1.0 - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		//extra lowpass for 4*12" speakers
		
		if (count < 0 || count > 128) count = 128;
		double resultBL = 0.0;
		if (flip)
		{
			dram->OddL[count+128] = dram->OddL[count] = iirSampleHL;
			resultBL = (dram->OddL[count+down] + dram->OddL[count+side] + dram->OddL[count+diagonal]);
		} else {
			dram->EvenL[count+128] = dram->EvenL[count] = iirSampleHL;
			resultBL = (dram->EvenL[count+down] + dram->EvenL[count+side] + dram->EvenL[count+diagonal]);
		}
		count--;
		
		iirSampleIL = (iirSampleIL * (1.0 - (offsetL * BEQ))) + (resultBL * (offsetL * BEQ));
		inputSampleL += (iirSampleIL*0.0625);
		//extra lowpass for 4*12" speakers
		iirSampleJL = (iirSampleJL * (1.0 - (offsetL * BEQ))) + (inputSampleL * (offsetL * BEQ));
		inputSampleL += iirSampleJL;
		inputSampleL = sin(inputSampleL);
				
		outSample = (inputSampleL * fixF[fix_a0]) + fixF[fix_sL1];
		fixF[fix_sL1] = (inputSampleL * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sL2];
		fixF[fix_sL2] = (inputSampleL * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
		inputSampleL = outSample; //fixed biquad filtering ultrasonics
		outSample = (inputSampleR * fixF[fix_a0]) + fixF[fix_sR1];
		fixF[fix_sR1] = (inputSampleR * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sR2];
		fixF[fix_sR2] = (inputSampleR * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
		inputSampleR = outSample; //fixed biquad filtering ultrasonics 6
		
		flip = !flip;
		//amp
		
		cycle++;
		if (cycle == cycleEnd) {
			
			double temp = (inputSampleL + smoothCabAL)/3.0;
			smoothCabAL = inputSampleL;
			inputSampleL = temp;
			
			dram->bL[81] = dram->bL[80]; dram->bL[80] = dram->bL[79]; 
			dram->bL[79] = dram->bL[78]; dram->bL[78] = dram->bL[77]; dram->bL[77] = dram->bL[76]; dram->bL[76] = dram->bL[75]; dram->bL[75] = dram->bL[74]; dram->bL[74] = dram->bL[73]; dram->bL[73] = dram->bL[72]; dram->bL[72] = dram->bL[71]; 
			dram->bL[71] = dram->bL[70]; dram->bL[70] = dram->bL[69]; dram->bL[69] = dram->bL[68]; dram->bL[68] = dram->bL[67]; dram->bL[67] = dram->bL[66]; dram->bL[66] = dram->bL[65]; dram->bL[65] = dram->bL[64]; dram->bL[64] = dram->bL[63]; 
			dram->bL[63] = dram->bL[62]; dram->bL[62] = dram->bL[61]; dram->bL[61] = dram->bL[60]; dram->bL[60] = dram->bL[59]; dram->bL[59] = dram->bL[58]; dram->bL[58] = dram->bL[57]; dram->bL[57] = dram->bL[56]; dram->bL[56] = dram->bL[55]; 
			dram->bL[55] = dram->bL[54]; dram->bL[54] = dram->bL[53]; dram->bL[53] = dram->bL[52]; dram->bL[52] = dram->bL[51]; dram->bL[51] = dram->bL[50]; dram->bL[50] = dram->bL[49]; dram->bL[49] = dram->bL[48]; dram->bL[48] = dram->bL[47]; 
			dram->bL[47] = dram->bL[46]; dram->bL[46] = dram->bL[45]; dram->bL[45] = dram->bL[44]; dram->bL[44] = dram->bL[43]; dram->bL[43] = dram->bL[42]; dram->bL[42] = dram->bL[41]; dram->bL[41] = dram->bL[40]; dram->bL[40] = dram->bL[39]; 
			dram->bL[39] = dram->bL[38]; dram->bL[38] = dram->bL[37]; dram->bL[37] = dram->bL[36]; dram->bL[36] = dram->bL[35]; dram->bL[35] = dram->bL[34]; dram->bL[34] = dram->bL[33]; dram->bL[33] = dram->bL[32]; dram->bL[32] = dram->bL[31]; 
			dram->bL[31] = dram->bL[30]; dram->bL[30] = dram->bL[29]; dram->bL[29] = dram->bL[28]; dram->bL[28] = dram->bL[27]; dram->bL[27] = dram->bL[26]; dram->bL[26] = dram->bL[25]; dram->bL[25] = dram->bL[24]; dram->bL[24] = dram->bL[23]; 
			dram->bL[23] = dram->bL[22]; dram->bL[22] = dram->bL[21]; dram->bL[21] = dram->bL[20]; dram->bL[20] = dram->bL[19]; dram->bL[19] = dram->bL[18]; dram->bL[18] = dram->bL[17]; dram->bL[17] = dram->bL[16]; dram->bL[16] = dram->bL[15]; 
			dram->bL[15] = dram->bL[14]; dram->bL[14] = dram->bL[13]; dram->bL[13] = dram->bL[12]; dram->bL[12] = dram->bL[11]; dram->bL[11] = dram->bL[10]; dram->bL[10] = dram->bL[9]; dram->bL[9] = dram->bL[8]; dram->bL[8] = dram->bL[7]; 
			dram->bL[7] = dram->bL[6]; dram->bL[6] = dram->bL[5]; dram->bL[5] = dram->bL[4]; dram->bL[4] = dram->bL[3]; dram->bL[3] = dram->bL[2]; dram->bL[2] = dram->bL[1]; dram->bL[1] = dram->bL[0]; dram->bL[0] = inputSampleL;
			
			inputSampleL += (dram->bL[1] * (1.35472031405494242  + (0.00220914099195157*fabs(dram->bL[1]))));
			inputSampleL += (dram->bL[2] * (1.63534207755253003  - (0.11406232654509685*fabs(dram->bL[2]))));
			inputSampleL += (dram->bL[3] * (1.82334575691525869  - (0.42647194712964054*fabs(dram->bL[3]))));
			inputSampleL += (dram->bL[4] * (1.86156386235405868  - (0.76744187887586590*fabs(dram->bL[4]))));
			inputSampleL += (dram->bL[5] * (1.67332739338852599  - (0.95161997324293013*fabs(dram->bL[5]))));
			inputSampleL += (dram->bL[6] * (1.25054130794899021  - (0.98410433514572859*fabs(dram->bL[6]))));
			inputSampleL += (dram->bL[7] * (0.70049121047281737  - (0.87375612110718992*fabs(dram->bL[7]))));
			inputSampleL += (dram->bL[8] * (0.15291791448081560  - (0.61195266024519046*fabs(dram->bL[8]))));
			inputSampleL -= (dram->bL[9] * (0.37301992914152693  + (0.16755422915252094*fabs(dram->bL[9]))));
			inputSampleL -= (dram->bL[10] * (0.76568539228498433  - (0.28554435228965386*fabs(dram->bL[10]))));
			inputSampleL -= (dram->bL[11] * (0.95726568749937369  - (0.61659719162806048*fabs(dram->bL[11]))));
			inputSampleL -= (dram->bL[12] * (1.01273552193911032  - (0.81827288407943954*fabs(dram->bL[12]))));
			inputSampleL -= (dram->bL[13] * (0.93920108117234447  - (0.80077111864205874*fabs(dram->bL[13]))));
			inputSampleL -= (dram->bL[14] * (0.79831898832953974  - (0.65814750339694406*fabs(dram->bL[14]))));
			inputSampleL -= (dram->bL[15] * (0.64200088100452313  - (0.46135833001232618*fabs(dram->bL[15]))));
			inputSampleL -= (dram->bL[16] * (0.48807302802822128  - (0.15506178974799034*fabs(dram->bL[16]))));
			inputSampleL -= (dram->bL[17] * (0.36545171501947982  + (0.16126103769376721*fabs(dram->bL[17]))));
			inputSampleL -= (dram->bL[18] * (0.31469581455759105  + (0.32250870039053953*fabs(dram->bL[18]))));
			inputSampleL -= (dram->bL[19] * (0.36893534817945800  + (0.25409418897237473*fabs(dram->bL[19]))));
			inputSampleL -= (dram->bL[20] * (0.41092557722975687  + (0.13114730488878301*fabs(dram->bL[20]))));
			inputSampleL -= (dram->bL[21] * (0.38584044480710594  + (0.06825323739722661*fabs(dram->bL[21]))));
			inputSampleL -= (dram->bL[22] * (0.33378434007178670  + (0.04144255489164217*fabs(dram->bL[22]))));
			inputSampleL -= (dram->bL[23] * (0.26144203061699706  + (0.06031313105098152*fabs(dram->bL[23]))));
			inputSampleL -= (dram->bL[24] * (0.25818342000920502  + (0.03642289242586355*fabs(dram->bL[24]))));
			inputSampleL -= (dram->bL[25] * (0.28096018498822661  + (0.00976973667327174*fabs(dram->bL[25]))));
			inputSampleL -= (dram->bL[26] * (0.25845682019095384  + (0.02749015358080831*fabs(dram->bL[26]))));
			inputSampleL -= (dram->bL[27] * (0.26655607865953096  - (0.00329839675455690*fabs(dram->bL[27]))));
			inputSampleL -= (dram->bL[28] * (0.30590085026938518  - (0.07375043215328811*fabs(dram->bL[28]))));
			inputSampleL -= (dram->bL[29] * (0.32875683916470899  - (0.12454134857516502*fabs(dram->bL[29]))));
			inputSampleL -= (dram->bL[30] * (0.38166643180506560  - (0.19973911428609989*fabs(dram->bL[30]))));
			inputSampleL -= (dram->bL[31] * (0.49068186937289598  - (0.34785166842136384*fabs(dram->bL[31]))));
			inputSampleL -= (dram->bL[32] * (0.60274753867622777  - (0.48685038872711034*fabs(dram->bL[32]))));
			inputSampleL -= (dram->bL[33] * (0.65944678627090636  - (0.49844657885975518*fabs(dram->bL[33]))));
			inputSampleL -= (dram->bL[34] * (0.64488955808717285  - (0.40514406499806987*fabs(dram->bL[34]))));
			inputSampleL -= (dram->bL[35] * (0.55818730353434354  - (0.28029870614987346*fabs(dram->bL[35]))));
			inputSampleL -= (dram->bL[36] * (0.43110859113387556  - (0.15373504582939335*fabs(dram->bL[36]))));
			inputSampleL -= (dram->bL[37] * (0.37726894966096269  - (0.11570983506028532*fabs(dram->bL[37]))));
			inputSampleL -= (dram->bL[38] * (0.39953242355200935  - (0.17879231130484088*fabs(dram->bL[38]))));
			inputSampleL -= (dram->bL[39] * (0.36726676379100875  - (0.22013553023983223*fabs(dram->bL[39]))));
			inputSampleL -= (dram->bL[40] * (0.27187029469227386  - (0.18461171768478427*fabs(dram->bL[40]))));
			inputSampleL -= (dram->bL[41] * (0.21109334552321635  - (0.14497481318083569*fabs(dram->bL[41]))));
			inputSampleL -= (dram->bL[42] * (0.19808797405293213  - (0.14916579928186940*fabs(dram->bL[42]))));
			inputSampleL -= (dram->bL[43] * (0.16287926785495671  - (0.15146098461120627*fabs(dram->bL[43]))));
			inputSampleL -= (dram->bL[44] * (0.11086621477163359  - (0.13182973443924018*fabs(dram->bL[44]))));
			inputSampleL -= (dram->bL[45] * (0.07531043236890560  - (0.08062172796472888*fabs(dram->bL[45]))));
			inputSampleL -= (dram->bL[46] * (0.01747364473230771  + (0.02201865873632456*fabs(dram->bL[46]))));
			inputSampleL += (dram->bL[47] * (0.03080279125662693  - (0.08721756240972101*fabs(dram->bL[47]))));
			inputSampleL += (dram->bL[48] * (0.02354148659185142  - (0.06376361763053796*fabs(dram->bL[48]))));
			inputSampleL -= (dram->bL[49] * (0.02835772372098715  + (0.00589978513642627*fabs(dram->bL[49]))));
			inputSampleL -= (dram->bL[50] * (0.08983370744565244  - (0.02350960427706536*fabs(dram->bL[50]))));
			inputSampleL -= (dram->bL[51] * (0.14148947620055380  - (0.03329826628693369*fabs(dram->bL[51]))));
			inputSampleL -= (dram->bL[52] * (0.17576502674572581  - (0.06507546651241880*fabs(dram->bL[52]))));
			inputSampleL -= (dram->bL[53] * (0.17168865666573860  - (0.07734801128437317*fabs(dram->bL[53]))));
			inputSampleL -= (dram->bL[54] * (0.14107027738292105  - (0.03136459344220402*fabs(dram->bL[54]))));
			inputSampleL -= (dram->bL[55] * (0.12287163395380074  + (0.01933408169185258*fabs(dram->bL[55]))));
			inputSampleL -= (dram->bL[56] * (0.12276622398112971  + (0.01983508766241737*fabs(dram->bL[56]))));
			inputSampleL -= (dram->bL[57] * (0.12349721440213673  - (0.01111031415304768*fabs(dram->bL[57]))));
			inputSampleL -= (dram->bL[58] * (0.08649454142716655  + (0.02252815645513927*fabs(dram->bL[58]))));
			inputSampleL -= (dram->bL[59] * (0.00953083685474757  + (0.13778878548343007*fabs(dram->bL[59]))));
			inputSampleL += (dram->bL[60] * (0.06045983158868478  - (0.23966318224935096*fabs(dram->bL[60]))));
			inputSampleL += (dram->bL[61] * (0.09053229817093242  - (0.27190119941572544*fabs(dram->bL[61]))));
			inputSampleL += (dram->bL[62] * (0.08112662178843048  - (0.22456862606452327*fabs(dram->bL[62]))));
			inputSampleL += (dram->bL[63] * (0.07503525686243730  - (0.14330154410548213*fabs(dram->bL[63]))));
			inputSampleL += (dram->bL[64] * (0.07372595404399729  - (0.06185193766408734*fabs(dram->bL[64]))));
			inputSampleL += (dram->bL[65] * (0.06073789200080433  + (0.01261857435786178*fabs(dram->bL[65]))));
			inputSampleL += (dram->bL[66] * (0.04616712695742254  + (0.05851771967084609*fabs(dram->bL[66]))));
			inputSampleL += (dram->bL[67] * (0.01036235510345900  + (0.08286534414423796*fabs(dram->bL[67]))));
			inputSampleL -= (dram->bL[68] * (0.03708389413229191  - (0.06695282381039531*fabs(dram->bL[68]))));
			inputSampleL -= (dram->bL[69] * (0.07092204876981217  - (0.01915829199112784*fabs(dram->bL[69]))));
			inputSampleL -= (dram->bL[70] * (0.09443579589460312  + (0.01210082455316246*fabs(dram->bL[70]))));
			inputSampleL -= (dram->bL[71] * (0.07824038577769601  + (0.06121988546065113*fabs(dram->bL[71]))));
			inputSampleL -= (dram->bL[72] * (0.00854730633079399  + (0.14468518752295506*fabs(dram->bL[72]))));
			inputSampleL += (dram->bL[73] * (0.06845589924191028  - (0.18902431382592944*fabs(dram->bL[73]))));
			inputSampleL += (dram->bL[74] * (0.10351569998375465  - (0.13204443060279647*fabs(dram->bL[74]))));
			inputSampleL += (dram->bL[75] * (0.10513368758532179  - (0.02993199294485649*fabs(dram->bL[75]))));
			inputSampleL += (dram->bL[76] * (0.08896978950235003  + (0.04074499273825906*fabs(dram->bL[76]))));
			inputSampleL += (dram->bL[77] * (0.03697537734050980  + (0.09217751130846838*fabs(dram->bL[77]))));
			inputSampleL -= (dram->bL[78] * (0.04014322441280276  - (0.14062297149365666*fabs(dram->bL[78]))));
			inputSampleL -= (dram->bL[79] * (0.10505934581398618  - (0.16988861157275814*fabs(dram->bL[79]))));
			inputSampleL -= (dram->bL[80] * (0.13937661651676272  - (0.15083294570551492*fabs(dram->bL[80]))));
			inputSampleL -= (dram->bL[81] * (0.13183458845108439  - (0.06657454442471208*fabs(dram->bL[81]))));
			
			temp = (inputSampleL + smoothCabBL)/3.0;
			smoothCabBL = inputSampleL;
			inputSampleL = temp/4.0;
			
			temp = (inputSampleR + smoothCabAR)/3.0;
			smoothCabAR = inputSampleR;
			inputSampleR = temp;
			
			dram->bR[82] = dram->bR[81]; dram->bR[81] = dram->bR[80]; dram->bR[80] = dram->bR[79]; 
			dram->bR[79] = dram->bR[78]; dram->bR[78] = dram->bR[77]; dram->bR[77] = dram->bR[76]; dram->bR[76] = dram->bR[75]; dram->bR[75] = dram->bR[74]; dram->bR[74] = dram->bR[73]; dram->bR[73] = dram->bR[72]; dram->bR[72] = dram->bR[71]; 
			dram->bR[71] = dram->bR[70]; dram->bR[70] = dram->bR[69]; dram->bR[69] = dram->bR[68]; dram->bR[68] = dram->bR[67]; dram->bR[67] = dram->bR[66]; dram->bR[66] = dram->bR[65]; dram->bR[65] = dram->bR[64]; dram->bR[64] = dram->bR[63]; 
			dram->bR[63] = dram->bR[62]; dram->bR[62] = dram->bR[61]; dram->bR[61] = dram->bR[60]; dram->bR[60] = dram->bR[59]; dram->bR[59] = dram->bR[58]; dram->bR[58] = dram->bR[57]; dram->bR[57] = dram->bR[56]; dram->bR[56] = dram->bR[55]; 
			dram->bR[55] = dram->bR[54]; dram->bR[54] = dram->bR[53]; dram->bR[53] = dram->bR[52]; dram->bR[52] = dram->bR[51]; dram->bR[51] = dram->bR[50]; dram->bR[50] = dram->bR[49]; dram->bR[49] = dram->bR[48]; dram->bR[48] = dram->bR[47]; 
			dram->bR[47] = dram->bR[46]; dram->bR[46] = dram->bR[45]; dram->bR[45] = dram->bR[44]; dram->bR[44] = dram->bR[43]; dram->bR[43] = dram->bR[42]; dram->bR[42] = dram->bR[41]; dram->bR[41] = dram->bR[40]; dram->bR[40] = dram->bR[39]; 
			dram->bR[39] = dram->bR[38]; dram->bR[38] = dram->bR[37]; dram->bR[37] = dram->bR[36]; dram->bR[36] = dram->bR[35]; dram->bR[35] = dram->bR[34]; dram->bR[34] = dram->bR[33]; dram->bR[33] = dram->bR[32]; dram->bR[32] = dram->bR[31]; 
			dram->bR[31] = dram->bR[30]; dram->bR[30] = dram->bR[29]; dram->bR[29] = dram->bR[28]; dram->bR[28] = dram->bR[27]; dram->bR[27] = dram->bR[26]; dram->bR[26] = dram->bR[25]; dram->bR[25] = dram->bR[24]; dram->bR[24] = dram->bR[23]; 
			dram->bR[23] = dram->bR[22]; dram->bR[22] = dram->bR[21]; dram->bR[21] = dram->bR[20]; dram->bR[20] = dram->bR[19]; dram->bR[19] = dram->bR[18]; dram->bR[18] = dram->bR[17]; dram->bR[17] = dram->bR[16]; dram->bR[16] = dram->bR[15]; 
			dram->bR[15] = dram->bR[14]; dram->bR[14] = dram->bR[13]; dram->bR[13] = dram->bR[12]; dram->bR[12] = dram->bR[11]; dram->bR[11] = dram->bR[10]; dram->bR[10] = dram->bR[9]; dram->bR[9] = dram->bR[8]; dram->bR[8] = dram->bR[7]; 
			dram->bR[7] = dram->bR[6]; dram->bR[6] = dram->bR[5]; dram->bR[5] = dram->bR[4]; dram->bR[4] = dram->bR[3]; dram->bR[3] = dram->bR[2]; dram->bR[2] = dram->bR[1]; dram->bR[1] = dram->bR[0]; dram->bR[0] = inputSampleR;
			inputSampleR += (dram->bR[1] * (1.42133070619855229  - (0.18270903813104500*fabs(dram->bR[1]))));
			inputSampleR += (dram->bR[2] * (1.47209686171873821  - (0.27954009590498585*fabs(dram->bR[2]))));
			inputSampleR += (dram->bR[3] * (1.34648011331265294  - (0.47178343556301960*fabs(dram->bR[3]))));
			inputSampleR += (dram->bR[4] * (0.82133804036124580  - (0.41060189990353935*fabs(dram->bR[4]))));
			inputSampleR += (dram->bR[5] * (0.21628057120466901  - (0.26062442734317454*fabs(dram->bR[5]))));
			inputSampleR -= (dram->bR[6] * (0.30306716082877883  + (0.10067648425439185*fabs(dram->bR[6]))));
			inputSampleR -= (dram->bR[7] * (0.69484313178531876  - (0.09655574841702286*fabs(dram->bR[7]))));
			inputSampleR -= (dram->bR[8] * (0.88320822356980833  - (0.26501644327144314*fabs(dram->bR[8]))));
			inputSampleR -= (dram->bR[9] * (0.81326147029423723  - (0.31115926837054075*fabs(dram->bR[9]))));
			inputSampleR -= (dram->bR[10] * (0.56728759049069222  - (0.23304233545561287*fabs(dram->bR[10]))));
			inputSampleR -= (dram->bR[11] * (0.33340326645198737  - (0.12361361388240180*fabs(dram->bR[11]))));
			inputSampleR -= (dram->bR[12] * (0.20280263733605616  - (0.03531960962500105*fabs(dram->bR[12]))));
			inputSampleR -= (dram->bR[13] * (0.15864533788751345  + (0.00355160825317868*fabs(dram->bR[13]))));
			inputSampleR -= (dram->bR[14] * (0.12544767480555119  + (0.01979010423176500*fabs(dram->bR[14]))));
			inputSampleR -= (dram->bR[15] * (0.06666788902658917  + (0.00188830739903378*fabs(dram->bR[15]))));
			inputSampleR += (dram->bR[16] * (0.02977793355081072  + (0.02304216615605394*fabs(dram->bR[16]))));
			inputSampleR += (dram->bR[17] * (0.12821526330916558  + (0.02636238376777800*fabs(dram->bR[17]))));
			inputSampleR += (dram->bR[18] * (0.19933812710210136  - (0.02932657234709721*fabs(dram->bR[18]))));
			inputSampleR += (dram->bR[19] * (0.18346460191225772  - (0.12859581955080629*fabs(dram->bR[19]))));
			inputSampleR -= (dram->bR[20] * (0.00088697526755385  + (0.15855257539277415*fabs(dram->bR[20]))));
			inputSampleR -= (dram->bR[21] * (0.28904286712096761  + (0.06226286786982616*fabs(dram->bR[21]))));
			inputSampleR -= (dram->bR[22] * (0.49133546282552537  - (0.06512851581813534*fabs(dram->bR[22]))));
			inputSampleR -= (dram->bR[23] * (0.52908013030763046  - (0.13606992188523465*fabs(dram->bR[23]))));
			inputSampleR -= (dram->bR[24] * (0.45897241332311706  - (0.15527194946346906*fabs(dram->bR[24]))));
			inputSampleR -= (dram->bR[25] * (0.35535938629924352  - (0.13634771941703441*fabs(dram->bR[25]))));
			inputSampleR -= (dram->bR[26] * (0.26185269405237693  - (0.08736651482771546*fabs(dram->bR[26]))));
			inputSampleR -= (dram->bR[27] * (0.19997351944186473  - (0.01714565029656306*fabs(dram->bR[27]))));
			inputSampleR -= (dram->bR[28] * (0.18894054145105646  + (0.04557612705740050*fabs(dram->bR[28]))));
			inputSampleR -= (dram->bR[29] * (0.24043993691153928  + (0.05267500387081067*fabs(dram->bR[29]))));
			inputSampleR -= (dram->bR[30] * (0.29191852873822671  + (0.01922151122971644*fabs(dram->bR[30]))));
			inputSampleR -= (dram->bR[31] * (0.29399783430587761  - (0.02238952856106585*fabs(dram->bR[31]))));
			inputSampleR -= (dram->bR[32] * (0.26662219155294159  - (0.07760819463416335*fabs(dram->bR[32]))));
			inputSampleR -= (dram->bR[33] * (0.20881206667122221  - (0.11930017354479640*fabs(dram->bR[33]))));
			inputSampleR -= (dram->bR[34] * (0.12916658879944876  - (0.11798638949823513*fabs(dram->bR[34]))));
			inputSampleR -= (dram->bR[35] * (0.07678815166012012  - (0.06826864734598684*fabs(dram->bR[35]))));
			inputSampleR -= (dram->bR[36] * (0.08568505484529348  - (0.00510459741104792*fabs(dram->bR[36]))));
			inputSampleR -= (dram->bR[37] * (0.13613615872486634  + (0.02288223583971244*fabs(dram->bR[37]))));
			inputSampleR -= (dram->bR[38] * (0.17426657494209266  + (0.02723737220296440*fabs(dram->bR[38]))));
			inputSampleR -= (dram->bR[39] * (0.17343619261009030  + (0.01412920547179825*fabs(dram->bR[39]))));
			inputSampleR -= (dram->bR[40] * (0.14548368977428555  - (0.02640418940455951*fabs(dram->bR[40]))));
			inputSampleR -= (dram->bR[41] * (0.10485295885802372  - (0.06334665781931498*fabs(dram->bR[41]))));
			inputSampleR -= (dram->bR[42] * (0.06632268974717079  - (0.05960343688612868*fabs(dram->bR[42]))));
			inputSampleR -= (dram->bR[43] * (0.06915692039882040  - (0.03541337869596061*fabs(dram->bR[43]))));
			inputSampleR -= (dram->bR[44] * (0.11889611687783583  - (0.02250608307287119*fabs(dram->bR[44]))));
			inputSampleR -= (dram->bR[45] * (0.14598456370320673  + (0.00280345943128246*fabs(dram->bR[45]))));
			inputSampleR -= (dram->bR[46] * (0.12312084125613143  + (0.04947283933434576*fabs(dram->bR[46]))));
			inputSampleR -= (dram->bR[47] * (0.11379940289994711  + (0.06590080966570636*fabs(dram->bR[47]))));
			inputSampleR -= (dram->bR[48] * (0.12963290754003182  + (0.02597647654256477*fabs(dram->bR[48]))));
			inputSampleR -= (dram->bR[49] * (0.12723837402978638  - (0.04942071966927938*fabs(dram->bR[49]))));
			inputSampleR -= (dram->bR[50] * (0.09185015882996231  - (0.10420810015956679*fabs(dram->bR[50]))));
			inputSampleR -= (dram->bR[51] * (0.04011592913036545  - (0.10234174227772008*fabs(dram->bR[51]))));
			inputSampleR += (dram->bR[52] * (0.00992597785057113  + (0.05674042373836896*fabs(dram->bR[52]))));
			inputSampleR += (dram->bR[53] * (0.04921452178306781  - (0.00222698867111080*fabs(dram->bR[53]))));
			inputSampleR += (dram->bR[54] * (0.06096504883783566  - (0.04040426549982253*fabs(dram->bR[54]))));
			inputSampleR += (dram->bR[55] * (0.04113530718724200  - (0.04190143593049960*fabs(dram->bR[55]))));
			inputSampleR += (dram->bR[56] * (0.01292699017654650  - (0.01121994018532499*fabs(dram->bR[56]))));
			inputSampleR -= (dram->bR[57] * (0.00437123132431870  - (0.02482497612289103*fabs(dram->bR[57]))));
			inputSampleR -= (dram->bR[58] * (0.02090571264211918  - (0.03732746039260295*fabs(dram->bR[58]))));
			inputSampleR -= (dram->bR[59] * (0.04749751678612051  - (0.02960060937328099*fabs(dram->bR[59]))));
			inputSampleR -= (dram->bR[60] * (0.07675095194206227  - (0.02241927084099648*fabs(dram->bR[60]))));
			inputSampleR -= (dram->bR[61] * (0.08879414028581609  - (0.01144281133042115*fabs(dram->bR[61]))));
			inputSampleR -= (dram->bR[62] * (0.07378854974999530  + (0.02518742701599147*fabs(dram->bR[62]))));
			inputSampleR -= (dram->bR[63] * (0.04677309194488959  + (0.08984657372223502*fabs(dram->bR[63]))));
			inputSampleR -= (dram->bR[64] * (0.02911874044176449  + (0.14202665940555093*fabs(dram->bR[64]))));
			inputSampleR -= (dram->bR[65] * (0.02103564720234969  + (0.14640411976171003*fabs(dram->bR[65]))));
			inputSampleR -= (dram->bR[66] * (0.01940626429101940  + (0.10867274382865903*fabs(dram->bR[66]))));
			inputSampleR -= (dram->bR[67] * (0.03965401793931531  + (0.04775225375522835*fabs(dram->bR[67]))));
			inputSampleR -= (dram->bR[68] * (0.08102486457314527  - (0.03204447425666343*fabs(dram->bR[68]))));
			inputSampleR -= (dram->bR[69] * (0.11794849372825778  - (0.12755667382696789*fabs(dram->bR[69]))));
			inputSampleR -= (dram->bR[70] * (0.11946469076758266  - (0.20151394599125422*fabs(dram->bR[70]))));
			inputSampleR -= (dram->bR[71] * (0.07404630324668053  - (0.21300634351769704*fabs(dram->bR[71]))));
			inputSampleR -= (dram->bR[72] * (0.00477584437144086  - (0.16864707684978708*fabs(dram->bR[72]))));
			inputSampleR += (dram->bR[73] * (0.05924822014377220  + (0.09394651445109450*fabs(dram->bR[73]))));
			inputSampleR += (dram->bR[74] * (0.10060989907457370  + (0.00419196431884887*fabs(dram->bR[74]))));
			inputSampleR += (dram->bR[75] * (0.10817907203844988  - (0.07459664480796091*fabs(dram->bR[75]))));
			inputSampleR += (dram->bR[76] * (0.08701102204768002  - (0.11129477437630560*fabs(dram->bR[76]))));
			inputSampleR += (dram->bR[77] * (0.05673785623180162  - (0.10638640242375266*fabs(dram->bR[77]))));
			inputSampleR += (dram->bR[78] * (0.02944190197442081  - (0.08499792583420167*fabs(dram->bR[78]))));
			inputSampleR += (dram->bR[79] * (0.01570145445652971  - (0.06190456843465320*fabs(dram->bR[79]))));
			inputSampleR += (dram->bR[80] * (0.02770233032476748  - (0.04573713136865480*fabs(dram->bR[80]))));
			inputSampleR += (dram->bR[81] * (0.05417160459175360  - (0.03965651064634598*fabs(dram->bR[81]))));
			inputSampleR += (dram->bR[82] * (0.06080831637644498  - (0.02909500789113911*fabs(dram->bR[82]))));
			
			temp = (inputSampleR + smoothCabBR)/3.0;
			smoothCabBR = inputSampleR;
			inputSampleR = temp/4.0;
			
			if (cycleEnd == 4) {
				lastRefL[0] = lastRefL[4]; //start from previous last
				lastRefL[2] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[1] = (lastRefL[0] + lastRefL[2])/2; //one quarter
				lastRefL[3] = (lastRefL[2] + inputSampleL)/2; //three quarters
				lastRefL[4] = inputSampleL; //full
				lastRefR[0] = lastRefR[4]; //start from previous last
				lastRefR[2] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[1] = (lastRefR[0] + lastRefR[2])/2; //one quarter
				lastRefR[3] = (lastRefR[2] + inputSampleR)/2; //three quarters
				lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				lastRefL[0] = lastRefL[3]; //start from previous last
				lastRefL[2] = (lastRefL[0]+lastRefL[0]+inputSampleL)/3; //third
				lastRefL[1] = (lastRefL[0]+inputSampleL+inputSampleL)/3; //two thirds
				lastRefL[3] = inputSampleL; //full
				lastRefR[0] = lastRefR[3]; //start from previous last
				lastRefR[2] = (lastRefR[0]+lastRefR[0]+inputSampleR)/3; //third
				lastRefR[1] = (lastRefR[0]+inputSampleR+inputSampleR)/3; //two thirds
				lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				lastRefL[0] = lastRefL[2]; //start from previous last
				lastRefL[1] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[2] = inputSampleL; //full
				lastRefR[0] = lastRefR[2]; //start from previous last
				lastRefR[1] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				lastRefL[0] = inputSampleL;
				lastRefR[0] = inputSampleR;
			}
			cycle = 0; //reset
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
		} else {
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables on BigAmp channel only
		{
			case 4:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[7])*0.5;
				lastRefL[7] = lastRefL[8]; //continue, do not break
			case 3:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[6])*0.5;
				lastRefL[6] = lastRefL[8]; //continue, do not break
			case 2:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[5])*0.5;
				lastRefL[5] = lastRefL[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		inputSampleR = (inputSampleL*0.5)+(inputSampleR*0.5);
		
		if (inputSampleR > 4.0) inputSampleR = 4.0; if (inputSampleR < -4.0) inputSampleR = -4.0;
		if (wasPosClipR == true) { //current will be over
			if (inputSampleR<lastSampleR) lastSampleR=0.7058208+(inputSampleR*0.2609148);
			else lastSampleR = 0.2491717+(lastSampleR*0.7390851);
		} wasPosClipR = false;
		if (inputSampleR>0.9549925859) {wasPosClipR=true;inputSampleR=0.7058208+(lastSampleR*0.2609148);}
		if (wasNegClipR == true) { //current will be -over
			if (inputSampleR > lastSampleR) lastSampleR=-0.7058208+(inputSampleR*0.2609148);
			else lastSampleR=-0.2491717+(lastSampleR*0.7390851);
		} wasNegClipR = false;
		if (inputSampleR<-0.9549925859) {wasNegClipR=true;inputSampleR=-0.7058208+(lastSampleR*0.2609148);}
		intermediateR[cycleEnd] = inputSampleR;
        inputSampleR = lastSampleR; //Latency is however many samples equals one 44.1k sample
		for (int x = cycleEnd; x > 0; x--) intermediateR[x-1] = intermediateR[x];
		lastSampleR = intermediateR[0]; //run a little buffer to handle this
		
		inputSampleL = inputSampleR;
		
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
	
	lastASampleL = 0.0;
	lastSlewL = 0.0;
	iirSampleAL = 0.0;
	iirSampleBL = 0.0;
	iirSampleCL = 0.0;
	iirSampleDL = 0.0;
	iirSampleEL = 0.0;
	iirSampleFL = 0.0;
	iirSampleGL = 0.0;
	iirSampleHL = 0.0;
	iirSampleIL = 0.0;
	iirSampleJL = 0.0;
	
	lastASampleR = 0.0;
	lastSlewR = 0.0;
	iirSampleAR = 0.0;
	iirSampleBR = 0.0;
	iirSampleCR = 0.0;
	iirSampleDR = 0.0;
	iirSampleER = 0.0;
	OddAR = 0.0;
	OddBR = 0.0;
	OddCR = 0.0;
	OddDR = 0.0;
	OddER = 0.0;
	EvenAR = 0.0;
	EvenBR = 0.0;
	EvenCR = 0.0;
	EvenDR = 0.0;
	EvenER = 0.0;
	
	for (int fcount = 0; fcount < 257; fcount++) {
		dram->OddL[fcount] = 0.0;
		dram->EvenL[fcount] = 0.0;
	}
	
	count = 0;
	flip = false; //amp
	
	for(int fcount = 0; fcount < 90; fcount++) {
		dram->bL[fcount] = 0;
		dram->bR[fcount] = 0;
	}
	smoothCabAL = 0.0; smoothCabBL = 0.0; lastCabSampleL = 0.0; //cab
	smoothCabAR = 0.0; smoothCabBR = 0.0; lastCabSampleR = 0.0; //cab
	
	for (int fcount = 0; fcount < 9; fcount++) {
		lastRefL[fcount] = 0.0;
		lastRefR[fcount] = 0.0;
	}
	cycle = 0; //undersampling
	
	for (int x = 0; x < fix_total; x++) {
		fixA[x] = 0.0;
		fixB[x] = 0.0;
		fixC[x] = 0.0;
		fixD[x] = 0.0;
		fixE[x] = 0.0;
		fixF[x] = 0.0;
	}	//filtering
	
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 16; x++) intermediateR[x] = 0.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
