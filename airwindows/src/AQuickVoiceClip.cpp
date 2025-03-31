#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "AQuickVoiceClip"
#define AIRWINDOWS_DESCRIPTION "Softens headset mic recordings that have been super hard clipped on capture."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','A','Q','u' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Highpass", .min = 300, .max = 30000, .def = 1860, .unit = kNT_unitNone, .scaling = kNT_scaling10, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float64 ataLast6Sample;
		Float64 ataLast5Sample;
		Float64 ataLast4Sample;
		Float64 ataLast3Sample;
		Float64 ataLast2Sample;
		Float64 ataLast1Sample;
		Float64 ataHalfwaySample;
		Float64 ataHalfDrySample;
		Float64 ataHalfDiffSample;
		Float64 ataLastDiffSample;
		Float64 ataDrySample;
		Float64 ataDiffSample;
		Float64 ataPrevDiffSample;
		Float64 ataK1;
		Float64 ataK2;
		Float64 ataK3;
		Float64 ataK4;
		Float64 ataK5;
		Float64 ataK6;
		Float64 ataK7;
		Float64 ataK8; //end antialiasing variables
		Float64 lastSample;
		Float64 lastOutSample;
		Float64 lastOut2Sample;
		Float64 lastOut3Sample;
		Float64 lpDepth;
		Float64 overshoot;
		Float64 overall;
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		bool flip;
		Float64 fpNShape;
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
	
	Float64 softness = 0.484416;
	Float64 hardness = 1.0 - softness;
	
	Float64 iirAmount = GetParameter( kParam_One )/8000.0;
	
	iirAmount /= overallscale;
	Float64 altAmount = (1.0 - iirAmount);
	
	Float64 cancelnew = 0.0682276;
	Float64 cancelold = 1.0 - cancelnew;
	
	Float64 maxRecent;
	
	Float64 lpSpeed = 0.0009;

	Float64 cliplevel = 0.98;
	
	Float64 refclip = 0.5; //preset to cut out gain quite a lot. 91%? no touchy unless clip
	
	Float64 inputSample;
	Float64 passThrough;
	Float64 outputSample;
	bool clipOnset;
	Float64 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		passThrough = ataDrySample = inputSample;
		
		
		ataHalfDrySample = ataHalfwaySample = (inputSample + ataLast1Sample + (ataLast2Sample*ataK1) + (ataLast3Sample*ataK2) + (ataLast4Sample*ataK6) + (ataLast5Sample*ataK7) + (ataLast6Sample*ataK8)) / 2.0;
		ataLast6Sample = ataLast5Sample; ataLast5Sample = ataLast4Sample; ataLast4Sample = ataLast3Sample; ataLast3Sample = ataLast2Sample; ataLast2Sample = ataLast1Sample; ataLast1Sample = inputSample;
		//setting up oversampled special antialiasing
		clipOnset = false;
		
		
		maxRecent = fabs( ataLast6Sample );
		if (fabs( ataLast5Sample ) > maxRecent ) maxRecent = fabs( ataLast5Sample );
		if (fabs( ataLast4Sample ) > maxRecent ) maxRecent = fabs( ataLast4Sample );
		if (fabs( ataLast3Sample ) > maxRecent ) maxRecent = fabs( ataLast3Sample );
		if (fabs( ataLast2Sample ) > maxRecent ) maxRecent = fabs( ataLast2Sample );
		if (fabs( ataLast1Sample ) > maxRecent ) maxRecent = fabs( ataLast1Sample );
		if (fabs( inputSample ) > maxRecent ) maxRecent = fabs( inputSample );
		//this gives us something that won't cut out in zero crossings, to interpolate with
		
		maxRecent *= 2.0;
		//by refclip this is 1.0 and fully into the antialiasing
		if (maxRecent > 1.0) maxRecent = 1.0;
		//and it tops out at 1. Higher means more antialiasing, lower blends into passThrough without antialiasing
		
		ataHalfwaySample -= overall;
		//subtract dist-cancel from input after getting raw input, before doing anything
		
		drySample = ataHalfwaySample;
		

		if (lastSample >= refclip)
		{
			lpDepth += 0.1;
			if (ataHalfwaySample < refclip)
			{
				lastSample = ((refclip*hardness) + (ataHalfwaySample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			lpDepth += 0.1;
			if (ataHalfwaySample > -refclip)
			{
				lastSample = ((-refclip*hardness) + (ataHalfwaySample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (ataHalfwaySample > refclip)
		{
			lpDepth += 0.1;
			if (lastSample < refclip)
			{
				ataHalfwaySample = ((refclip*hardness) + (lastSample * softness));
			}
			else ataHalfwaySample = refclip;
		}
		
		if (ataHalfwaySample < -refclip)
		{
			lpDepth += 0.1;
			if (lastSample > -refclip)
			{
				ataHalfwaySample = ((-refclip*hardness) + (lastSample * softness));
			}
			else ataHalfwaySample = -refclip;
		}
				
        outputSample = lastSample;
		lastSample = ataHalfwaySample;
		ataHalfwaySample = outputSample;
		//swap around in a circle for one final ADClip,
		//this time not tracking overshoot anymore
		
		//end interpolated sample
		
		//begin raw sample- inputSample and ataDrySample handled separately here
		
		inputSample -= overall;
		//subtract dist-cancel from input after getting raw input, before doing anything
		
		drySample = inputSample;
		
		if (lastSample >= refclip)
		{
			lpDepth += 0.1;
			if (inputSample < refclip)
			{
				lastSample = ((refclip*hardness) + (inputSample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			lpDepth += 0.1;
			if (inputSample > -refclip)
			{
				lastSample = ((-refclip*hardness) + (inputSample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (inputSample > refclip)
		{
			lpDepth += 0.1;
			if (lastSample < refclip)
			{
				inputSample = ((refclip*hardness) + (lastSample * softness));
			}
			else inputSample = refclip;
		}
		
		if (inputSample < -refclip)
		{
			lpDepth += 0.1;
			if (lastSample > -refclip)
			{
				inputSample = ((-refclip*hardness) + (lastSample * softness));
			}
			else inputSample = -refclip;
		}
		
        outputSample = lastSample;
		lastSample = inputSample;
		inputSample = outputSample;
		
		//end raw sample
		
		ataHalfDrySample = (ataDrySample*ataK3)+(ataHalfDrySample*ataK4);
		ataHalfDiffSample = (ataHalfwaySample - ataHalfDrySample)/2.0;
		ataLastDiffSample = ataDiffSample*ataK5;
		ataDiffSample = (inputSample - ataDrySample)/2.0;
		ataDiffSample += ataHalfDiffSample;
		ataDiffSample -= ataLastDiffSample;
		inputSample = ataDrySample;
		inputSample += ataDiffSample;
		
		
		overall = (overall * cancelold) + (ataDiffSample * cancelnew);
		//apply all the diffs to a lowpassed IIR
		
		if (flip)
		{
			iirSampleA = (iirSampleA * altAmount) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
			iirSampleC = (iirSampleC * altAmount) + (passThrough * iirAmount);
			passThrough -= iirSampleC;
		}
		else
		{
			iirSampleB = (iirSampleB * altAmount) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
			iirSampleD = (iirSampleD * altAmount) + (passThrough * iirAmount);
			passThrough -= iirSampleD;
		}
		flip = !flip;
		//highpass section 
		
		
		lastOut3Sample = lastOut2Sample;
		lastOut2Sample = lastOutSample;
		lastOutSample = inputSample;
		
		lpDepth -= lpSpeed;
		if (lpDepth > 0.0)
		{
			if (lpDepth > 1.0) lpDepth = 1.0;
			inputSample *= (1.0-lpDepth);
			inputSample += (((lastOutSample + lastOut2Sample + lastOut3Sample) / 3.6)*lpDepth);
		}
		if (lpDepth < 0.0) lpDepth = 0.0;
				
		inputSample *= (1.0-maxRecent);
		inputSample += (passThrough * maxRecent);
		//there's our raw signal, without antialiasing. Brings up low level stuff and softens more when hot
		
		if (inputSample > cliplevel) inputSample = cliplevel;
		if (inputSample < -cliplevel) inputSample = -cliplevel;
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
	ataLast6Sample = ataLast5Sample = ataLast4Sample = ataLast3Sample = ataLast2Sample = ataLast1Sample = 0.0;
	ataHalfwaySample = ataHalfDrySample = ataHalfDiffSample = 0.0;
	ataLastDiffSample = ataDrySample = ataDiffSample = ataPrevDiffSample = 0.0;
	ataK1 = -0.646; //first FIR shaping of interpolated sample, brightens
	ataK2 = 0.311; //second FIR shaping of interpolated sample, thickens
	ataK6 = -0.093; //third FIR shaping of interpolated sample, brings air
	ataK7 = 0.057; //fourth FIR shaping of interpolated sample, thickens
	ataK8 = -0.023; //fifth FIR shaping of interpolated sample, brings air
	ataK3 = 0.114; //add raw to interpolated dry, toughens
	ataK4 = 0.886; //remainder of interpolated dry, adds up to 1.0
	ataK5 = 0.431; //subtract this much prev. diff sample, brightens.  0.431 becomes flat
	lastSample = 0.0;
	lastOutSample = 0.0;
	lastOut2Sample = 0.0;
	lastOut3Sample = 0.0;
	lpDepth = 0.0;
	overshoot = 0.0;
	overall = 0;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
