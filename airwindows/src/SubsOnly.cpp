#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SubsOnly"
#define AIRWINDOWS_DESCRIPTION "A mix check plugin that shows you only the extreme lows."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','u','c' )
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
 
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 iirSampleE;
		Float64 iirSampleF;
		Float64 iirSampleG;
		Float64 iirSampleH;
		Float64 iirSampleI;
		Float64 iirSampleJ;
		Float64 iirSampleK;
		Float64 iirSampleL;
		Float64 iirSampleM;
		Float64 iirSampleN;
		Float64 iirSampleO;
		Float64 iirSampleP;
		Float64 iirSampleQ;
		Float64 iirSampleR;
		Float64 iirSampleS;
		Float64 iirSampleT;
		Float64 iirSampleU;
		Float64 iirSampleV;
		Float64 iirSampleW;
		Float64 iirSampleX;
		Float64 iirSampleY;
		Float64 iirSampleZ;
		uint32_t fpd;
	
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
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 iirAmount = 2250/44100.0;
	Float64 gaintarget = 1.42;
	Float64 gain;
	iirAmount /= overallscale;
	Float64 altAmount = 1.0 - iirAmount;
	Float64 inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;

		gain = gaintarget;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		iirSampleA = (iirSampleA * altAmount) + (inputSample * iirAmount); inputSample = iirSampleA;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleB = (iirSampleB * altAmount) + (inputSample * iirAmount); inputSample = iirSampleB;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleC = (iirSampleC * altAmount) + (inputSample * iirAmount); inputSample = iirSampleC;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleD = (iirSampleD * altAmount) + (inputSample * iirAmount); inputSample = iirSampleD;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleE = (iirSampleE * altAmount) + (inputSample * iirAmount); inputSample = iirSampleE;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleF = (iirSampleF * altAmount) + (inputSample * iirAmount); inputSample = iirSampleF;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleG = (iirSampleG * altAmount) + (inputSample * iirAmount); inputSample = iirSampleG;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleH = (iirSampleH * altAmount) + (inputSample * iirAmount); inputSample = iirSampleH;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleI = (iirSampleI * altAmount) + (inputSample * iirAmount); inputSample = iirSampleI;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleJ = (iirSampleJ * altAmount) + (inputSample * iirAmount); inputSample = iirSampleJ;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleK = (iirSampleK * altAmount) + (inputSample * iirAmount); inputSample = iirSampleK;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleL = (iirSampleL * altAmount) + (inputSample * iirAmount); inputSample = iirSampleL;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleM = (iirSampleM * altAmount) + (inputSample * iirAmount); inputSample = iirSampleM;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleN = (iirSampleN * altAmount) + (inputSample * iirAmount); inputSample = iirSampleN;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleO = (iirSampleO * altAmount) + (inputSample * iirAmount); inputSample = iirSampleO;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleP = (iirSampleP * altAmount) + (inputSample * iirAmount); inputSample = iirSampleP;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleQ = (iirSampleQ * altAmount) + (inputSample * iirAmount); inputSample = iirSampleQ;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleR = (iirSampleR * altAmount) + (inputSample * iirAmount); inputSample = iirSampleR;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleS = (iirSampleS * altAmount) + (inputSample * iirAmount); inputSample = iirSampleS;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleT = (iirSampleT * altAmount) + (inputSample * iirAmount); inputSample = iirSampleT;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleU = (iirSampleU * altAmount) + (inputSample * iirAmount); inputSample = iirSampleU;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleV = (iirSampleV * altAmount) + (inputSample * iirAmount); inputSample = iirSampleV;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleW = (iirSampleW * altAmount) + (inputSample * iirAmount); inputSample = iirSampleW;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleX = (iirSampleX * altAmount) + (inputSample * iirAmount); inputSample = iirSampleX;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleY = (iirSampleY * altAmount) + (inputSample * iirAmount); inputSample = iirSampleY;
		inputSample *= gain; gain = ((gain-1)*0.75)+1;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		iirSampleZ = (iirSampleZ * altAmount) + (inputSample * iirAmount); inputSample = iirSampleZ;
		if (inputSample > 1.0) inputSample = 1.0;
		if (inputSample < -1.0) inputSample = -1.0;
		
		
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
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	iirSampleF = 0.0;
	iirSampleG = 0.0;
	iirSampleH = 0.0;
	iirSampleI = 0.0;
	iirSampleJ = 0.0;
	iirSampleK = 0.0;
	iirSampleL = 0.0;
	iirSampleM = 0.0;
	iirSampleN = 0.0;
	iirSampleO = 0.0;
	iirSampleP = 0.0;
	iirSampleQ = 0.0;
	iirSampleR = 0.0;
	iirSampleS = 0.0;
	iirSampleT = 0.0;
	iirSampleU = 0.0;
	iirSampleV = 0.0;
	iirSampleW = 0.0;
	iirSampleX = 0.0;
	iirSampleY = 0.0;
	iirSampleZ = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
