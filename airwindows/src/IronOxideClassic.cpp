#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "IronOxideClassic"
#define AIRWINDOWS_DESCRIPTION "The purer, simpler, early form of Iron Oxide before all the features."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','r','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	struct _dram* dram;
 
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 fastIIRA;
		Float64 fastIIRB;
		Float64 slowIIRA;
		Float64 slowIIRB;
		SInt32 gcount;
		
		uint32_t fpd;
		bool fpFlip;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 d[264];
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	
	Float64 inputgain = pow(10.0,GetParameter( kParam_One )/20.0);
	Float64 outputgain = pow(10.0,GetParameter( kParam_Three )/20.0);
	Float64 ips = GetParameter( kParam_Two ) * 1.1;
	//slight correction to dial in convincing ips settings
	if (ips < 1 || ips > 200){ips=33.0;}
	//sanity checks are always key
	Float64 iirAmount = ips/430.0; //for low leaning
	Float64 bridgerectifier;
	Float64 fastTaper = ips/15.0;
	Float64 slowTaper = 2.0/(ips*ips);
	Float64 lowspeedscale = (5.0/ips);
	double inputSample;
	SInt32 count;
	Float64 temp;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	if (overallscale == 0) {fastTaper += 1.0; slowTaper += 1.0;}
	else
	{
		iirAmount /= overallscale;
		lowspeedscale *= overallscale;
		fastTaper = 1.0 + (fastTaper / overallscale);
		slowTaper = 1.0 + (slowTaper / overallscale);
	}
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
				
		if (fpFlip)
		{
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
		}
		//do IIR highpass for leaning out
		
		if (inputgain != 1.0) inputSample *= inputgain;
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;		
		
		if (gcount < 0 || gcount > 131) {gcount = 131;}
		count = gcount;
		//increment the counter
		
		dram->d[count+131] = dram->d[count] = inputSample;
		if (fpFlip)
		{
			fastIIRA = fastIIRA/fastTaper;
			slowIIRA = slowIIRA/slowTaper;
			//scale stuff down
			fastIIRA += dram->d[count];
			count += 3;
			temp = dram->d[count+127];
			temp += dram->d[count+113];
			temp += dram->d[count+109];
			temp += dram->d[count+107];
			temp += dram->d[count+103];
			temp += dram->d[count+101];
			temp += dram->d[count+97];
			temp += dram->d[count+89];
			temp += dram->d[count+83];
			temp /= 2;
			temp += dram->d[count+79];
			temp += dram->d[count+73];
			temp += dram->d[count+71];
			temp += dram->d[count+67];
			temp += dram->d[count+61];
			temp += dram->d[count+59];
			temp += dram->d[count+53];
			temp += dram->d[count+47];
			temp += dram->d[count+43];
			temp += dram->d[count+41];
			temp += dram->d[count+37];
			temp += dram->d[count+31];
			temp += dram->d[count+29];
			temp /= 2;
			temp += dram->d[count+23];
			temp += dram->d[count+19];
			temp += dram->d[count+17];
			temp += dram->d[count+13];
			temp += dram->d[count+11];
			temp /= 2;
			temp += dram->d[count+7];
			temp += dram->d[count+5];
			temp += dram->d[count+3];
			temp /= 2;
			temp += dram->d[count+2];
			temp += dram->d[count+1];
			slowIIRA += (temp/128);
			inputSample = fastIIRA - (slowIIRA / slowTaper);
		}
		else
		{
			fastIIRB = fastIIRB/fastTaper;
			slowIIRB = slowIIRB/slowTaper;
			//scale stuff down
			fastIIRB += dram->d[count];
			count += 3;
			temp = dram->d[count+127];
			temp += dram->d[count+113];
			temp += dram->d[count+109];
			temp += dram->d[count+107];
			temp += dram->d[count+103];
			temp += dram->d[count+101];
			temp += dram->d[count+97];
			temp += dram->d[count+89];
			temp += dram->d[count+83];
			temp /= 2;
			temp += dram->d[count+79];
			temp += dram->d[count+73];
			temp += dram->d[count+71];
			temp += dram->d[count+67];
			temp += dram->d[count+61];
			temp += dram->d[count+59];
			temp += dram->d[count+53];
			temp += dram->d[count+47];
			temp += dram->d[count+43];
			temp += dram->d[count+41];
			temp += dram->d[count+37];
			temp += dram->d[count+31];
			temp += dram->d[count+29];
			temp /= 2;
			temp += dram->d[count+23];
			temp += dram->d[count+19];
			temp += dram->d[count+17];
			temp += dram->d[count+13];
			temp += dram->d[count+11];
			temp /= 2;
			temp += dram->d[count+7];
			temp += dram->d[count+5];
			temp += dram->d[count+3];
			temp /= 2;
			temp += dram->d[count+2];
			temp += dram->d[count+1];
			slowIIRB += (temp/128);
			inputSample = fastIIRB - (slowIIRB / slowTaper);
		}
		inputSample /= fastTaper;
		inputSample /= lowspeedscale;
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		//can use as an output limiter
		if (inputSample > 0.0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		
		if (outputgain != 1.0) inputSample *= outputgain;
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
	for (int temp = 0; temp < 263; temp++) {dram->d[temp] = 0.0;}
	gcount = 0;
	fastIIRA = fastIIRB = slowIIRA = slowIIRB = 0.0;
	iirSampleA = iirSampleB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	fpFlip = true;
}
};
