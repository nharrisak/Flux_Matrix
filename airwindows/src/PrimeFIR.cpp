#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PrimeFIR"
#define AIRWINDOWS_DESCRIPTION "PrimeFIR"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','j' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Window", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Prime", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		int firPosition;
		uint32_t fpd;
	
	struct _dram {
			float firBuffer[32768];
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	float freq = pow(GetParameter( kParam_A ),2)*M_PI_2; if (freq < 0.0001f) freq = 0.0001f;
	float positionMiddle = sin(freq)*0.5f; //shift relative to frequency, not sample-rate
	freq /= overallscale; //generating the FIR relative to sample rate
	int window = (int)(GetParameter( kParam_B )*256.0f*overallscale); //so's the window size
	if (window < 2) window = 2; if (window > 998) window = 998;
	float fir[window+2];
	int middle = (int)((float)window*positionMiddle);
	bool nonPrime = (GetParameter( kParam_C ) < 0.5f);
	if (nonPrime) {
		for(int fip = 0; fip < middle; fip++) {
			fir[fip] = (fip-middle)*freq;
			fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
			fir[fip] *= sin(((float)fip/(float)window)*M_PI); //windowed with sin()
		}
		fir[middle] = 1.0f;
		for(int fip = middle+1; fip < window; fip++) {
			fir[fip] = (fip-middle)*freq;
			fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
			fir[fip] *= sin(((float)fip/(float)window)*M_PI); //windowed with sin()
		}
	} else {
		for(int fip = 0; fip < middle; fip++) {
			fir[fip] = (prime[middle-fip])*freq;
			fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
			fir[fip] *= sin(((float)fip/(float)window)*M_PI); //windowed with sin()
		}
		fir[middle] = 1.0f;
		for(int fip = middle+1; fip < window; fip++) {
			fir[fip] = (prime[fip-middle])*freq;
			fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
			fir[fip] *= sin(((float)fip/(float)window)*M_PI); //windowed with sin()
		}
	}
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		if (firPosition < 0 || firPosition > 32767) firPosition = 32767; int firp = firPosition;		
		dram->firBuffer[firp] = inputSample; inputSample = 0.0f;
		if (nonPrime) {
			if (firp + window < 32767) {
				for(int fip = 1; fip < window; fip++) {
					inputSample += dram->firBuffer[firp+fip] * fir[fip];
				}
			} else {
				for(int fip = 1; fip < window; fip++) {
					inputSample += dram->firBuffer[firp+fip - ((firp+fip > 32767)?32768:0)] * fir[fip];
				}
			}
			inputSample *= 0.25f;
		} else {
			if (firp + prime[window] < 32767) {
				for(int fip = 1; fip < window; fip++) {
					inputSample += dram->firBuffer[firp+prime[fip]] * fir[fip];
				}
			} else {
				for(int fip = 1; fip < window; fip++) {
					inputSample += dram->firBuffer[firp+prime[fip] - ((firp+prime[fip] > 32767)?32768:0)] * fir[fip];
				}
			}
			inputSample *= 0.5f;
		}
		inputSample *= sqrt(freq); //compensate for gain
		firPosition--;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 32767; count++) {dram->firBuffer[count] = 0.0;}
	firPosition = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
