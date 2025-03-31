#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "FinalClip"
#define AIRWINDOWS_DESCRIPTION "One stage of ADClip8 set up to clip +6dB for Final Cut Pro."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','i','n' )
#define AIRWINDOWS_KERNELS
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
};
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float32 lastSample;  //Float32 as this runs in pure bypass most of the time
		Float32 intermediate[16];
		bool wasPosClip;
		bool wasNegClip;
		//uint32_t fpd; //leave off
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
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
	
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	//double hardness = 0.618033988749894; // golden ratio
	//double softness = 0.381966011250105; // 1.0 - hardness
	//double refclip = 1.618033988749894; // -0.2dB we're making all this pure raw code
	//refclip*hardness = 1.0  to use ClipOnly as a prefab code-chunk.
	//refclip*softness = 0.618033988749894	Seven decimal places is plenty as it's
	//not related to the original sound much: it's an arbitrary position in softening.

	while (nSampleFrames-- > 0) {
		Float32 inputSample = *sourceP;
		
		if (inputSample > 4.0) inputSample = 4.0; if (inputSample < -4.0) inputSample = -4.0;
		if (inputSample - lastSample > 0.618033988749894) inputSample = lastSample + 0.618033988749894;
		if (inputSample - lastSample < -0.618033988749894) inputSample = lastSample - 0.618033988749894;
		//same as slew clippage
		
		//begin ClipOnly2 as a little, compressed chunk that can be dropped into code
		if (wasPosClip == true) { //current will be over
			if (inputSample<lastSample) lastSample=1.0+(inputSample*0.381966011250105);
			else lastSample = 0.618033988749894+(lastSample*0.618033988749894);
		} wasPosClip = false;
		if (inputSample>1.618033988749894) {wasPosClip=true;inputSample=1.0+(lastSample*0.381966011250105);}
		if (wasNegClip == true) { //current will be -over
			if (inputSample > lastSample) lastSample=-1.0+(inputSample*0.381966011250105);
			else lastSample=-0.618033988749894+(lastSample*0.618033988749894);
		} wasNegClip = false;
		if (inputSample<-1.618033988749894) {wasNegClip=true;inputSample=-1.0+(lastSample*0.381966011250105);}
		intermediate[spacing] = inputSample;
        inputSample = lastSample; //Latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) intermediate[x-1] = intermediate[x];
		lastSample = intermediate[0]; //run a little buffer to handle this
		//end ClipOnly2 as a little, compressed chunk that can be dropped into code
		
        *destP = inputSample; //this one doesn't get dithering as it is almost always pure bypass
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	wasPosClip = false;
	wasNegClip = false;
	for (int x = 0; x < 16; x++) intermediate[x] = 0.0;
	//fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX; //leave off for ClipOnly
}
};
