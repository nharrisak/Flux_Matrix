#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PitchDelay"
#define AIRWINDOWS_DESCRIPTION "TapeDelay2 but with pitch shift instead of flutter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','i','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Time", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reso", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pitch", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 prevSample;
		Float64 regenSample;
		Float64 delay;
		Float64 sweep;
		double regenFilter[9];
		double outFilter[9];
		double lastRef[10];
		int cycle;	
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 d[88211];
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
	
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check	
	
	Float64 baseSpeed = (pow(GetParameter( kParam_One ),4)*20.0)+1.0;
	Float64 feedback = GetParameter( kParam_Two )*(3.0-fabs(regenSample*2.0));
	
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	regenFilter[0] = ((pow(GetParameter( kParam_Three ),3)*0.4)+0.0001);
	regenFilter[1] = pow(GetParameter( kParam_Four ),2)+0.01; //resonance
	double K = tan(M_PI * regenFilter[0]);
	double norm = 1.0 / (1.0 + K / regenFilter[1] + K * K);
	regenFilter[2] = K / regenFilter[1] * norm;
	regenFilter[4] = -regenFilter[2];
	regenFilter[5] = 2.0 * (K * K - 1.0) * norm;
	regenFilter[6] = (1.0 - K / regenFilter[1] + K * K) * norm;
	
	//[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
	//[1] is resonance, 0.7071 is Butterworth. Also can't be zero
	outFilter[0] = regenFilter[0];
	outFilter[1] = regenFilter[1] * 1.618033988749894848204586; //resonance
	K = tan(M_PI * outFilter[0]);
	norm = 1.0 / (1.0 + K / outFilter[1] + K * K);
	outFilter[2] = K / outFilter[1] * norm;
	outFilter[4] = -outFilter[2];
	outFilter[5] = 2.0 * (K * K - 1.0) * norm;
	outFilter[6] = (1.0 - K / outFilter[1] + K * K) * norm;

	Float64 vibSpeed = (GetParameter( kParam_Five )-0.5)*61.8;
	Float64 vibDepth = (fabs(vibSpeed)*20.0*baseSpeed)+1.0;
	Float64 wet = GetParameter( kParam_Six )*2.0;
	Float64 dry = 2.0 - wet;
	if (wet > 1.0) wet = 1.0;
	if (wet < 0.0) wet = 0.0;
	if (dry > 1.0) dry = 1.0;
	if (dry < 0.0) dry = 0.0;
	//this echo makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added echo loudness.
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;
		
		cycle++;
		if (cycle == cycleEnd) {
			Float64 speed = baseSpeed;
			
			int pos = floor(delay);
			Float64 newSample = inputSample + (regenSample*feedback);
			double tempSample = (newSample * regenFilter[2]) + regenFilter[7];
			regenFilter[7] = -(tempSample * regenFilter[5]) + regenFilter[8];
			regenFilter[8] = (newSample * regenFilter[4]) - (tempSample * regenFilter[6]);
			newSample = tempSample;
			
			delay -= speed; if (delay < 0) delay += 88200.0;
			Float64 increment = (newSample - prevSample) / speed;
			dram->d[pos] = prevSample;
			while (pos != floor(delay)) {
				dram->d[pos] = prevSample;
				prevSample += increment;
				pos--; if (pos < 0) pos += 88200;
			}
			prevSample = newSample;

			sweep += (0.0001*vibSpeed);
			if (sweep < 0.0) sweep += 6.283185307179586; if (sweep > 6.283185307179586) sweep -= 6.283185307179586;
			Float64 sweepOffset = sweep + M_PI;
			if (sweepOffset > 6.283185307179586) sweepOffset -= 6.283185307179586;
			Float64 newTapA = delay-(sweep*vibDepth); if (newTapA < 0) newTapA += 88200.0;
			Float64 newTapB = delay-(sweepOffset*vibDepth); if (newTapB < 0) newTapB += 88200.0;
			Float64 tapAmplitudeA = (sin(sweep+(M_PI*1.5))+1.0)*0.25;
			Float64 tapAmplitudeB = (sin(sweepOffset+(M_PI*1.5))+1.0)*0.25;
			pos = floor(newTapA); inputSample = dram->d[pos]*tapAmplitudeA;
			pos = floor(newTapB); inputSample += dram->d[pos]*tapAmplitudeB;
			
			regenSample = sin(inputSample);
			
			tempSample = (inputSample * outFilter[2]) + outFilter[7];
			outFilter[7] = -(tempSample * outFilter[5]) + outFilter[8];
			outFilter[8] = (inputSample * outFilter[4]) - (tempSample * outFilter[6]);
			inputSample = tempSample;
			
			
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (wet < 1.0) inputSample *= wet;
		if (dry < 1.0) drySample *= dry;
		inputSample += drySample;
		//this is our submix echo dry/wet: 0.5 is BOTH at FULL VOLUME
		//purpose is that, if you're adding echo, you're not altering other balances
		
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
	for(int x = 0; x < 88210; x++) {dram->d[x] = 0.0;}
	prevSample = 0.0;
	regenSample = 0.0;
	delay = 0.0;
	sweep = 0.0;
	for (int x = 0; x < 9; x++) {regenFilter[x] = 0.0; outFilter[x] = 0.0;}
	for (int x = 0; x < 9; x++) {lastRef[x] = 0.0;}
	cycle = 0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
