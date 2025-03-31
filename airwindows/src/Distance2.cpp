#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Distance2"
#define AIRWINDOWS_DESCRIPTION "A versatile space shaper for creating depth."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','v' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Atmosphere", .min = 0, .max = 1000, .def = 850, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darken", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		double lastSampleA;
		double lastSampleB;
		double lastSampleC;
		double lastSampleD;
		double lastSampleE;
		double lastSampleF;
		double lastSampleG;
		double lastSampleH;
		double lastSampleI;
		double lastSampleJ;
		double lastSampleK;
		double lastSampleL;
		double lastSampleM;
		double thresholdA;
		double thresholdB;
		double thresholdC;
		double thresholdD;
		double thresholdE;
		double thresholdF;
		double thresholdG;
		double thresholdH;
		double thresholdI;
		double thresholdJ;
		double thresholdK;
		double thresholdL;
		double thresholdM;
		
		Float64 thirdSample;
		Float64 lastSample;
		
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	thresholdA = 0.618033988749894 / overallscale;
	thresholdB = 0.679837387624884 / overallscale;
	thresholdC = 0.747821126387373 / overallscale;
	thresholdD = 0.82260323902611 / overallscale;
	thresholdE = 0.904863562928721 / overallscale;
	thresholdF = 0.995349919221593 / overallscale;
	thresholdG = 1.094884911143752 / overallscale;
	thresholdH = 1.204373402258128 / overallscale;
	thresholdI = 1.32481074248394 / overallscale;
	thresholdJ = 1.457291816732335 / overallscale;
	thresholdK = 1.603020998405568 / overallscale;
	thresholdL = 1.763323098246125 / overallscale;
	thresholdM = 1.939655408070737 / overallscale;	
	Float64 softslew = (pow(GetParameter( kParam_One ),3)*24)+.6;
	softslew *= overallscale;
	Float64 filter = softslew * GetParameter( kParam_Two );
	Float64 secondfilter = filter / 3.0;
	Float64 thirdfilter = filter / 5.0;
	Float64 offsetScale = GetParameter( kParam_One ) * 0.1618;
	Float64 levelcorrect = 1.0 + ((filter / 12.0) * GetParameter( kParam_One ));
	//bring in top slider again to manage boost level for lower settings
	Float64 wet = GetParameter( kParam_Three );
	//this also functions as a pad for the intensely distorty deep distance mode

	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		double offset = offsetScale - (lastSample - inputSample);		
		inputSample += (offset*offsetScale); //extra bit from Loud: offset air compression
		inputSample *= wet; //clean up w. dry introduced
		inputSample *= softslew; //scale into Atmosphere algorithm
		
		double clamp = inputSample - lastSampleA;
		if (clamp > thresholdA) inputSample = lastSampleA + thresholdA;
		if (-clamp > thresholdA) inputSample = lastSampleA - thresholdA;
		
		clamp = inputSample - lastSampleB;
		if (clamp > thresholdB) inputSample = lastSampleB + thresholdB;
		if (-clamp > thresholdB) inputSample = lastSampleB - thresholdB;
		
		clamp = inputSample - lastSampleC;
		if (clamp > thresholdC) inputSample = lastSampleC + thresholdC;
		if (-clamp > thresholdC) inputSample = lastSampleC - thresholdC;
		
		clamp = inputSample - lastSampleD;
		if (clamp > thresholdD) inputSample = lastSampleD + thresholdD;
		if (-clamp > thresholdD) inputSample = lastSampleD - thresholdD;
		
		clamp = inputSample - lastSampleE;
		if (clamp > thresholdE) inputSample = lastSampleE + thresholdE;
		if (-clamp > thresholdE) inputSample = lastSampleE - thresholdE;
		
		clamp = inputSample - lastSampleF;
		if (clamp > thresholdF) inputSample = lastSampleF + thresholdF;
		if (-clamp > thresholdF) inputSample = lastSampleF - thresholdF;
		
		clamp = inputSample - lastSampleG;
		if (clamp > thresholdG) inputSample = lastSampleG + thresholdG;
		if (-clamp > thresholdG) inputSample = lastSampleG - thresholdG;
		
		clamp = inputSample - lastSampleH;
		if (clamp > thresholdH) inputSample = lastSampleH + thresholdH;
		if (-clamp > thresholdH) inputSample = lastSampleH - thresholdH;
		
		clamp = inputSample - lastSampleI;
		if (clamp > thresholdI) inputSample = lastSampleI + thresholdI;
		if (-clamp > thresholdI) inputSample = lastSampleI - thresholdI;
		
		clamp = inputSample - lastSampleJ;
		if (clamp > thresholdJ) inputSample = lastSampleJ + thresholdJ;
		if (-clamp > thresholdJ) inputSample = lastSampleJ - thresholdJ;
		
		clamp = inputSample - lastSampleK;
		if (clamp > thresholdK) inputSample = lastSampleK + thresholdK;
		if (-clamp > thresholdK) inputSample = lastSampleK - thresholdK;
		
		clamp = inputSample - lastSampleL;
		if (clamp > thresholdL) inputSample = lastSampleL + thresholdL;
		if (-clamp > thresholdL) inputSample = lastSampleL - thresholdL;
		
		clamp = inputSample - lastSampleM;
		if (clamp > thresholdM) inputSample = lastSampleM + thresholdM;
		if (-clamp > thresholdM) inputSample = lastSampleM - thresholdM;
		
		
		lastSampleM = lastSampleL;
		lastSampleL = lastSampleK;
		lastSampleK = lastSampleJ;
		lastSampleJ = lastSampleI;
		lastSampleI = lastSampleH;
		lastSampleH = lastSampleG;
		lastSampleG = lastSampleF;
		lastSampleF = lastSampleE;
		lastSampleE = lastSampleD;
		lastSampleD = lastSampleC;
		lastSampleC = lastSampleB;
		lastSampleB = lastSampleA;
		lastSampleA = drySample;
		//store the raw input sample again for use next time
		
		inputSample *= levelcorrect;
		inputSample /= softslew;
		inputSample -= (offset*offsetScale);
		//begin IIR stage
		inputSample += (thirdSample * thirdfilter);
		inputSample /= (thirdfilter + 1.0);
		inputSample += (lastSample * secondfilter);
		inputSample /= (secondfilter + 1.0);
		//do an IIR like thing to further squish superdistant stuff
		thirdSample = lastSample;
		lastSample = inputSample;		
		inputSample *= levelcorrect;
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		//Dry/Wet control, defaults to the last slider

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
	thirdSample = lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	lastSampleA = 0.0;
	lastSampleB = 0.0;
	lastSampleC = 0.0;
	lastSampleD = 0.0;
	lastSampleE = 0.0;
	lastSampleF = 0.0;
	lastSampleG = 0.0;
	lastSampleH = 0.0;
	lastSampleI = 0.0;
	lastSampleJ = 0.0;
	lastSampleK = 0.0;
	lastSampleL = 0.0;
	lastSampleM = 0.0;
	thresholdA = 0.618033988749894;
	thresholdB = 0.679837387624884;
	thresholdC = 0.747821126387373;
	thresholdD = 0.82260323902611;
	thresholdE = 0.904863562928721;
	thresholdF = 0.995349919221593;
	thresholdG = 1.094884911143752;
	thresholdH = 1.204373402258128;
	thresholdI = 1.32481074248394;
	thresholdJ = 1.457291816732335;
	thresholdK = 1.603020998405568;
	thresholdL = 1.763323098246125;
	thresholdM = 1.939655408070737;
}
};
