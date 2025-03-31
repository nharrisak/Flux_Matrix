#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BassAmp"
#define AIRWINDOWS_DESCRIPTION "An old plugin with new tweaks, that gives some bass guitar tones."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','s' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "High", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dub", .min = 0, .max = 1000, .def = 750, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sub", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
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
		
		Float64 iirDriveSampleA;
		Float64 iirDriveSampleB;
		Float64 iirDriveSampleC;
		Float64 iirDriveSampleD;
		Float64 iirDriveSampleE;
		Float64 iirDriveSampleF;
		bool flip; //drive things
		
		int bflip;
		bool WasNegative;
		bool SubOctave;
		Float64 iirHeadBumpA;
		Float64 iirHeadBumpB;
		Float64 iirHeadBumpC;
		
		Float64 iirSubBumpA;
		Float64 iirSubBumpB;
		Float64 iirSubBumpC;
				
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
	
	Float64 contHigh = GetParameter( kParam_One );
	Float64 dry = GetParameter( kParam_Two );
	Float64 contDub = GetParameter( kParam_Three )*1.3;
	Float64 contSub = GetParameter( kParam_Four )/2.0;
	
	Float64 driveone = pow(contHigh*3.0,2);
	Float64 driveoutput = contHigh;
	Float64 iirAmount = 0.344/overallscale;
	Float64 altAmount = 1.0 - iirAmount;	
	Float64 BassGain = contDub * 0.1;
	Float64 HeadBumpFreq = (BassGain+0.0001)/overallscale;
	Float64 BassOutGain = contDub * 0.2;
	Float64 SubGain = contSub * 0.1;
	Float64 SubBumpFreq = (SubGain+0.0001)/overallscale;
	Float64 SubOutGain = contSub * 0.3;
	Float64 iirHPAmount = 0.0000014/overallscale;
	Float64 altHPAmount = 1.0 - iirHPAmount;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		ataDrySample = inputSample = *sourceP;
		ataHalfDrySample = ataHalfwaySample = (inputSample + ataLast1Sample + (ataLast2Sample*ataK1) + (ataLast3Sample*ataK2) + (ataLast4Sample*ataK6) + (ataLast5Sample*ataK7) + (ataLast6Sample*ataK8)) / 2.0;
		ataLast6Sample = ataLast5Sample; ataLast5Sample = ataLast4Sample; ataLast4Sample = ataLast3Sample; ataLast3Sample = ataLast2Sample; ataLast2Sample = ataLast1Sample; ataLast1Sample = inputSample;
		//setting up oversampled special antialiasing
		
		Float64 correction;
		Float64 subtractSample;
		Float64 tempSample; //used repeatedly in different places
		
		//begin first half- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
		
		Float64 ataHalfwayLowpass;
		if (flip)
		{
			tempSample = subtractSample = ataHalfwaySample;
			iirDriveSampleA = (iirDriveSampleA * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleA; correction = iirDriveSampleA;
			iirDriveSampleC = (iirDriveSampleC * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleC; correction += iirDriveSampleC;
			iirDriveSampleE = (iirDriveSampleE * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleE; correction += iirDriveSampleE;
			ataHalfwaySample -= correction;
			ataHalfwayLowpass = subtractSample - ataHalfwaySample;
		}
		else
		{
			tempSample = subtractSample = ataHalfwaySample;
			iirDriveSampleB = (iirDriveSampleB * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleB; correction = iirDriveSampleB;
			iirDriveSampleD = (iirDriveSampleD * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleD; correction += iirDriveSampleD;
			iirDriveSampleF = (iirDriveSampleF * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleF; correction += iirDriveSampleF;
			ataHalfwaySample -= correction;			
			ataHalfwayLowpass = subtractSample - ataHalfwaySample;
		}
		//highpass section
		if (ataHalfwaySample > 1.0) {ataHalfwaySample = 1.0;}
		if (ataHalfwaySample < -1.0) {ataHalfwaySample = -1.0;}
		Float64 out = driveone;
		while (out > 0.60)
		{
			out -= 0.60;
			ataHalfwaySample -= (ataHalfwaySample * (fabs(ataHalfwaySample) * 0.60) * (fabs(ataHalfwaySample) * 0.60) );
			ataHalfwaySample *= (1.0+0.60);
		}
		//that's taken care of the really high gain stuff
		
		ataHalfwaySample -= (ataHalfwaySample * (fabs(ataHalfwaySample) * out) * (fabs(ataHalfwaySample) * out) );
		ataHalfwaySample *= (1.0+out);
		
		
		//end first half
		//begin second half- inputSample and ataDrySample handled separately here
		
		Float64 ataLowpass;
		if (flip)
		{
			tempSample = subtractSample = inputSample;
			iirDriveSampleA = (iirDriveSampleA * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleA; correction = iirDriveSampleA;
			iirDriveSampleC = (iirDriveSampleC * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleC; correction += iirDriveSampleC;
			iirDriveSampleE = (iirDriveSampleE * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleE; correction += iirDriveSampleE;
			inputSample -= correction;
			ataLowpass = subtractSample - inputSample;
		}
		else
		{
			tempSample = subtractSample = inputSample;
			iirDriveSampleB = (iirDriveSampleB * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleB; correction = iirDriveSampleB;
			iirDriveSampleD = (iirDriveSampleD * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleD; correction += iirDriveSampleD;
			iirDriveSampleF = (iirDriveSampleF * altAmount) + (tempSample * iirAmount); tempSample -= iirDriveSampleF; correction += iirDriveSampleF;
			inputSample -= correction;
			ataLowpass = subtractSample - inputSample;
		}
		//highpass section
		if (inputSample > 1.0) {inputSample = 1.0;}
		if (inputSample < -1.0) {inputSample = -1.0;}
		out = driveone;
		while (out > 0.60)
		{
			out -= 0.60;
			inputSample -= (inputSample * (fabs(inputSample) * 0.60) * (fabs(inputSample) * 0.60) );
			inputSample *= (1.0+0.60);
		}
		//that's taken care of the really high gain stuff
		
		inputSample -= (inputSample * (fabs(inputSample) * out) * (fabs(inputSample) * out) );
		inputSample *= (1.0+out);
		//end second half
		
		
		ataHalfDrySample = (ataDrySample*ataK3)+(ataHalfDrySample*ataK4);
		ataHalfDiffSample = (ataHalfwaySample - ataHalfDrySample)/2.0;
		ataLastDiffSample = ataDiffSample*ataK5;
		ataDiffSample = (inputSample - ataDrySample)/2.0;
		ataDiffSample += ataHalfDiffSample;
		ataDiffSample -= ataLastDiffSample;
		inputSample = ataDrySample;
		inputSample += ataDiffSample;
		//apply processing as difference to non-oversampled raw input
		
		//now we've got inputSample as the Drive top-end output, and we have ataLowpass and ataHalfwayLowpass
		ataLowpass += ataHalfwayLowpass; //and combined them. Now we make sub-octaves
		
		
		//set up polarities for sub-bass version
		Float64 randy = (double(fpd)/UINT32_MAX)*0.0555; //0 to 1 the noise, may not be needed		
		
		switch (bflip)
		{
			case 1:				
				iirHeadBumpA += (ataLowpass * BassGain);
				iirHeadBumpA -= (iirHeadBumpA * iirHeadBumpA * iirHeadBumpA * HeadBumpFreq);
				iirHeadBumpA = ((1.0-randy) * iirHeadBumpA) + (randy * 0.5 * iirHeadBumpB) + (randy * 0.5 * iirHeadBumpC);
				break;
			case 2:
				iirHeadBumpB += (ataLowpass * BassGain);
				iirHeadBumpB -= (iirHeadBumpB * iirHeadBumpB * iirHeadBumpB * HeadBumpFreq);
				iirHeadBumpB = (randy * 0.5 * iirHeadBumpA) + ((1.0-randy) * iirHeadBumpB) + (randy * 0.5 * iirHeadBumpC);
				break;
			case 3:
				iirHeadBumpC += (ataLowpass * BassGain);
				iirHeadBumpC -= (iirHeadBumpC * iirHeadBumpC * iirHeadBumpC * HeadBumpFreq);
				iirHeadBumpC = (randy * 0.5 * iirHeadBumpA) + (randy * 0.5 * iirHeadBumpB) + ((1.0-randy) * iirHeadBumpC);
				break;
		}
		Float64 HeadBump = iirHeadBumpA + iirHeadBumpB + iirHeadBumpC;
		
		Float64 SubBump = fabs(HeadBump);
		
		if (HeadBump > 0.0) {
			if (WasNegative) SubOctave = !SubOctave;
			WasNegative = false;
		} else WasNegative = true;
		if (SubOctave == false) SubBump = -SubBump;
		
		switch (bflip)
		{
			case 1:				
				iirSubBumpA += (SubBump * SubGain);
				iirSubBumpA -= (iirSubBumpA * iirSubBumpA * iirSubBumpA * SubBumpFreq);
				iirSubBumpA = ((1.0-randy) * iirSubBumpA) + (randy * 0.5 * iirSubBumpB) + (randy * 0.5 * iirSubBumpC);
				break;
			case 2:
				iirSubBumpB += (SubBump * SubGain);
				iirSubBumpB -= (iirSubBumpB * iirSubBumpB * iirSubBumpB * SubBumpFreq);
				iirSubBumpB = (randy * 0.5 * iirSubBumpA) + ((1.0-randy) * iirSubBumpB) + (randy * 0.5 * iirSubBumpC);
				break;
			case 3:
				iirSubBumpC += (SubBump * SubGain);
				iirSubBumpC -= (iirSubBumpC * iirSubBumpC * iirSubBumpC * SubBumpFreq);
				iirSubBumpC = (randy * 0.5 * iirSubBumpA) + (randy * 0.5 * iirSubBumpB) + ((1.0-randy) * iirSubBumpC);
				break;
		}
		SubBump = iirSubBumpA + iirSubBumpB + iirSubBumpC;
		
		
		flip = !flip;
		bflip++;
		if (bflip < 1 || bflip > 3) bflip = 1;
		
		inputSample *= driveoutput;
		correction = ataDrySample*dry;
		correction += (HeadBump * BassOutGain);
		correction += (SubBump * SubOutGain);
		inputSample += correction;
		
		tempSample = inputSample;
		
		iirSampleA = (iirSampleA * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleA; correction = iirSampleA;
		iirSampleB = (iirSampleB * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleB; correction += iirSampleB;
		iirSampleC = (iirSampleC * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleC; correction += iirSampleC;
		iirSampleD = (iirSampleD * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleD; correction += iirSampleD;
		iirSampleE = (iirSampleE * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleE; correction += iirSampleE;
		iirSampleF = (iirSampleF * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleF; correction += iirSampleF;
		iirSampleG = (iirSampleG * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleG; correction += iirSampleG;
		iirSampleH = (iirSampleH * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleH; correction += iirSampleH;
		iirSampleI = (iirSampleI * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleI; correction += iirSampleI;
		iirSampleJ = (iirSampleJ * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleJ; correction += iirSampleJ;
		iirSampleK = (iirSampleK * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleK; correction += iirSampleK;
		iirSampleL = (iirSampleL * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleL; correction += iirSampleL;
		iirSampleM = (iirSampleM * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleM; correction += iirSampleM;
		iirSampleN = (iirSampleN * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleN; correction += iirSampleN;
		iirSampleO = (iirSampleO * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleO; correction += iirSampleO;
		iirSampleP = (iirSampleP * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleP; correction += iirSampleP;
		iirSampleQ = (iirSampleQ * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleQ; correction += iirSampleQ;
		iirSampleR = (iirSampleR * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleR; correction += iirSampleR;
		iirSampleS = (iirSampleS * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleS; correction += iirSampleS;
		iirSampleT = (iirSampleT * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleT; correction += iirSampleT;
		iirSampleU = (iirSampleU * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleU; correction += iirSampleU;
		iirSampleV = (iirSampleV * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleV; correction += iirSampleV;
		iirSampleW = (iirSampleW * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleW; correction += iirSampleW;
		iirSampleX = (iirSampleX * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleX; correction += iirSampleX;
		iirSampleY = (iirSampleY * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleY; correction += iirSampleY;
		iirSampleZ = (iirSampleZ * altHPAmount) + (tempSample * iirHPAmount); tempSample -= iirSampleZ; correction += iirSampleZ;
		//do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
		//numbers being used. Don't keep doing it against the possibly high level signal number.
		inputSample -= correction;
		//apply stored up tiny corrections
		
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
	ataK5 = 0.122; //subtract this much prev. diff sample, brightens
	WasNegative = false;
	SubOctave = false;
	bflip = 0;
	flip = false;
	iirDriveSampleA = 0.0;
	iirDriveSampleB = 0.0;
	iirDriveSampleC = 0.0;
	iirDriveSampleD = 0.0;
	iirDriveSampleE = 0.0;
	iirDriveSampleF = 0.0;
	
	iirHeadBumpA = 0.0;
	iirHeadBumpB = 0.0;
	iirHeadBumpC = 0.0;
	
	iirSubBumpA = 0.0;
	iirSubBumpB = 0.0;
	iirSubBumpC = 0.0;
	
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
