#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hull2"
#define AIRWINDOWS_DESCRIPTION "A very clear three-band EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','u','m' )
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
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;

		int hullp;
		double hullb[5];
		
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		double hull[225];	
};
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
	//max out at 4x, 192k
	
	double treble = GetParameter( kParam_One )*2.0;
	double mid = GetParameter( kParam_Two )*2.0;
	double bass = GetParameter( kParam_Three )*2.0;
	double iirAmount = 0.125/cycleEnd;
	int limit = 4*cycleEnd;
	double divisor = 2.0/limit;

	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		//begin Hull2 treble crossover
		hullp--; if (hullp < 0) hullp += 60;
		dram->hull[hullp] = dram->hull[hullp+60] = inputSample;
		int x = hullp;
		double midSample = 0.0;
		while (x < hullp+(limit/2)) {
			midSample += dram->hull[x] * divisor;
			x++;
		}
		midSample += midSample * 0.125;
		while (x < hullp+limit) {
			midSample -= dram->hull[x] * 0.125 * divisor;
			x++;
		}
		dram->hull[hullp+20] = dram->hull[hullp+80] = midSample;
		x = hullp+20;
		midSample = 0.0;
		while (x < hullp+20+(limit/2)) {
			midSample += dram->hull[x] * divisor;
			x++;
		}
		midSample += midSample * 0.125;
		while (x < hullp+20+limit) {
			midSample -= dram->hull[x] * 0.125 * divisor;
			x++;
		}
		dram->hull[hullp+40] = dram->hull[hullp+100] = midSample;
		x = hullp+40;
		midSample = 0.0;
		while (x < hullp+40+(limit/2)) {
			midSample += dram->hull[x] * divisor;
			x++;
		}
		midSample += midSample * 0.125;
		while (x < hullp+40+limit) {
			midSample -= dram->hull[x] * 0.125 * divisor;
			x++;
		}
		double trebleSample = drySample - midSample;
		//end Hull2 treble crossover
		
		//begin Hull2 midbass crossover
		double bassSample = midSample; x = 0;
		while (x < 3) {
			hullb[x] = (hullb[x] * (1.0 - iirAmount)) + (bassSample * iirAmount);
			if (fabs(hullb[x])<1.18e-37) hullb[x] = 0.0;
			bassSample = hullb[x];
			x++;
		}
		midSample -= bassSample;
		//end Hull2 midbass crossover
		
		inputSample = (bassSample * bass) + (midSample * mid) + (trebleSample * treble);
		
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
	for(int count = 0; count < 222; count++) {dram->hull[count] = 0.0;}
	for(int count = 0; count < 4; count++) {hullb[count] = 0.0;}
	hullp = 1;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
