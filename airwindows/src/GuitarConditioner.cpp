#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "GuitarConditioner"
#define AIRWINDOWS_DESCRIPTION "Like a Tube Screamer voicing without the squishiness and indistinctness."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','u','i' )
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
 
		uint32_t fpd;
		bool fpFlip;
		Float64 lastSampleT;
		Float64 lastSampleB; //for Slews
		Float64 iirSampleTA;
		Float64 iirSampleTB;
		Float64 iirSampleBA;
		Float64 iirSampleBB; //for Highpasses

		
	
	struct _dram {
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
	double inputSample;
	double treble;
	double bass;
	Float64 iirTreble = 0.287496/overallscale; //tight is -1
	Float64 iirBass = 0.085184/overallscale; //tight is 1
	iirTreble += iirTreble;
	iirBass += iirBass; //simple double when tight is -1 or 1
	Float64 tightBass = 0.6666666666;
	Float64 tightTreble = -0.3333333333;
	Float64 offset;
	Float64 clamp;
	Float64 threshTreble = 0.0081/overallscale;
	Float64 threshBass = 0.0256/overallscale;

	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		
		treble = bass = inputSample;
		treble += treble; //+3dB on treble as the highpass is higher
		
		offset = (1 + tightTreble) + ((1-fabs(treble))*tightTreble); //treble HP
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1; //made offset for HP
		if (fpFlip) {
			iirSampleTA = (iirSampleTA * (1 - (offset * iirTreble))) + (treble * (offset * iirTreble));
			treble = treble - iirSampleTA;
		} else {
			iirSampleTB = (iirSampleTB * (1 - (offset * iirTreble))) + (treble * (offset * iirTreble));
			treble = treble - iirSampleTB;
		} //done treble HP
		
		offset = (1 - tightBass) + (fabs(bass)*tightBass); //bass HP
		if (offset < 0) offset = 0;
		if (offset > 1) offset = 1;
		if (fpFlip) {
			iirSampleBA = (iirSampleBA * (1 - (offset * iirBass))) + (bass * (offset * iirBass));
			bass = bass - iirSampleBA;
		} else {
			iirSampleBB = (iirSampleBB * (1 - (offset * iirBass))) + (bass * (offset * iirBass));
			bass = bass - iirSampleBB;
		} //done bass HP
		
		inputSample = treble;
		clamp = inputSample - lastSampleT;
		if (clamp > threshTreble)
			treble = lastSampleT + threshTreble;
		if (-clamp > threshTreble)
			treble = lastSampleT - threshTreble;
		lastSampleT = treble; //treble slew
		
		inputSample = bass;
		clamp = inputSample - lastSampleB;
		if (clamp > threshBass)
			bass = lastSampleB + threshBass;
		if (-clamp > threshBass)
			bass = lastSampleB - threshBass;
		lastSampleB = bass; //bass slew
		
		inputSample = treble + bass; //final merge
		fpFlip = !fpFlip;
		
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
	lastSampleT = 0.0;
	lastSampleB = 0.0; //for Slews. T for treble, B for bass
	iirSampleTA = 0.0;
	iirSampleTB = 0.0;
	iirSampleBA = 0.0;
	iirSampleBB = 0.0; //for Highpasses
}
};
