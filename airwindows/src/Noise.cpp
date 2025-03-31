#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Noise"
#define AIRWINDOWS_DESCRIPTION "The Airwindows deep noise oscillator, as a sound reinforcer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','o','i' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "High Cutoff", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low Cutoff", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low Shape", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Decay", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Distance", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 noiseA;
		Float64 noiseB;
		Float64 noiseC;
		Float64 rumbleA;
		Float64 rumbleB;
		Float64 surge;
		int position;
		int quadratic;
		bool flip;
		bool filterflip;
		Float64 b[11];
		Float64 f[11];		
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
	Float64 cutoff;
	Float64 cutofftarget = (GetParameter( kParam_One )*3.5);
	Float64 rumblecutoff = cutofftarget * 0.005;
	Float64 invcutoff;
	double inputSample;
	Float64 drySample;
	Float64 highpass = GetParameter( kParam_Three )*38.0;
	int lowcut = floor(highpass);
	int dcut;
	if (lowcut > 37) {dcut= 1151;}
	if (lowcut == 37) {dcut= 1091;}
	if (lowcut == 36) {dcut= 1087;}
	if (lowcut == 35) {dcut= 1031;}
	if (lowcut == 34) {dcut= 1013;}
	if (lowcut == 33) {dcut= 971;}
	if (lowcut == 32) {dcut= 907;}
	if (lowcut == 31) {dcut= 839;}
	if (lowcut == 30) {dcut= 797;}
	if (lowcut == 29) {dcut= 733;}
	if (lowcut == 28) {dcut= 719;}
	if (lowcut == 27) {dcut= 673;}
	if (lowcut == 26) {dcut= 613;}
	if (lowcut == 25) {dcut= 593;}
	if (lowcut == 24) {dcut= 541;}
	if (lowcut == 23) {dcut= 479;}
	if (lowcut == 22) {dcut= 431;}
	if (lowcut == 21) {dcut= 419;}
	if (lowcut == 20) {dcut= 373;}
	if (lowcut == 19) {dcut= 311;}
	if (lowcut == 18) {dcut= 293;}
	if (lowcut == 17) {dcut= 233;}
	if (lowcut == 16) {dcut= 191;}
	if (lowcut == 15) {dcut= 173;}
	if (lowcut == 14) {dcut= 131;}
	if (lowcut == 13) {dcut= 113;}
	if (lowcut == 12) {dcut= 71;}
	if (lowcut == 11) {dcut= 53;}
	if (lowcut == 10) {dcut= 31;}
	if (lowcut == 9) {dcut= 27;}
	if (lowcut == 8) {dcut= 23;}
	if (lowcut == 7) {dcut= 19;}
	if (lowcut == 6) {dcut= 17;}
	if (lowcut == 5) {dcut= 13;}
	if (lowcut == 4) {dcut= 11;}
	if (lowcut == 3) {dcut= 7;}
	if (lowcut == 2) {dcut= 5;}
	if (lowcut < 2) {dcut= 3;}
	highpass = GetParameter( kParam_Two ) * 22.0;
	lowcut = floor(highpass)+1;
		
	Float64 decay = 0.001 - ((1.0-pow(1.0-GetParameter( kParam_Four ),3))*0.001);
	if (decay == 0.001) decay = 0.1;
	Float64 wet = GetParameter( kParam_Six );
	//removed unnecessary dry variable
	wet *= 0.01; //correct large gain issue
	Float64 correctionSample;
	Float64 accumulatorSample;
	Float64 overallscale = (GetParameter( kParam_Five )*9.0)+1.0;
	Float64 gain = overallscale;
	
	if (gain > 1.0) {f[0] = 1.0; gain -= 1.0;} else {f[0] = gain; gain = 0.0;}
	if (gain > 1.0) {f[1] = 1.0; gain -= 1.0;} else {f[1] = gain; gain = 0.0;}
	if (gain > 1.0) {f[2] = 1.0; gain -= 1.0;} else {f[2] = gain; gain = 0.0;}
	if (gain > 1.0) {f[3] = 1.0; gain -= 1.0;} else {f[3] = gain; gain = 0.0;}
	if (gain > 1.0) {f[4] = 1.0; gain -= 1.0;} else {f[4] = gain; gain = 0.0;}
	if (gain > 1.0) {f[5] = 1.0; gain -= 1.0;} else {f[5] = gain; gain = 0.0;}
	if (gain > 1.0) {f[6] = 1.0; gain -= 1.0;} else {f[6] = gain; gain = 0.0;}
	if (gain > 1.0) {f[7] = 1.0; gain -= 1.0;} else {f[7] = gain; gain = 0.0;}
	if (gain > 1.0) {f[8] = 1.0; gain -= 1.0;} else {f[8] = gain; gain = 0.0;}
	if (gain > 1.0) {f[9] = 1.0; gain -= 1.0;} else {f[9] = gain; gain = 0.0;}
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0) overallscale = 1.0;
	f[0] /= overallscale;
	f[1] /= overallscale;
	f[2] /= overallscale;
	f[3] /= overallscale;
	f[4] /= overallscale;
	f[5] /= overallscale;
	f[6] /= overallscale;
	f[7] /= overallscale;
	f[8] /= overallscale;
	f[9] /= overallscale;
	//and now it's neatly scaled, too
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		if (surge<fabs(inputSample))
		{
			surge += (double(fpd)/UINT32_MAX)*(fabs(inputSample)-surge);
			if (surge > 1.0) surge = 1.0;
		}
		else
		{
			surge -= ((double(fpd)/UINT32_MAX)*(surge-fabs(inputSample))*decay);
			if (surge < 0.0) surge = 0.0;
		}
		
		cutoff = pow((cutofftarget*surge),5);
		if (cutoff > 1.0) cutoff = 1.0;
		invcutoff = 1.0 - cutoff;
		//set up modified cutoff
		
		flip = !flip;
		filterflip = !filterflip;
		quadratic -= 1;
		if (quadratic < 0)
		{
			position += 1;		
			quadratic = position * position;
			quadratic = quadratic % 170003; //% is C++ mod operator
			quadratic *= quadratic;
			quadratic = quadratic % 17011; //% is C++ mod operator
			quadratic *= quadratic;
			//quadratic = quadratic % 1709; //% is C++ mod operator
			//quadratic *= quadratic;
			quadratic = quadratic % dcut; //% is C++ mod operator
			quadratic *= quadratic;
			quadratic = quadratic % lowcut;
			//sets density of the centering force
			if (noiseA < 0) {flip = true;}
			else {flip = false;}
		}
		
		
		if (flip) noiseA += (double(fpd)/UINT32_MAX);
		else noiseA -= (double(fpd)/UINT32_MAX);
		
		if (filterflip)
		{
			noiseB *= invcutoff; noiseB += (noiseA*cutoff);
			inputSample = noiseB+noiseC;
			rumbleA *= (1.0-rumblecutoff);
			rumbleA += (inputSample*rumblecutoff);
		}
		else 
		{
			noiseC *= invcutoff; noiseC += (noiseA*cutoff);
			inputSample = noiseB+noiseC;
			rumbleB *= (1.0-rumblecutoff);
			rumbleB += (inputSample*rumblecutoff);
		}
		
		inputSample -= (rumbleA+rumbleB);
		inputSample *= (1.0-rumblecutoff);
		
		inputSample *= wet;
		inputSample += (drySample * (1.0-wet));
		//apply the dry to the noise
		
		b[9] = b[8]; b[8] = b[7]; b[7] = b[6]; b[6] = b[5];
		b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1];
		b[1] = b[0]; b[0] = accumulatorSample = inputSample;
		
		accumulatorSample *= f[0];
		accumulatorSample += (b[1] * f[1]);
		accumulatorSample += (b[2] * f[2]);
		accumulatorSample += (b[3] * f[3]);
		accumulatorSample += (b[4] * f[4]);
		accumulatorSample += (b[5] * f[5]);
		accumulatorSample += (b[6] * f[6]);
		accumulatorSample += (b[7] * f[7]);
		accumulatorSample += (b[8] * f[8]);
		accumulatorSample += (b[9] * f[9]);
		//we are doing our repetitive calculations on a separate value
		
		correctionSample = inputSample - accumulatorSample;
		//we're gonna apply the total effect of all these calculations as a single subtract
		
		inputSample -= correctionSample;
		//applying the distance calculation to both the dry AND the noise output to blend them
				
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	position = 99999999;
	quadratic = 0;
	noiseA = 0.0;
	noiseB = 0.0;
	noiseC = 0.0;
	rumbleA = 0.0;
	rumbleB = 0.0;
	surge = 0.0;
	flip = false;
	filterflip = false;	
	for(int count = 0; count < 11; count++) {b[count] = 0.0; f[count] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
