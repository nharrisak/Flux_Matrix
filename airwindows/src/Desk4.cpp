#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Desk4"
#define AIRWINDOWS_DESCRIPTION "Distinctive analog coloration (a tuneable version of the control-less Desk plugins)"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','t' )
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
{ .name = "Overdrive", .min = 0, .max = 1000, .def = 270, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hi Choke", .min = 0, .max = 1000, .def = 180, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Power Sag", .min = 0, .max = 1000, .def = 260, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Frequency", .min = 0, .max = 1000, .def = 540, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = 0, .max = 1000, .def = 840, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 control;
		int gcount;
		Float64 lastSample;
		Float64 lastOutSample;
		Float64 lastSlew;
		uint32_t fpd;
	
	struct _dram {
			Float64 d[10000];
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
	
	Float64 gain = (pow(GetParameter( kParam_One ),2)*10)+0.0001;
	Float64 gaintrim = (pow(GetParameter( kParam_One ),2)*2)+1.0;
	Float64 slewgain = (pow(GetParameter( kParam_Two ),3)*40)+0.0001;	
	Float64 prevslew = 0.105;
	Float64 intensity = (pow(GetParameter( kParam_Three ),6)*15)+0.0001;
	Float64 depthA = (pow(GetParameter( kParam_Four ),4)*940)+0.00001;
	int offsetA = (int)(depthA * overallscale);
	if (offsetA < 1) offsetA = 1;
	if (offsetA > 4880) offsetA = 4880;
	Float64 balanceB = 0.0001;	
	slewgain *= overallscale;
	prevslew *= overallscale;
	balanceB /= overallscale;
	Float64 outputgain = GetParameter( kParam_Five );
	Float64 wet = GetParameter( kParam_Six );
	//removed unnecessary dry variable
	Float64 clamp;
	Float64 thickness;
	Float64 out;
	Float64 balanceA = 1.0 - balanceB;
	Float64 slew;
	Float64 bridgerectifier;
	Float64 combSample;
	Float64 drySample;
	
	double inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		drySample = inputSample;
		
		
		if (gcount < 0 || gcount > 4900) {gcount = 4900;}
		dram->d[gcount+4900] = dram->d[gcount] = fabs(inputSample)*intensity;
		control += (dram->d[gcount] / offsetA);
		control -= (dram->d[gcount+offsetA] / offsetA);
		control -= 0.000001;
		clamp = 1;
		if (control < 0) {control = 0;}
		if (control > 1) {clamp -= (control - 1); control = 1;}
		if (clamp < 0.5) {clamp = 0.5;}
		gcount--;
		//control = 0 to 1
		thickness = ((1.0 - control) * 2.0) - 1.0;
		
		out = fabs(thickness);		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		if (thickness > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out);
		//blend according to density control
		
		inputSample *= clamp;
		
		slew = inputSample - lastSample;
		lastSample = inputSample;
		//Set up direct reference for slew
		
		
		bridgerectifier = fabs(slew*slewgain);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		if (slew > 0) slew = bridgerectifier/slewgain;
		else slew = -(bridgerectifier/slewgain);
		
		inputSample = (lastOutSample*balanceA) + (lastSample*balanceB) + slew;
		//go from last slewed, but include some raw values
		lastOutSample = inputSample;
		//Set up slewed reference
		
		combSample = fabs(drySample*lastSample);
		if (combSample > 1.0) combSample = 1.0;
		//bailout for very high input gains
		inputSample -= (lastSlew * combSample * prevslew);
		lastSlew = slew;
		//slew interaction with previous slew
		
		inputSample *= gain;
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.0;
		else bridgerectifier = sin(bridgerectifier);
		
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;
		//drive section
		inputSample /= gain;
		inputSample *= gaintrim;
		//end of Desk section
		//end second half

		if (outputgain != 1.0) {
			inputSample *= outputgain;
		}
		
		if (wet !=1.0) {
			inputSample = (inputSample * wet) + (drySample * (1.0-wet));
		}
		
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
	for(int count = 0; count < 9999; count++) {dram->d[count] = 0;}
	control = 0;
	gcount = 0;
	lastSample = 0.0;
	lastOutSample = 0.0;
	lastSlew = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
