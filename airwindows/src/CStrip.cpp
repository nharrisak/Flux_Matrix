#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "CStrip"
#define AIRWINDOWS_DESCRIPTION "An Airwindows channel strip."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','S','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	kParam_Seven =6,
	kParam_Eight =7,
	kParam_Nine =8,
	kParam_Ten =9,
	kParam_Eleven =10,
	kParam_Twelve =11,
	//Add your parameters here...
	kNumberOfParameters=12
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Treble", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 1000, .max = 16000, .def = 16000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble Freq", .min = 1000, .max = 16000, .def = 6000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Freq", .min = 300, .max = 16000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling10, .enumStrings = NULL },
{ .name = "Highpass", .min = 300, .max = 16000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling10, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Compression", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Comp Speed", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Timing Lag", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Gain", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 fpNShape;
		uint32_t fpd;
		
		Float64 lastSample;
		Float64 last2Sample;
		
		//begin EQ
		Float64 iirHighSampleA;
		Float64 iirHighSampleB;
		Float64 iirHighSampleC;
		Float64 iirHighSampleD;
		Float64 iirHighSampleE;
		Float64 iirLowSampleA;
		Float64 iirLowSampleB;
		Float64 iirLowSampleC;
		Float64 iirLowSampleD;
		Float64 iirLowSampleE;
		Float64 iirHighSample;
		Float64 iirLowSample;
		
		Float64 tripletA;
		Float64 tripletB;
		Float64 tripletC;
		Float64 tripletFactor;
		
		Float64 lowpassSampleAA;
		Float64 lowpassSampleAB;
		Float64 lowpassSampleBA;
		Float64 lowpassSampleBB;
		Float64 lowpassSampleCA;
		Float64 lowpassSampleCB;
		Float64 lowpassSampleDA;
		Float64 lowpassSampleDB;
		Float64 lowpassSampleE;
		Float64 lowpassSampleF;
		Float64 lowpassSampleG;
		
		Float64 highpassSampleAA;
		Float64 highpassSampleAB;
		Float64 highpassSampleBA;
		Float64 highpassSampleBB;
		Float64 highpassSampleCA;
		Float64 highpassSampleCB;
		Float64 highpassSampleDA;
		Float64 highpassSampleDB;
		Float64 highpassSampleE;
		Float64 highpassSampleF;
		
		bool flip;
		int flipthree;
		//end EQ
		
		//begin Gate
		bool WasNegative;
		int ZeroCross;
		Float64 gateroller;
		Float64 gate;
		//end Gate
		
		//begin Timing
		int count;
		//end Timing
		
		//begin ButterComp
		Float64 controlApos;
		Float64 controlAneg;
		Float64 controlBpos;
		Float64 controlBneg;
		Float64 targetpos;
		Float64 targetneg;
		Float64 avgA;
		Float64 avgB;
		Float64 nvgA;
		Float64 nvgB;
		//end ButterComp
		//flip is already covered in EQ		
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 p[4099];
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	Float64 compscale = overallscale;
	overallscale = GetSampleRate();
	compscale = compscale * overallscale;
	//compscale is the one that's 1 or something like 2.2 for 96K rates
	double fpOld = 0.618033988749894848204586; //golden ratio!
	double fpNew = 1.0 - fpOld;
	
	Float64 inputSample;
	Float64 highSample = 0.0;
	Float64 midSample = 0.0;
	Float64 bassSample = 0.0;
	Float64 densityA = GetParameter( kParam_One )/2.0;
	Float64 densityB = GetParameter( kParam_Two )/2.0;
	Float64 densityC = GetParameter( kParam_Three )/2.0;
	bool engageEQ = true;
	if ( (0.0 == densityA) && (0.0 == densityB) && (0.0 == densityC) ) engageEQ = false;
	
	densityA = pow(10.0,densityA/20.0)-1.0;
	densityB = pow(10.0,densityB/20.0)-1.0;
	densityC = pow(10.0,densityC/20.0)-1.0;
	//convert to 0 to X multiplier with 1.0 being O db
	//minus one gives nearly -1 to ? (should top out at 1)
	//calibrate so that X db roughly equals X db with maximum topping out at 1 internally
	
	Float64 tripletIntensity = -densityA;
	
	Float64 iirAmountC = (GetParameter( kParam_Four )*0.0188) + 0.7;
	if (iirAmountC > 1.0) iirAmountC = 1.0;
	bool engageLowpass = false;
	if (GetParameter( kParam_Four ) < 15.99) engageLowpass = true;
	
	Float64 iirAmountA = (GetParameter( kParam_Five )*1000)/overallscale;
	Float64 iirAmountB = (GetParameter( kParam_Six )*10)/overallscale;
	Float64 iirAmountD = (GetParameter( kParam_Seven )*1.0)/overallscale;
	bool engageHighpass = false;
	if (GetParameter( kParam_Seven ) > 30.01) engageHighpass = true;
	//bypass the highpass and lowpass if set to extremes
	Float64 bridgerectifier;
	Float64 outA = fabs(densityA);
	Float64 outB = fabs(densityB);
	Float64 outC = fabs(densityC);
	//end EQ
	//begin Gate
	Float64 onthreshold = (pow(GetParameter( kParam_Eight ),4)/3)+0.00018;
	Float64 offthreshold = onthreshold * 1.1;
	bool engageGate = false;
	if (onthreshold > 0.00018) engageGate = true;
	
	Float64 release = 0.028331119964586;
	Float64 absmax = 220.9;
	//speed to be compensated w.r.t sample rate
	//end Gate
	//begin Timing
	Float64 offset = pow(GetParameter( kParam_Eleven ),5) * 700;
	int near = (int)floor(fabs(offset));
	Float64 farLevel = fabs(offset) - near;
	int far = near + 1;
	Float64 nearLevel = 1.0 - farLevel;
	bool engageTiming = false;
	if (offset > 0.0) engageTiming = true;
	//end Timing
	//begin ButterComp
	Float64 inputpos;
	Float64 inputneg;
	Float64 calcpos;
	Float64 calcneg;
	Float64 outputpos;
	Float64 outputneg;
	Float64 totalmultiplier;
	Float64 inputgain = (pow(GetParameter( kParam_Nine ),4)*35)+1.0;
	Float64 compoutgain = inputgain;
	compoutgain -= 1.0;
	compoutgain /= 1.2;
	compoutgain += 1.0;
	Float64 divisor = (0.008 * pow(GetParameter( kParam_Ten ),2))+0.0004;
	//originally 0.012
	divisor /= compscale;
	Float64 remainder = divisor;
	divisor = 1.0 - divisor;
	bool engageComp = false;
	if (inputgain > 1.0) engageComp = true;
	//end ButterComp
	Float64 outputgain = pow(10.0,GetParameter( kParam_Twelve )/20.0);
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		last2Sample = lastSample;
		lastSample = inputSample;
		
		//begin Gate
		if (engageGate)
		{
			if (inputSample > 0)
			{if (WasNegative == true){ZeroCross = absmax * 0.3;}
				WasNegative = false;}
			else
			{ZeroCross += 1; WasNegative = true;}
			
			if (ZeroCross > absmax)
			{ZeroCross = absmax;}
			
			if (gate == 0.0)
			{
				//if gate is totally silent
				if (fabs(inputSample) > onthreshold)
				{
					if (gateroller == 0.0) gateroller = ZeroCross;
					else gateroller -= release;
					// trigger from total silence only- if we're active then signal must clear offthreshold
				}
				else gateroller -= release;
			}
			else
			{
				//gate is not silent but closing
				if (fabs(inputSample) > offthreshold)
				{
					if (gateroller < ZeroCross) gateroller = ZeroCross;
					else gateroller -= release;
					//always trigger if gate is over offthreshold, otherwise close anyway
				}
				else gateroller -= release;
			}
			
			if (gateroller < 0.0)
			{gateroller = 0.0;}
			
			if (gateroller < 1.0)
			{
				gate = gateroller;
				bridgerectifier = 1-cos(fabs(inputSample));			
				if (inputSample > 0) inputSample = (inputSample*gate)+(bridgerectifier*(1-gate));
				else inputSample = (inputSample*gate)-(bridgerectifier*(1-gate));
				if (gate == 0.0) inputSample = 0.0;			
			}
			else
			{gate = 1.0;}
		}
		//end Gate, begin antialiasing
				
		flip = !flip;
		flipthree++;
		if (flipthree < 1 || flipthree > 3) flipthree = 1;
		//counters
		
		//begin highpass
		if (engageHighpass)
		{
			if (flip)
			{
				highpassSampleAA = (highpassSampleAA * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleAA;
				highpassSampleBA = (highpassSampleBA * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleBA;
				highpassSampleCA = (highpassSampleCA * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleCA;
				highpassSampleDA = (highpassSampleDA * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleDA;
			}
			else
			{
				highpassSampleAB = (highpassSampleAB * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleAB;
				highpassSampleBB = (highpassSampleBB * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleBB;
				highpassSampleCB = (highpassSampleCB * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleCB;
				highpassSampleDB = (highpassSampleDB * (1 - iirAmountD)) + (inputSample * iirAmountD);
				inputSample = inputSample - highpassSampleDB;
			}
			highpassSampleE = (highpassSampleE * (1 - iirAmountD)) + (inputSample * iirAmountD);
			inputSample = inputSample - highpassSampleE;
			highpassSampleF = (highpassSampleF * (1 - iirAmountD)) + (inputSample * iirAmountD);
			inputSample = inputSample - highpassSampleF;			
			
		}
		//end highpass 
		
		//begin compressor
		if (engageComp)
		{
			inputSample *= inputgain;
			
			inputpos = (inputSample * fpOld) + (avgA * fpNew) + 1.0;
			avgA = inputSample;
			
			if (inputpos < 0.0) inputpos = 0.0;
			outputpos = inputpos / 2.0;
			if (outputpos > 1.0) outputpos = 1.0;		
			inputpos *= inputpos;
			targetpos *= divisor;
			targetpos += (inputpos * remainder);
			calcpos = pow((1.0/targetpos),2);
			
			inputneg = (-inputSample * fpOld) + (nvgA * fpNew) + 1.0;
			nvgA = -inputSample;
			
			if (inputneg < 0.0) inputneg = 0.0;
			outputneg = inputneg / 2.0;
			if (outputneg > 1.0) outputneg = 1.0;		
			inputneg *= inputneg;
			targetneg *= divisor;
			targetneg += (inputneg * remainder);
			calcneg = pow((1.0/targetneg),2);
			//now we have mirrored targets for comp
			//outputpos and outputneg go from 0 to 1
			
			if (inputSample > 0)
			{ //working on pos
				if (true == flip)
				{
					controlApos *= divisor;
					controlApos += (calcpos*remainder);
					
				}
				else
				{
					controlBpos *= divisor;
					controlBpos += (calcpos*remainder);
				}	
			}
			else
			{ //working on neg
				if (true == flip)
				{
					controlAneg *= divisor;
					controlAneg += (calcneg*remainder);
				}
				else
				{
					controlBneg *= divisor;
					controlBneg += (calcneg*remainder);
				}
			}
			//this causes each of the four to update only when active and in the correct 'flip'
			
			if (true == flip)
			{totalmultiplier = (controlApos * outputpos) + (controlAneg * outputneg);}
			else
			{totalmultiplier = (controlBpos * outputpos) + (controlBneg * outputneg);}
			//this combines the sides according to flip, blending relative to the input value
			
			inputSample *= totalmultiplier;
			inputSample /= compoutgain;
		}
		//end compressor
		
		//begin EQ
		if (engageEQ)
		{
			switch (flipthree)
			{
				case 1:
					tripletFactor = last2Sample - inputSample;
					tripletA += tripletFactor;
					tripletC -= tripletFactor;
					tripletFactor = tripletA * tripletIntensity;
					iirHighSampleC = (iirHighSampleC * (1 - iirAmountA)) + (inputSample * iirAmountA);
					highSample = inputSample - iirHighSampleC;
					iirLowSampleC = (iirLowSampleC * (1 - iirAmountB)) + (inputSample * iirAmountB);
					bassSample = iirLowSampleC;
					break;
				case 2:
					tripletFactor = last2Sample - inputSample;
					tripletB += tripletFactor;
					tripletA -= tripletFactor;
					tripletFactor = tripletB * tripletIntensity;
					iirHighSampleD = (iirHighSampleD * (1 - iirAmountA)) + (inputSample * iirAmountA);
					highSample = inputSample - iirHighSampleD;
					iirLowSampleD = (iirLowSampleD * (1 - iirAmountB)) + (inputSample * iirAmountB);
					bassSample = iirLowSampleD;
					break;
				case 3:
					tripletFactor = last2Sample - inputSample;
					tripletC += tripletFactor;
					tripletB -= tripletFactor;
					tripletFactor = tripletC * tripletIntensity;
					iirHighSampleE = (iirHighSampleE * (1 - iirAmountA)) + (inputSample * iirAmountA);
					highSample = inputSample - iirHighSampleE;
					iirLowSampleE = (iirLowSampleE * (1 - iirAmountB)) + (inputSample * iirAmountB);
					bassSample = iirLowSampleE;
					break;
			}
			tripletA /= 2.0;
			tripletB /= 2.0;
			tripletC /= 2.0;
			highSample = highSample + tripletFactor;
			
			if (flip)
			{
				iirHighSampleA = (iirHighSampleA * (1 - iirAmountA)) + (highSample * iirAmountA);
				highSample = highSample - iirHighSampleA;
				iirLowSampleA = (iirLowSampleA * (1 - iirAmountB)) + (bassSample * iirAmountB);
				bassSample = iirLowSampleA;
			}
			else
			{
				iirHighSampleB = (iirHighSampleB * (1 - iirAmountA)) + (highSample * iirAmountA);
				highSample = highSample - iirHighSampleB;
				iirLowSampleB = (iirLowSampleB * (1 - iirAmountB)) + (bassSample * iirAmountB);
				bassSample = iirLowSampleB;
			}
			iirHighSample = (iirHighSample * (1 - iirAmountA)) + (highSample * iirAmountA);
			highSample = highSample - iirHighSample;
			iirLowSample = (iirLowSample * (1 - iirAmountB)) + (bassSample * iirAmountB);
			bassSample = iirLowSample;
			
			midSample = (inputSample-bassSample)-highSample;
			
			//drive section
			highSample *= (densityA+1.0);
			bridgerectifier = fabs(highSample)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			//max value for sine function
			if (densityA > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (highSample > 0) highSample = (highSample*(1-outA))+(bridgerectifier*outA);
			else highSample = (highSample*(1-outA))-(bridgerectifier*outA);
			//blend according to densityA control
			
			midSample *= (densityB+1.0);
			bridgerectifier = fabs(midSample)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			//max value for sine function
			if (densityB > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (midSample > 0) midSample = (midSample*(1-outB))+(bridgerectifier*outB);
			else midSample = (midSample*(1-outB))-(bridgerectifier*outB);
			//blend according to densityB control
			
			bassSample *= (densityC+1.0);
			bridgerectifier = fabs(bassSample)*1.57079633;
			if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
			//max value for sine function
			if (densityC > 0) bridgerectifier = sin(bridgerectifier);
			else bridgerectifier = 1-cos(bridgerectifier);
			//produce either boosted or starved version
			if (bassSample > 0) bassSample = (bassSample*(1-outC))+(bridgerectifier*outC);
			else bassSample = (bassSample*(1-outC))-(bridgerectifier*outC);
			//blend according to densityC control
			
			inputSample = midSample;
			inputSample += highSample;
			inputSample += bassSample;
		}
		//end EQ
		
		//begin Timing
		if (engageTiming)
		{
			if (count < 1 || count > 2048) {count = 2048;}
			dram->p[count+2048] = dram->p[count] = inputSample;
			inputSample = dram->p[count+near]*nearLevel;
			inputSample += dram->p[count+far]*farLevel;
			count -= 1;
			//consider adding third sample just to bring out superhighs subtly, like old interpolation hacks
			//or third and fifth samples, ditto		
		}
		//end Timing
		
		//EQ lowpass is after all processing like the compressor that might produce hash
		if (engageLowpass)
		{
			if (flip)
			{
				lowpassSampleAA = (lowpassSampleAA * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleAA;
				lowpassSampleBA = (lowpassSampleBA * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleBA;
				lowpassSampleCA = (lowpassSampleCA * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleCA;
				lowpassSampleDA = (lowpassSampleDA * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleDA;
				lowpassSampleE = (lowpassSampleE * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleE;
			}
			else
			{
				lowpassSampleAB = (lowpassSampleAB * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleAB;
				lowpassSampleBB = (lowpassSampleBB * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleBB;
				lowpassSampleCB = (lowpassSampleCB * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleCB;
				lowpassSampleDB = (lowpassSampleDB * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleDB;
				lowpassSampleF = (lowpassSampleF * (1 - iirAmountC)) + (inputSample * iirAmountC);
				inputSample = lowpassSampleF;			
			}
			lowpassSampleG = (lowpassSampleG * (1 - iirAmountC)) + (inputSample * iirAmountC);
			inputSample = (lowpassSampleG * (1 - iirAmountC)) + (inputSample * iirAmountC);
		}
				
		//built in output trim and dry/wet if desired
		if (outputgain != 1.0) inputSample *= outputgain;
		
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
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	
	lastSample = 0.0;
	last2Sample = 0.0;
	
	iirHighSampleA = 0.0;
	iirHighSampleB = 0.0;
	iirHighSampleC = 0.0;
	iirHighSampleD = 0.0;
	iirHighSampleE = 0.0;
	iirLowSampleA = 0.0;
	iirLowSampleB = 0.0;
	iirLowSampleC = 0.0;
	iirLowSampleD = 0.0;
	iirLowSampleE = 0.0;
	iirHighSample = 0.0;
	iirLowSample = 0.0;
	tripletA = 0.0;
	tripletB = 0.0;
	tripletC = 0.0;
	tripletFactor = 0.0;
	flip = false;
	flipthree = 0;
	
	lowpassSampleAA = 0.0;
	lowpassSampleAB = 0.0;
	lowpassSampleBA = 0.0;
	lowpassSampleBB = 0.0;
	lowpassSampleCA = 0.0;
	lowpassSampleCB = 0.0;
	lowpassSampleDA = 0.0;
	lowpassSampleDB = 0.0;
	lowpassSampleE = 0.0;
	lowpassSampleF = 0.0;
	lowpassSampleG = 0.0;
	
	highpassSampleAA = 0.0;
	highpassSampleAB = 0.0;
	highpassSampleBA = 0.0;
	highpassSampleBB = 0.0;
	highpassSampleCA = 0.0;
	highpassSampleCB = 0.0;
	highpassSampleDA = 0.0;
	highpassSampleDB = 0.0;
	highpassSampleE = 0.0;
	highpassSampleF = 0.0;
	//end EQ
	//begin Gate
	WasNegative = false;
	ZeroCross = 0;
	gateroller = 0.0;
	gate = 0.0;
	//end Gate
	//begin Timing
	register UInt32 fcount;
	for(fcount = 0; fcount < 4098; fcount++) {dram->p[fcount] = 0.0;}
	count = 0;
	//end Timing
	//begin ButterComp
	controlApos = 1.0;
	controlAneg = 1.0;
	controlBpos = 1.0;
	controlBneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;	
	avgA = avgB = 0.0;
	nvgA = nvgB = 0.0;
	//end ButterComp
}
};
