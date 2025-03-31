#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BeziCompMono"
#define AIRWINDOWS_DESCRIPTION "BeziCompMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','e','#' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Comp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			bez_AL,
			bez_BL,
			bez_CL,
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		
		double lastSampleL;
		double intermediateL[16];
		bool wasPosClipL;
		bool wasNegClipL;

		uint32_t fpd;
	
	struct _dram {
			double bezComp[bez_total];
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;

	double bezCThresh = pow(GetParameter( kParam_A ),2.0) * 64.0;
	double bezMakeUp = sqrt(bezCThresh+1.0);
	double bezRez = (pow(GetParameter( kParam_B ),6.0)+0.0001)/overallscale; if (bezRez > 1.0) bezRez = 1.0;
	double wet = GetParameter( kParam_C );
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpd * 1.18e-17;
		double drySampleL = inputSampleL;
		
		dram->bezComp[bez_cycle] += bezRez;
		dram->bezComp[bez_SampL] += (fabs(inputSampleL) * bezRez);
		
		if (dram->bezComp[bez_cycle] > 1.0) {
			dram->bezComp[bez_cycle] -= 1.0;
			dram->bezComp[bez_CL] = dram->bezComp[bez_BL];
			dram->bezComp[bez_BL] = dram->bezComp[bez_AL];
			dram->bezComp[bez_AL] = dram->bezComp[bez_SampL];
			dram->bezComp[bez_SampL] = 0.0;
		}
		double CBL = (dram->bezComp[bez_CL]*(1.0-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_BL]*dram->bezComp[bez_cycle]);
		double BAL = (dram->bezComp[bez_BL]*(1.0-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_AL]*dram->bezComp[bez_cycle]);
		double CBAL = (dram->bezComp[bez_BL]+(CBL*(1.0-dram->bezComp[bez_cycle]))+(BAL*dram->bezComp[bez_cycle]))*0.5;
		inputSampleL *= 1.0-(fmin(CBAL*bezCThresh,1.0));
		inputSampleL *= bezMakeUp;
		
		if (wet < 1.0) inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0-wet));
		
		//begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
		if (inputSampleL > 4.0) inputSampleL = 4.0; if (inputSampleL < -4.0) inputSampleL = -4.0;
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=0.7058208+(inputSampleL*0.2609148);
			else lastSampleL = 0.2491717+(lastSampleL*0.7390851);
		} wasPosClipL = false;
		if (inputSampleL>0.9549925859) {wasPosClipL=true;inputSampleL=0.7058208+(lastSampleL*0.2609148);}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=-0.7058208+(inputSampleL*0.2609148);
			else lastSampleL=-0.2491717+(lastSampleL*0.7390851);
		} wasNegClipL = false;
		if (inputSampleL<-0.9549925859) {wasNegClipL=true;inputSampleL=-0.7058208+(lastSampleL*0.2609148);}
		intermediateL[spacing] = inputSampleL;
        inputSampleL = lastSampleL; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediateL[x-1] = intermediateL[x];
		lastSampleL = intermediateL[0]; //run a little buffer to handle this
		//end ClipOnly2 mono as a little, compressed chunk that can be dropped into code*/
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSampleL += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {dram->bezComp[x] = 0.0;}
	dram->bezComp[bez_cycle] = 1.0;

	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	for (int x = 0; x < 16; x++) {intermediateL[x] = 0.0;}

	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
