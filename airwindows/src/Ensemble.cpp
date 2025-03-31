#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Ensemble"
#define AIRWINDOWS_DESCRIPTION "A weird flangey little modulation effect."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','n','s' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Ensemble", .min = 200, .max = 4800, .def = 800, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Fullness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Brighten", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		const static int totalsamples = 65540;
		Float32 p[totalsamples];
		int gcount;
		Float64 airPrev;
		Float64 airEven;
		Float64 airOdd;
		Float64 airFactor;
		uint32_t fpd;
		bool fpFlip;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 sweep[49];
};
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
	
	Float64 spd = pow(0.4+(GetParameter( kParam_Two )/12),10);
	spd *= overallscale;
	Float64 depth = 0.002 / spd;
	Float64 tupi = 3.141592653589793238 * 2.0;
	Float64 taps = GetParameter( kParam_One );
	Float64 brighten = GetParameter( kParam_Three );
	Float64 wet = GetParameter( kParam_Four );
	//removed unnecessary dry variable
	Float64 hapi = 3.141592653589793238 / taps;
	Float64 offset;
	Float64 floffset;
	Float64 start[49];
	Float64 sinoffset[49];	
	Float64 speed[49];
	int count;
	int ensemble;
	Float64 temp;
	double inputSample;
	Float64 drySample;
	//now we'll precalculate some stuff that needn't be in every sample
	
	for(count = 1; count <= taps; count++)
		{
		start[count] = depth * count;
		sinoffset[count] = hapi * (count-1);
		speed[count] = spd / (1 + (count/taps));
		}
	//that's for speeding up things in the sample-processing area
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;

		airFactor = airPrev - inputSample;
		if (fpFlip) {airEven += airFactor; airOdd -= airFactor; airFactor = airEven;}
		else {airOdd += airFactor; airEven -= airFactor; airFactor = airOdd;}
		airOdd = (airOdd - ((airOdd - airEven)/256.0)) / 1.0001;
		airEven = (airEven - ((airEven - airOdd)/256.0)) / 1.0001;
		airPrev = inputSample;
		inputSample += (airFactor*brighten);
		//air, compensates for loss of highs in flanger's interpolation
		if (gcount < 1 || gcount > 32767) {gcount = 32767;}
		count = gcount;
		p[count+32767] = p[count] = temp = inputSample;
		//double buffer

		for(ensemble = 1; ensemble <= taps; ensemble++)
			{
				offset = start[ensemble] + (depth * sin(dram->sweep[ensemble]+sinoffset[ensemble]));
				floffset = offset-floor(offset);
				count = gcount + (int)floor(offset);
				temp += p[count] * (1-floffset); //less as value moves away from .0
				temp += p[count+1]; //we can assume always using this in one way or another?
				temp += p[count+2] * floffset; //greater as value moves away from .0
				temp -= ((p[count]-p[count+1])-(p[count+1]-p[count+2]))/50; //interpolation hacks 'r us
				dram->sweep[ensemble] += speed[ensemble];
				if (dram->sweep[ensemble] > tupi){dram->sweep[ensemble] -= tupi;}
			}
		gcount--;
		//still scrolling through the samples, remember
		
		inputSample = temp/(4.0*sqrt(taps));


		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		fpFlip = !fpFlip;
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		destP += inNumChannels;
		sourceP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 65539; count++) {p[count] = 0;}
	for(int count = 0; count < 49; count++) {dram->sweep[count] = 3.141592653589793238 / 2.0;}
	gcount = 0;
	airPrev = 0.0;
	airEven = 0.0;
	airOdd = 0.0;
	airFactor = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
