#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PodcastDeluxe"
#define AIRWINDOWS_DESCRIPTION "A pile of compressors (curve style) and phase rotators."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','f' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Boost", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int tap1, tap2, tap3, tap4, tap5, maxdelay1, maxdelay2, maxdelay3, maxdelay4, maxdelay5;
		//the phase rotator
		
		Float64 c1;
		Float64 c2;
		Float64 c3;
		Float64 c4;
		Float64 c5;
		//the compressor
				
		Float64 lastSample;
		Float64 lastOutSample;

		uint32_t fpd;
	
	struct _dram {
			Float64 d1[503];
		Float64 d2[503];
		Float64 d3[503];
		Float64 d4[503];
		Float64 d5[503];
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
	
	int allpasstemp;
	Float64 outallpass = 0.618033988749894848204586;
	
	Float64 compress = 1.0+pow(GetParameter( kParam_One )*0.8,2);
	
	Float64 speed1 = 128.0 / pow(compress,2);
	speed1 *= overallscale;
	Float64 speed2 = speed1 * 1.4;
	Float64 speed3 = speed2 * 1.5;
	Float64 speed4 = speed3 * 1.6;
	Float64 speed5 = speed4 * 1.7;
	
	maxdelay1 = (int)(23.0*overallscale);
	maxdelay2 = (int)(19.0*overallscale);
	maxdelay3 = (int)(17.0*overallscale);
	maxdelay4 = (int)(13.0*overallscale);
	maxdelay5 = (int)(11.0*overallscale);
	//set up the prime delays
	
	Float64 refclip = 0.999;
	Float64 softness = 0.435;
	Float64 invsoft = 0.56;
	Float64 outsoft = 0.545;
	Float64 trigger;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
								
		allpasstemp = tap1 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay1) {allpasstemp = maxdelay1;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap		
		inputSample -= dram->d1[allpasstemp]*outallpass;
		dram->d1[tap1] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d1[allpasstemp]);
		//allpass stage
		tap1--; if (tap1 < 0 || tap1 > maxdelay1) {tap1 = maxdelay1;}
		//decrement the position for reals
		
		inputSample *= c1;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c1 += trigger/speed5;
		if (c1 > compress) c1 = compress;
		//compress stage
				
		allpasstemp = tap2 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay2) {allpasstemp = maxdelay2;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d2[allpasstemp]*outallpass;
		dram->d2[tap2] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d2[allpasstemp]);
		//allpass stage
		tap2--; if (tap2 < 0 || tap2 > maxdelay2) {tap2 = maxdelay2;}
		//decrement the position for reals
		
		inputSample *= c2;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c2 += trigger/speed4;
		if (c2 > compress) c2 = compress;
		//compress stage
				
		allpasstemp = tap3 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay3) {allpasstemp = maxdelay3;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d3[allpasstemp]*outallpass;
		dram->d3[tap3] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d3[allpasstemp]);
		//allpass stage
		tap3--; if (tap3 < 0 || tap3 > maxdelay3) {tap3 = maxdelay3;}
		//decrement the position for reals
		
		inputSample *= c3;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c3 += trigger/speed3;
		if (c3 > compress) c3 = compress;
		//compress stage
				
		allpasstemp = tap4 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay4) {allpasstemp = maxdelay4;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d4[allpasstemp]*outallpass;
		dram->d4[tap4] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d4[allpasstemp]);
		//allpass stage
		tap4--; if (tap4 < 0 || tap4 > maxdelay4) {tap4 = maxdelay4;}
		//decrement the position for reals
		
		inputSample *= c4;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c4 += trigger/speed2;
		if (c4 > compress) c4 = compress;
		//compress stage
				
		allpasstemp = tap5 - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay5) {allpasstemp = maxdelay5;}
		//set up the delay position
		//using 'tap' and 'allpasstemp' to position the tap
		inputSample -= dram->d5[allpasstemp]*outallpass;
		dram->d5[tap5] = inputSample;
		inputSample *= outallpass;
		inputSample += (dram->d5[allpasstemp]);
		//allpass stage
		tap5--; if (tap5 < 0 || tap5 > maxdelay5) {tap5 = maxdelay5;}
		//decrement the position for reals
		
		inputSample *= c5;
		trigger = fabs(inputSample)*4.7;
		if (trigger > 4.7) trigger = 4.7;
		trigger = sin(trigger);
		if (trigger < 0) trigger *= 8.0;
		if (trigger < -4.2) trigger = -4.2;
		c5 += trigger/speed1;
		if (c5 > compress) c5 = compress;
		//compress stage
		
		if (compress > 1.0) inputSample /= compress;
		
		if (lastSample >= refclip)
		{
			if (inputSample < refclip)
			{
				lastSample = (outsoft + (inputSample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			if (inputSample > -refclip)
			{
				lastSample = (-outsoft + (inputSample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (inputSample > refclip)
		{
			if (lastSample < refclip)
			{
				inputSample = (invsoft + (lastSample * softness));
			}
			else inputSample = refclip;
		}
		
		if (inputSample < -refclip)
		{
			if (lastSample > -refclip)
			{
				inputSample = (-invsoft + (lastSample * softness));
			}
			else inputSample = -refclip;
		}

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
	for(int count = 0; count < 502; count++) {dram->d1[count] = 0.0; dram->d2[count] = 0.0; dram->d3[count] = 0.0; dram->d4[count] = 0.0; dram->d5[count] = 0.0;}
	tap1 = 1; tap2 = 1; tap3 = 1; tap4 = 1; tap5 = 1;
	maxdelay1 = 9001; maxdelay2 = 9001; maxdelay3 = 9001; maxdelay4 = 9001; maxdelay5 = 9001;
	c1 = 2.0; c2 = 2.0; c3 = 2.0; c4 = 2.0; c5 = 2.0; //startup comp gains
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
