#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "IronOxide5"
#define AIRWINDOWS_DESCRIPTION "The old school, heavily colored Airwindows tape emulation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','I','r','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	kParam_Seven =6,
	//Add your parameters here...
	kNumberOfParameters=7
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highs Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Lows Tape Speed", .min = 150, .max = 15000, .def = 1500, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Flutter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Noise", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Dry/Wet", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 iirSamplehA;
		Float64 iirSamplehB;
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 fastIIRA;
		Float64 fastIIRB;
		Float64 slowIIRA;
		Float64 slowIIRB;
		Float64 fastIIHA;
		Float64 fastIIHB;
		Float64 slowIIHA;
		Float64 slowIIHB;
		SInt32 gcount;
		Float64 prevInputSample;
		
		SInt32 fstoredcount;
		Float64 rateof;
		Float64 sweep;
		Float64 nextmax;
		uint32_t fpd;
		bool flip;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 d[264];
		Float64 fl[100];
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	
	Float64 inputgain = pow(10.0,GetParameter( kParam_One )/20.0);
	Float64 outputgain = pow(10.0,GetParameter( kParam_Six )/20.0);
	Float64 ips = GetParameter( kParam_Two ) * 1.1;
	//slight correction to dial in convincing ips settings
	if (ips < 1 || ips > 200){ips=33.0;}
	//sanity checks are always key
	Float64 tempRandy = 0.04+(0.11/sqrt(ips));
	Float64 randy;
	Float64 lps = GetParameter( kParam_Three ) * 1.1;
	//slight correction to dial in convincing ips settings
	if (lps < 1 || lps > 200){lps=33.0;}
	//sanity checks are always key
	Float64 iirAmount = lps/430.0; //for low leaning
	Float64 bridgerectifier;
	Float64 fastTaper = ips/15.0;
	Float64 slowTaper = 2.0/(lps*lps);
	Float64 lowspeedscale = (5.0/ips);
	double inputSample;
	Float64 drySample;
	SInt32 count;
	SInt32 flutcount;
	Float64 flutterrandy;
	Float64 temp;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 depth = pow(GetParameter( kParam_Four ),2)*overallscale;
	Float64 fluttertrim = 0.00581/overallscale;
	Float64 sweeptrim = (0.0005*depth)/overallscale;
	Float64 offset;	
	Float64 tupi = 3.141592653589793238 * 2.0;
	Float64 newrate = 0.006/overallscale;
	Float64 oldrate = 1.0-newrate;	
	if (overallscale == 0) {fastTaper += 1.0; slowTaper += 1.0;}
	else
	{
		iirAmount /= overallscale;
		lowspeedscale *= overallscale;
		fastTaper = 1.0 + (fastTaper / overallscale);
		slowTaper = 1.0 + (slowTaper / overallscale);
	}
	Float64 noise = GetParameter( kParam_Five ) * 0.5;
	Float64 invdrywet = GetParameter( kParam_Seven );
	Float64 dry = 1.0;
	if (invdrywet > 0.0) dry -= invdrywet;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		flutterrandy = (double(fpd)/UINT32_MAX);
		//part of flutter section
		//now we've got a random flutter, so we're messing with the pitch before tape effects go on
		if (fstoredcount < 0 || fstoredcount > 30) {fstoredcount = 30;}
		flutcount = fstoredcount;
		dram->fl[flutcount+31] = dram->fl[flutcount] = inputSample;
		offset = (1.0 + sin(sweep)) * depth;
		flutcount += (int)floor(offset);
		bridgerectifier = (dram->fl[flutcount] * (1-(offset-floor(offset))));
		bridgerectifier += (dram->fl[flutcount+1] * (offset-floor(offset)));
		rateof = (nextmax * newrate) + (rateof * oldrate);
		sweep += rateof * fluttertrim;
		sweep += sweep * sweeptrim;
		if (sweep >= tupi){sweep = 0.0; nextmax = 0.02 + (flutterrandy*0.98);}
		fstoredcount--;
		inputSample = bridgerectifier;
		//apply to input signal, interpolate samples
		//all the funky renaming is just trying to fix how I never reassigned the control numbers
		
		if (flip)
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
		inputSample *= inputgain;
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0.0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//preliminary gain stage using antialiasing
		
		//over to the Iron Oxide shaping code using inputsample
		if (gcount < 0 || gcount > 131) {gcount = 131;}
		count = gcount;
		//increment the counter
		
		dram->d[count+131] = dram->d[count] = inputSample;
		if (flip)
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
		//inputsample side
		//post-center code on inputSample and halfwaySample in parallel
		//begin raw sample- inputSample and ataDrySample handled separately here
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		bridgerectifier = sin(bridgerectifier);
		//can use as an output limiter
		if (inputSample > 0.0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//second stage of overdrive to prevent overs and allow bloody loud extremeness		
		
		randy = (0.55 + tempRandy + ((double(fpd)/UINT32_MAX)*tempRandy))*noise; //0 to 2
		inputSample *= (1.0 - randy);
		inputSample += (prevInputSample*randy);
		prevInputSample = drySample;
		
		flip = !flip;
		
		//begin invdrywet block with outputgain
		if (outputgain != 1.0) inputSample *= outputgain;
		if (invdrywet != 1.0) inputSample *= invdrywet;
		if (dry != 1.0) drySample *= dry;
		if (fabs(drySample) > 0.0) inputSample += drySample;
		//end invdrywet block with outputgain
				
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
	int temp;
	for (temp = 0; temp < 263; temp++) {dram->d[temp] = 0.0;}
	gcount = 0;
	fastIIRA = fastIIRB = slowIIRA = slowIIRB = 0.0;
	fastIIHA = fastIIHB = slowIIHA = slowIIHB = 0.0;
	iirSamplehA = iirSamplehB = 0.0;
	iirSampleA = iirSampleB = 0.0;
	prevInputSample = 0.0;
	flip = false;
	for (temp = 0; temp < 99; temp++) {dram->fl[temp] = 0.0;}
	fstoredcount = 0;	
	sweep = 0.0;
	rateof = 0.5;
	nextmax = 0.5;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
