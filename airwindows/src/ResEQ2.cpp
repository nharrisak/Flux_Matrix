#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ResEQ2"
#define AIRWINDOWS_DESCRIPTION "A single, sharp, sonorous mid peak."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','e','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "MSweep", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MBoost", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		double mpk[2005];
		double f[66];
		double prevfreqMPeak;
		double prevamountMPeak;
		int mpc;
		
		uint32_t fpd;
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
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	
	//begin ResEQ2 Mid Boost
	double freqMPeak = pow(GetParameter( kParam_One )+0.15,3);
	double amountMPeak = pow(GetParameter( kParam_Two ),2);
	int maxMPeak = (amountMPeak*63.0)+1;
	if ((freqMPeak != prevfreqMPeak)||(amountMPeak != prevamountMPeak)) {
	for (int x = 0; x < maxMPeak; x++) {
		if (((double)x*freqMPeak) < M_PI_4) f[x] = sin(((double)x*freqMPeak)*4.0)*freqMPeak*sin(((double)(maxMPeak-x)/(double)maxMPeak)*M_PI_2);
		else f[x] = cos((double)x*freqMPeak)*freqMPeak*sin(((double)(maxMPeak-x)/(double)maxMPeak)*M_PI_2);
	}
	prevfreqMPeak = freqMPeak; prevamountMPeak = amountMPeak;
	}//end ResEQ2 Mid Boost
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		//begin ResEQ2 Mid Boost
		mpc++; if (mpc < 1 || mpc > 2001) mpc = 1;
		mpk[mpc] = inputSample;
		double midMPeak = 0.0;
		for (int x = 0; x < maxMPeak; x++) {
			int y = x*cycleEnd;
			switch (cycleEnd)
			{
				case 1: 
					midMPeak += (mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x]); break;
				case 2: 
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.5); y--;
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.5); break;
				case 3: 
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.333); y--;
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.333); y--;
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.333); break;
				case 4: 
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.25); y--;
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.25); y--;
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.25); y--;
					midMPeak += ((mpk[(mpc-y)+((mpc-y < 1)?2001:0)] * f[x])*0.25); //break
			}
		}
		inputSample = (midMPeak*amountMPeak)+((1.5-amountMPeak>1.0)?inputSample:inputSample*(1.5-amountMPeak));
		//end ResEQ2 Mid Boost
	
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 2004; count++) {mpk[count] = 0.0;}
	for(int count = 0; count < 65; count++) {f[count] = 0.0;}
	prevfreqMPeak = -1;
	prevamountMPeak = -1;
	mpc = 1;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
