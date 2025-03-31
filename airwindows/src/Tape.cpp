#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Tape"
#define AIRWINDOWS_DESCRIPTION "Simplified, all-purpose tape mojo: my personal jam."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','a','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Slam", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bump", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 iirMidRollerA;
		Float64 iirMidRollerB;
		Float64 iirHeadBumpA;
		Float64 iirHeadBumpB;
		bool flip;
		double biquadA[9];
		double biquadB[9];
		double biquadC[9];
		double biquadD[9];
		double lastSample;
		uint32_t fpd;
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
	
	Float64 inputgain = pow(10.0,GetParameter( kParam_One )/20.0);
	Float64 bumpgain = GetParameter( kParam_Two ) * 0.1;
	Float64 HeadBumpFreq = 0.12/overallscale;
	Float64 softness = 0.618033988749894848204586;
	Float64 RollAmount = (1.0 - softness) / overallscale;
	
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	biquadA[0] = biquadB[0] = 0.0072/overallscale;
	biquadA[1] = biquadB[1] = 0.0009;
	double K = tan(M_PI * biquadB[0]);
	double norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
	biquadA[2] = biquadB[2] = K / biquadB[1] * norm;
	biquadA[4] = biquadB[4] = -biquadB[2];
	biquadA[5] = biquadB[5] = 2.0 * (K * K - 1.0) * norm;
	biquadA[6] = biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
	
	biquadC[0] = biquadD[0] = 0.032/overallscale;
	biquadC[1] = biquadD[1] = 0.0007;
	K = tan(M_PI * biquadD[0]);
	norm = 1.0 / (1.0 + K / biquadD[1] + K * K);
	biquadC[2] = biquadD[2] = K / biquadD[1] * norm;
	biquadC[4] = biquadD[4] = -biquadD[2];
	biquadC[5] = biquadD[5] = 2.0 * (K * K - 1.0) * norm;
	biquadC[6] = biquadD[6] = (1.0 - K / biquadD[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		if (inputgain < 1.0) {
			inputSample *= inputgain;
		} //gain cut before anything, even dry
		
		double drySample = inputSample;
		
		double HighsSample = 0.0;
		double NonHighsSample = 0.0;
		double tempSample;
		
		if (flip)
		{
			iirMidRollerA = (iirMidRollerA * (1.0 - RollAmount)) + (inputSample * RollAmount);
			HighsSample = inputSample - iirMidRollerA;
			NonHighsSample = iirMidRollerA;
			
			iirHeadBumpA += (inputSample * 0.05);
			iirHeadBumpA -= (iirHeadBumpA * iirHeadBumpA * iirHeadBumpA * HeadBumpFreq);
			iirHeadBumpA = sin(iirHeadBumpA);
			tempSample = (iirHeadBumpA * biquadA[2]) + biquadA[7];
			biquadA[7] = (iirHeadBumpA * biquadA[3]) - (tempSample * biquadA[5]) + biquadA[8];
			biquadA[8] = (iirHeadBumpA * biquadA[4]) - (tempSample * biquadA[6]);
			iirHeadBumpA = tempSample; //interleaved biquad
			if (iirHeadBumpA > 1.0) iirHeadBumpA = 1.0;
			if (iirHeadBumpA < -1.0) iirHeadBumpA = -1.0;
			iirHeadBumpA = asin(iirHeadBumpA);
			
			inputSample = sin(inputSample);
			tempSample = (inputSample * biquadC[2]) + biquadC[7];
			biquadC[7] = (inputSample * biquadC[3]) - (tempSample * biquadC[5]) + biquadC[8];
			biquadC[8] = (inputSample * biquadC[4]) - (tempSample * biquadC[6]);
			inputSample = tempSample; //interleaved biquad
			if (inputSample > 1.0) inputSample = 1.0;
			if (inputSample < -1.0) inputSample = -1.0;
			inputSample = asin(inputSample);
		} else {
			iirMidRollerB = (iirMidRollerB * (1.0 - RollAmount)) + (inputSample * RollAmount);
			HighsSample = inputSample - iirMidRollerB;
			NonHighsSample = iirMidRollerB;
			
			iirHeadBumpB += (inputSample * 0.05);
			iirHeadBumpB -= (iirHeadBumpB * iirHeadBumpB * iirHeadBumpB * HeadBumpFreq);
			iirHeadBumpB = sin(iirHeadBumpB);
			tempSample = (iirHeadBumpB * biquadB[2]) + biquadB[7];
			biquadB[7] = (iirHeadBumpB * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
			biquadB[8] = (iirHeadBumpB * biquadB[4]) - (tempSample * biquadB[6]);
			iirHeadBumpB = tempSample; //interleaved biquad
			if (iirHeadBumpB > 1.0) iirHeadBumpB = 1.0;
			if (iirHeadBumpB < -1.0) iirHeadBumpB = -1.0;
			iirHeadBumpB = asin(iirHeadBumpB);
			
			inputSample = sin(inputSample);
			tempSample = (inputSample * biquadD[2]) + biquadD[7];
			biquadD[7] = (inputSample * biquadD[3]) - (tempSample * biquadD[5]) + biquadD[8];
			biquadD[8] = (inputSample * biquadD[4]) - (tempSample * biquadD[6]);
			inputSample = tempSample; //interleaved biquad
			if (inputSample > 1.0) inputSample = 1.0;
			if (inputSample < -1.0) inputSample = -1.0;
			inputSample = asin(inputSample);
		}
		flip = !flip;
		
		double groundSample = drySample - inputSample; //set up UnBox
		if (inputgain > 1.0) {
			inputSample *= inputgain;
		} //gain boost inside UnBox: do not boost fringe audio
		
		
		double applySoften = fabs(HighsSample)*1.57079633;
		if (applySoften > 1.57079633) applySoften = 1.57079633;
		applySoften = 1-cos(applySoften);
		if (HighsSample > 0) inputSample -= applySoften;
		if (HighsSample < 0) inputSample += applySoften;
		//apply Soften depending on polarity
		
		if (inputSample > 1.2533141373155) inputSample = 1.2533141373155;
		if (inputSample < -1.2533141373155) inputSample = -1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output
		inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0) ?1:fabs(inputSample));
		//Spiral, for cleanest most optimal tape effect
		
		Float64 suppress = (1.0-fabs(inputSample)) * 0.00013;
		if (iirHeadBumpA > suppress) iirHeadBumpA -= suppress;
		if (iirHeadBumpA < -suppress) iirHeadBumpA += suppress;
		if (iirHeadBumpB > suppress) iirHeadBumpB -= suppress;
		if (iirHeadBumpB < -suppress) iirHeadBumpB += suppress;
		//restrain resonant quality of head bump algorithm
		
		inputSample += groundSample; //apply UnBox processing
		
		inputSample += ((iirHeadBumpA + iirHeadBumpB) * bumpgain);//and head bump
				
		if (lastSample >= 0.99)
		{
			if (inputSample < 0.99) lastSample = ((0.99*softness) + (inputSample * (1.0-softness)));
			else lastSample = 0.99;
		}
		
		if (lastSample <= -0.99)
		{
			if (inputSample > -0.99) lastSample = ((-0.99*softness) + (inputSample * (1.0-softness)));
			else lastSample = -0.99;
		}
		
		if (inputSample > 0.99)
		{
			if (lastSample < 0.99) inputSample = ((0.99*softness) + (lastSample * (1.0-softness)));
			else inputSample = 0.99;
		}
		
		if (inputSample < -0.99)
		{
			if (lastSample > -0.99) inputSample = ((-0.99*softness) + (lastSample * (1.0-softness)));
			else inputSample = -0.99;
		}
		lastSample = inputSample;
		
		if (inputSample > 0.99) inputSample = 0.99;
		if (inputSample < -0.99) inputSample = -0.99;
		//final iron bar

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
	iirMidRollerA = 0.0;
	iirMidRollerB = 0.0;
	iirHeadBumpA = 0.0;
	iirHeadBumpB = 0.0;	
	flip = false;
	for (int x = 0; x < 9; x++) {biquadA[x] = 0.0;biquadB[x] = 0.0;biquadC[x] = 0.0;biquadD[x] = 0.0;}
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
