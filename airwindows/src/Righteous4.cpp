#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Righteous4"
#define AIRWINDOWS_DESCRIPTION "A final output stage for targeting dynamic range."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','i','g' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
static const int k16bit = 1;
static const int k24bit = 2;
static const int k32bit = 3;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static char const * const enumStrings1[] = { "", "16 bit", "24 bit", "32 bit", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Loudness Target", .min = -28000, .max = -4000, .def = -13000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Bit Depth", .min = 1, .max = 3, .def = 2, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings1 },
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
 
		
		Float64 midSampleA;
		Float64 midSampleB;
		Float64 midSampleC;
		Float64 midSampleD;
		Float64 midSampleE;
		Float64 midSampleF;
		Float64 midSampleG;
		Float64 midSampleH;
		Float64 midSampleI;
		Float64 midSampleJ;
		Float64 midSampleK;
		Float64 midSampleL;
		Float64 midSampleM;
		Float64 midSampleN;
		Float64 midSampleO;
		Float64 midSampleP;
		Float64 midSampleQ;
		Float64 midSampleR;
		Float64 midSampleS;
		Float64 midSampleT;
		Float64 midSampleU;
		Float64 midSampleV;
		Float64 midSampleW;
		Float64 midSampleX;
		Float64 midSampleY;
		Float64 midSampleZ;
		
		Float64 byn[13];
		double noiseShaping;
		
		Float64 lastSample;
		Float64 IIRsample;
		
		Float64 gwPrev;
		Float64 gwA;
		Float64 gwB;
		
		uint32_t fpd;		
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
	double fpOld = 0.618033988749894848204586; //golden ratio!
	double fpNew = 1.0 - fpOld;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 IIRscaleback = 0.0002597;//scaleback of harmonic avg
	IIRscaleback /= overallscale;
	IIRscaleback = 1.0 - IIRscaleback;
		
	Float64 target = GetParameter( kParam_One );
	target += 17; //gives us scaled distortion factor based on test conditions
	target = pow(10.0,target/20.0); //we will multiply and divide by this
	//ShortBuss section
	if (target == 0) target = 1; //insanity check
	
	int bitDepth = (int) GetParameter( kParam_Two ); // +1 for Reaper bug workaround
	
	Float64 fusswithscale = 149940.0; //corrected
	Float64 cutofffreq = 20; //was 46/2.0
	Float64 midAmount = (cutofffreq)/fusswithscale;
	midAmount /= overallscale;
	Float64 midaltAmount = 1.0 - midAmount;
	Float64 gwAfactor = 0.718;
	gwAfactor -= (overallscale*0.05); //0.2 at 176K, 0.1 at 88.2K, 0.05 at 44.1K
	//reduce slightly to not less than 0.5 to increase effect
	Float64 gwBfactor = 1.0 - gwAfactor;
	Float64 softness = 0.2135;
	Float64 hardness = 1.0 - softness;
	Float64 refclip = pow(10.0,-0.0058888);	
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		Float64 drySample = inputSample;
		
		//begin the whole distortion dealiebop
		inputSample /= target;
		
		//running shortbuss on direct sample
		IIRsample *= IIRscaleback;
		Float64 secondharmonic = sin((2 * inputSample * inputSample) * IIRsample);
		//secondharmonic is calculated before IIRsample is updated, to delay reaction
		
		double bridgerectifier = inputSample;
		if (bridgerectifier > 1.2533141373155) bridgerectifier = 1.2533141373155;
		if (bridgerectifier < -1.2533141373155) bridgerectifier = -1.2533141373155;
		//clip to 1.2533141373155 to reach maximum output
		bridgerectifier = sin(bridgerectifier * fabs(bridgerectifier)) / ((bridgerectifier == 0.0) ?1:fabs(bridgerectifier));
		if (inputSample > bridgerectifier) IIRsample += ((inputSample - bridgerectifier)*0.0009);
		if (inputSample < -bridgerectifier) IIRsample += ((inputSample + bridgerectifier)*0.0009);
		//manipulate IIRSample
		inputSample = bridgerectifier;
		//apply the distortion transform for reals. Has been converted back to -1/1
		
		//apply resonant highpass
		Float64 tempSample = inputSample;
		midSampleA = (midSampleA * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleA; Float64 correction = midSampleA;
		midSampleB = (midSampleB * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleB; correction += midSampleB;
		midSampleC = (midSampleC * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleC; correction += midSampleC;
		midSampleD = (midSampleD * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleD; correction += midSampleD;
		midSampleE = (midSampleE * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleE; correction += midSampleE;
		midSampleF = (midSampleF * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleF; correction += midSampleF;
		midSampleG = (midSampleG * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleG; correction += midSampleG;
		midSampleH = (midSampleH * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleH; correction += midSampleH;
		midSampleI = (midSampleI * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleI; correction += midSampleI;
		midSampleJ = (midSampleJ * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleJ; correction += midSampleJ;
		midSampleK = (midSampleK * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleK; correction += midSampleK;
		midSampleL = (midSampleL * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleL; correction += midSampleL;
		midSampleM = (midSampleM * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleM; correction += midSampleM;
		midSampleN = (midSampleN * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleN; correction += midSampleN;
		midSampleO = (midSampleO * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleO; correction += midSampleO;
		midSampleP = (midSampleP * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleP; correction += midSampleP;
		midSampleQ = (midSampleQ * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleQ; correction += midSampleQ;
		midSampleR = (midSampleR * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleR; correction += midSampleR;
		midSampleS = (midSampleS * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleS; correction += midSampleS;
		midSampleT = (midSampleT * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleT; correction += midSampleT;
		midSampleU = (midSampleU * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleU; correction += midSampleU;
		midSampleV = (midSampleV * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleV; correction += midSampleV;
		midSampleW = (midSampleW * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleW; correction += midSampleW;
		midSampleX = (midSampleX * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleX; correction += midSampleX;
		midSampleY = (midSampleY * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleY; correction += midSampleY;
		midSampleZ = (midSampleZ * midaltAmount) + (tempSample * midAmount); tempSample -= midSampleZ; correction += midSampleZ;
		correction *= fabs(secondharmonic);
		//scale it directly by second harmonic: DC block is now adding harmonics too
		correction -= secondharmonic*fpOld;
		//apply the shortbuss processing to output DCblock by subtracting it 
		//we are not a peak limiter! not using it to clip or nothin'
		//adding it inversely, it's the same as adding to inputsample only we are accumulating 'stuff' in 'correction'
		inputSample -= correction;
		if (inputSample < 0) inputSample = (inputSample * fpNew) - (sin(-inputSample)*fpOld);
		//lastly, class A clipping on the negative to combat the one-sidedness
		//uses bloom/antibloom to dial in previous unconstrained behavior
		//end the whole distortion dealiebop
		inputSample *= target;
		//begin simplified Groove Wear, outside the scaling
		//varies depending on what sample rate you're at:
		//high sample rate makes it more airy
		gwB = gwA; gwA = tempSample = (inputSample-gwPrev);
		tempSample *= gwAfactor;
		tempSample += (gwB * gwBfactor);
		correction = (inputSample-gwPrev) - tempSample;
		gwPrev = inputSample;		
		inputSample -= correction;		
		//simplified Groove Wear.
		
		//begin simplified ADClip
		drySample = inputSample;
		if (lastSample >= refclip)
		{
			if (inputSample < refclip)
			{
				lastSample = ((refclip*hardness) + (inputSample * softness));
			}
			else lastSample = refclip;
		}
		
		if (lastSample <= -refclip)
		{
			if (inputSample > -refclip)
			{
				lastSample = ((-refclip*hardness) + (inputSample * softness));
			}
			else lastSample = -refclip;
		}
		
		if (inputSample > refclip)
		{
			if (lastSample < refclip)
			{
				inputSample = ((refclip*hardness) + (lastSample * softness));
			}
			else inputSample = refclip;
		}
		
		if (inputSample < -refclip)
		{
			if (lastSample > -refclip)
			{
				inputSample = ((-refclip*hardness) + (lastSample * softness));
			}
			else inputSample = -refclip;
		}
		lastSample = drySample;
				
		//output dither section
		if (bitDepth == 3) {
			//begin 32 bit floating point dither
			int expon; frexpf((float)inputSample, &expon);
			fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
			inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
			//end 32 bit floating point dither
		} else {
			//entire Naturalize section used when not on 32 bit out
			
			inputSample -= noiseShaping;
			if (bitDepth == 2) inputSample *= 8388608.0; //go to dither at 24 bit
			if (bitDepth == 1) inputSample *= 32768.0; //go to dither at 16 bit
			
			Float64 benfordize = floor(inputSample);
			while (benfordize >= 1.0) {benfordize /= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			int hotbinA = floor(benfordize);
			//hotbin becomes the Benford bin value for this number floored
			Float64 totalA = 0;
			if ((hotbinA > 0) && (hotbinA < 10))
			{
				byn[hotbinA] += 1;
				totalA += (301-byn[1]);
				totalA += (176-byn[2]);
				totalA += (125-byn[3]);
				totalA += (97-byn[4]);
				totalA += (79-byn[5]);
				totalA += (67-byn[6]);
				totalA += (58-byn[7]);
				totalA += (51-byn[8]);
				totalA += (46-byn[9]);
				byn[hotbinA] -= 1;
			} else {hotbinA = 10;}
			//produce total number- smaller is closer to Benford real
			
			benfordize = ceil(inputSample);
			while (benfordize >= 1.0) {benfordize /= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			if (benfordize < 1.0) {benfordize *= 10;}
			int hotbinB = floor(benfordize);
			//hotbin becomes the Benford bin value for this number ceiled
			Float64 totalB = 0;
			if ((hotbinB > 0) && (hotbinB < 10))
			{
				byn[hotbinB] += 1;
				totalB += (301-byn[1]);
				totalB += (176-byn[2]);
				totalB += (125-byn[3]);
				totalB += (97-byn[4]);
				totalB += (79-byn[5]);
				totalB += (67-byn[6]);
				totalB += (58-byn[7]);
				totalB += (51-byn[8]);
				totalB += (46-byn[9]);
				byn[hotbinB] -= 1;
			} else {hotbinB = 10;}
			//produce total number- smaller is closer to Benford real
			
			if (totalA < totalB)
			{
				byn[hotbinA] += 1;
				inputSample = floor(inputSample);
			}
			else
			{
				byn[hotbinB] += 1;
				inputSample = ceil(inputSample);
			}
			//assign the relevant one to the delay line
			//and floor/ceil signal accordingly
			
			totalA = byn[1] + byn[2] + byn[3] + byn[4] + byn[5] + byn[6] + byn[7] + byn[8] + byn[9];
			totalA /= 1000;
			totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
			byn[1] /= totalA;
			byn[2] /= totalA;
			byn[3] /= totalA;
			byn[4] /= totalA;
			byn[5] /= totalA;
			byn[6] /= totalA;
			byn[7] /= totalA;
			byn[8] /= totalA;
			byn[9] /= totalA;
			byn[10] /= 2; //catchall for garbage data
			
			if (bitDepth == 2) inputSample /= 8388608.0;
			if (bitDepth == 1) inputSample /= 32768.0;
			noiseShaping += inputSample - drySample;
		}
		
		if (inputSample > refclip) inputSample = refclip;
		if (inputSample < -refclip) inputSample = -refclip;
		//iron bar prohibits any overs
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	midSampleA = 0.0;
	midSampleB = 0.0;
	midSampleC = 0.0;
	midSampleD = 0.0;
	midSampleE = 0.0;
	midSampleF = 0.0;
	midSampleG = 0.0;
	midSampleH = 0.0;
	midSampleI = 0.0;
	midSampleJ = 0.0;
	midSampleK = 0.0;
	midSampleL = 0.0;
	midSampleM = 0.0;
	midSampleN = 0.0;
	midSampleO = 0.0;
	midSampleP = 0.0;
	midSampleQ = 0.0;
	midSampleR = 0.0;
	midSampleS = 0.0;
	midSampleT = 0.0;
	midSampleU = 0.0;
	midSampleV = 0.0;
	midSampleW = 0.0;
	midSampleX = 0.0;
	midSampleY = 0.0;
	midSampleZ = 0.0;
	
	byn[0] = 1000;
	byn[1] = 301;
	byn[2] = 176;
	byn[3] = 125;
	byn[4] = 97;
	byn[5] = 79;
	byn[6] = 67;
	byn[7] = 58;
	byn[8] = 51;
	byn[9] = 46;
	byn[10] = 1000;
	noiseShaping = 0.0;
	
	lastSample = 0.0;
	IIRsample = 0.0;
	gwPrev = 0.0;
	gwA = 0.0;
	gwB = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
