#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ADClip8"
#define AIRWINDOWS_DESCRIPTION "The ultimate Airwindows loudenator/biggenator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','D','D' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,//Add your parameters here...
	kNumberOfParameters=3
};
static const int kNormal = 1;
static const int kGain = 2;
static const int kClip = 3;
static const int kAfterburner = 4;
static const int kExplode = 5;
static const int kNuke = 6;
static const int kApocalypse = 7;
static const int kApotheosis = 8;
static const int kDefaultValue_ParamThree = kNormal;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static char const * const enumStrings2[] = { "", "ADClip Normal", "Gain Match", "Clip Only", "Afterburner", "Explode", "Nuke", "Apocalypse", "Apotheosis", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Boost", .min = 0, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ceiling", .min = 0, .max = 1000, .def = 883, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mode", .min = 1, .max = 8, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings2 },
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
 
		double lastSample[8];
		double intermediate[16][8];
		bool wasPosClip[8];
		bool wasNegClip[8];
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	//double hardness = 0.618033988749894; // golden ratio
	//double softness = 0.381966011250105; // 1.0 - hardness
	//double refclip = 1.618033988749894; // -0.2dB we're making all this pure raw code
	//refclip*hardness = 1.0  to use ClipOnly as a prefab code-chunk.
	//refclip*softness = 0.618033988749894	Seven decimal places is plenty as it's
	//not related to the original sound much: it's an arbitrary position in softening.
	double inputGain = pow(10.0,(GetParameter( kParam_One ))/20.0);

	double ceiling = (1.0+(GetParameter( kParam_Two )*0.23594733))*0.5;
	int mode = (int) GetParameter( kParam_Three );
	int stageSetting = mode-2;
	if (stageSetting < 1) stageSetting = 1;
	inputGain = ((inputGain-1.0)/stageSetting)+1.0;

	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double overshoot = 0.0;
		inputSample *= 1.618033988749894;
		
		for (int stage = 0; stage < stageSetting; stage++) {
			if (inputGain != 1.0) {
				inputSample *= inputGain;
			}
			if (stage == 0){
				overshoot = fabs(inputSample) - 1.618033988749894;
				if (overshoot < 0.0) overshoot = 0.0;
			}
			if (inputSample > 4.0) inputSample = 4.0; if (inputSample < -4.0) inputSample = -4.0;
			if (inputSample - lastSample[stage] > 0.618033988749894) inputSample = lastSample[stage] + 0.618033988749894;
			if (inputSample - lastSample[stage] < -0.618033988749894) inputSample = lastSample[stage] - 0.618033988749894;
			//same as slew clippage
			
			//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
			if (wasPosClip[stage] == true) { //current will be over
				if (inputSample<lastSample[stage]) lastSample[stage]=1.0+(inputSample*0.381966011250105);
				else lastSample[stage] = 0.618033988749894+(lastSample[stage]*0.618033988749894);
			} wasPosClip[stage] = false;
			if (inputSample>1.618033988749894) {wasPosClip[stage]=true;inputSample=1.0+(lastSample[stage]*0.381966011250105);}
			if (wasNegClip[stage] == true) { //current will be -over
				if (inputSample > lastSample[stage]) lastSample[stage]=-1.0+(inputSample*0.381966011250105);
				else lastSample[stage]=-0.618033988749894+(lastSample[stage]*0.618033988749894);
			} wasNegClip[stage] = false;
			if (inputSample<-1.618033988749894) {wasNegClip[stage]=true;inputSample=-1.0+(lastSample[stage]*0.381966011250105);}
			intermediate[spacing][stage] = inputSample;
			inputSample = lastSample[stage]; //Latency is however many samples equals one 44.1k sample
			for (int x = spacing; x > 0; x--) intermediate[x-1][stage] = intermediate[x][stage];
			lastSample[stage] = intermediate[0][stage]; //run a little buffer to handle this
			//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		}
		
		switch (mode)
		{
			case 1: break; //Normal
			case 2: inputSample /= inputGain; break; //Gain Match
			case 3: inputSample = overshoot; break; //Clip Only
			case 4: break; //Afterburner
			case 5: break; //Explode
			case 6: break; //Nuke
			case 7: break; //Apocalypse
			case 8: break; //Apotheosis
		}
		//this is our output mode switch, showing the effects
		inputSample *= ceiling;

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
	for (int stage = 0; stage < 8; stage++) {
		lastSample[stage] = 0.0;
		wasPosClip[stage] = false;
		wasNegClip[stage] = false;
		for (int x = 0; x < 16; x++) intermediate[x][stage] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
