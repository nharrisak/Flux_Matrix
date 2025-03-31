#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ElectroHat"
#define AIRWINDOWS_DESCRIPTION "A hi-hat tone generator that uses the original sound as a control voltage."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','l','e' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
static const int kSynHat = 1;
static const int kElectroHat = 2;
static const int kDenseHat = 3;
static const int k606Hat = 4;
static const int k808Hat = 5;
static const int k909Hat = 6;
static const int kDefaultValue_ParamOne = kSynHat;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static char const * const enumStrings0[] = { "", "Tunable Syn Hat", "Tunable Electro Hat", "Tunable Dense Hat", "606 Style Preset", "808 Style Preset", "909 Style Preset", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "HiHat Type", .min = 1, .max = 6, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Trim", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Brighten", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Pad", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 storedSample;
		Float64 lastSample;
		int tik;
		int lok;
		bool flip;
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
	bool highSample = false;
	if (GetSampleRate() > 64000) highSample = true;
	//we will go to another dither for 88 and 96K
	Float64 inputSample;
	Float64 tempSample;
	Float64 drySample;
	int deSyn = GetParameter( kParam_One );
	Float64 increment = GetParameter( kParam_Two );
	Float64 brighten = GetParameter( kParam_Three );
	Float64 outputlevel = GetParameter( kParam_Four );
	Float64 wet = GetParameter( kParam_Five );
	
	if (deSyn == 4) {deSyn = 1; increment = 0.411; brighten = 0.87;}
	//606 preset
	if (deSyn == 5) {deSyn = 2; increment = 0.111; brighten = 1.0;}
	//808 preset
	if (deSyn == 6) {deSyn = 2; increment = 0.299; brighten = 0.359;}
	//909 preset
	int tok = deSyn + 1;
	increment *= 0.98;
	increment += 0.01;
	increment += (Float64)tok;
	Float64 fosA = increment;
	Float64 fosB = fosA * increment;
	Float64 fosC = fosB * increment;
	Float64 fosD = fosC * increment;
	Float64 fosE = fosD * increment;
	Float64 fosF = fosE * increment;
	int posA = fosA;
	int posB = fosB;
	int posC = fosC;
	int posD = fosD;
	int posE = fosE;
	int posF = fosF;
	int posG = posF*posE*posD*posC*posB; //factorial
	

	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		drySample = inputSample;
		
		inputSample = fabs(inputSample)*outputlevel;
		
		if (flip) { //will always be true unless we have high sample rate
			tik++;
			tik = tik % posG;		
			tok = tik * tik; tok = tok % posF;
			tok *= tok; tok = tok % posE;
			tok *= tok; tok = tok % posD;
			tok *= tok; tok = tok % posC;
			tok *= tok; tok = tok % posB;
			tok *= tok; tok = tok % posA;
			inputSample = tok*inputSample;
			if ((abs(lok-tok)<abs(lok+tok))&&(deSyn == 1)) {inputSample = -tok*inputSample;}		
			if ((abs(lok-tok)>abs(lok+tok))&&(deSyn == 2)) {inputSample = -tok*inputSample;}
			if ((abs(lok-tok)<abs(lok+tok))&&(deSyn == 3)) {inputSample = -tok*inputSample;}
			lok = tok;
			tempSample = inputSample;
			inputSample = inputSample - (lastSample*brighten);
			lastSample = tempSample;
		} else { //we have high sample rate and this is an interpolate sample
			inputSample = lastSample;
			//not really interpolating, just sample-and-hold
		}
		
		if (highSample) {
			flip = !flip;
		} else {
			flip = true;
		}
		
		tempSample = inputSample;
		inputSample += storedSample;
		storedSample = tempSample;
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}		
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSample;
		//moving average combats that near-Nyquist stuff
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	storedSample = 0.0;
	lastSample = 0.0;
	tik = 3746926;
	lok = 0;
	flip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
