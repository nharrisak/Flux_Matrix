#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Huge"
#define AIRWINDOWS_DESCRIPTION "A dirty, dirty loudenator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','u','g' )
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
{ .name = "Huge", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Nasty", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		double prev3;
		double prev5;
		double prev7;
		double prev9;
		double prev11;
		double prev13;
		double prevOut;
		double limit3;
		double limit5;
		double limit7;
		double limit9;
		double limit11;
		double limit13;
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	
	double huge = GetParameter( kParam_One );
	double threshold = (GetParameter( kParam_Two ) * 0.05) + 0.05;
	double attack = (threshold * 8.0) / overallscale;
	double release = (threshold * 8.0) / overallscale;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;

		inputSample *= huge;
		
		double inP3 = inputSample * inputSample * inputSample;
		double outP3 = ((4.0*inP3)-(3.0*inputSample))*-0.36;
		double inP5 = inP3 * inputSample * inputSample;
		double outP5 = ((16.0*inP5)-(20.0*inP3)+(5.0*inputSample))*0.2025;
		double inP7 = inP5 * inputSample * inputSample;
		double outP7 = ((64.0*inP7)-(112.0*inP5)+(56.0*inP3)-(7.0*inputSample))*-0.1444;
		double inP9 = inP7 * inputSample * inputSample;
		double outP9 = ((256.0*inP9)-(576.0*inP7)+(432.0*inP5)-(120.0*inP3)+(9.0*inputSample))*0.1225;
		double inP11 = inP9 * inputSample * inputSample;
		double outP11 = ((1024.0*inP11)-(2816.0*inP9)+(2816.0*inP7)-(1232.0*inP5)+(220.0*inP3)-(11.0*inputSample))*-0.1024;
		double inP13 = inP11 * inputSample * inputSample;
		double outP13 = ((4096.0*inP13)-(13312.0*inP11)+(16640.0*inP9)-(9984.0*inP7)+(2912.0*inP5)-(364.0*inP3)+(13.0*inputSample))*0.09;

		double slew3 = fabs(outP3 - prev3); prev3 = outP3;
		double slew5 = fabs(outP5 - prev5); prev5 = outP5;
		double slew7 = fabs(outP7 - prev7); prev7 = outP7;
		double slew9 = fabs(outP9 - prev9); prev9 = outP9;
		double slew11 = fabs(outP11 - prev11); prev11 = outP11;
		double slew13 = fabs(outP13 - prev13); prev13 = outP13;
		
		if (slew3 < threshold) limit3 += release;
		else limit3 -= attack;
		if (slew5 < threshold) limit5 += release;
		else limit5 -= attack;
		if (slew7 < threshold) limit7 += release;
		else limit7 -= attack;
		if (slew9 < threshold) limit9 += release;
		else limit9 -= attack;
		if (slew11 < threshold) limit11 += release;
		else limit11 -= attack;
		if (slew13 < threshold) limit13 += release;
		else limit13 -= attack;
		
		if (limit3 > 1.0)  {limit3 = 1.0;}
		if (limit5 > 1.0)  {limit3 = 1.0; limit5 = 1.0;}
		if (limit7 > 1.0)  {limit3 = 1.0; limit5 = 1.0; limit7 = 1.0;}
		if (limit9 > 1.0)  {limit3 = 1.0; limit5 = 1.0; limit7 = 1.0; limit9 = 1.0;}
		if (limit11 > 1.0) {limit3 = 1.0; limit5 = 1.0; limit7 = 1.0; limit9 = 1.0; limit11 = 1.0;}
		if (limit13 > 1.0) {limit3 = 1.0; limit5 = 1.0; limit7 = 1.0; limit9 = 1.0; limit11 = 1.0; limit13 = 1.0;}
		if (limit3 < 0.0) {limit3 = 0.0; limit5 = 0.0; limit7 = 0.0; limit9 = 0.0; limit11 = 0.0; limit13 = 0.0;}
		if (limit5 < 0.0) {limit5 = 0.0; limit7 = 0.0; limit9 = 0.0; limit11 = 0.0; limit13 = 0.0;}
		if (limit7 < 0.0) {limit7 = 0.0; limit9 = 0.0; limit11 = 0.0; limit13 = 0.0;}
		if (limit9 < 0.0) {limit9 = 0.0; limit11 = 0.0; limit13 = 0.0;}
		if (limit11 < 0.0) {limit11 = 0.0; limit13 = 0.0;}
		if (limit13 < 0.0) {limit13 = 0.0;}
		
		double chebyshev = (outP3 * limit3);
		chebyshev += (outP5 * limit5);
		chebyshev += (outP7 * limit7);
		chebyshev += (outP9 * limit9);
		chebyshev += (outP11 * limit11);
		chebyshev += (outP13 * limit13);
		inputSample += ((chebyshev+prevOut)*0.5);
		prevOut = chebyshev;
		
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
	prev3 = prev5 = prev7 = prev9 = prev11 = prev13 = prevOut = 0.0;
	limit3 = limit5 = limit7 = limit9 = limit11 = limit13 = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
