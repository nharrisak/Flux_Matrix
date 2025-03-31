#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Crystal"
#define AIRWINDOWS_DESCRIPTION "A tone shaper and buss soft-clipper."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','r','y' )
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
{ .name = "Hardness", .min = 0, .max = 1000, .def = 700, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Personality", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 3000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float64 b[35];
		Float64 lastSample;
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

	Float64 threshold = GetParameter( kParam_One );
	Float64 hardness;
	Float64 breakup = (1.0-(threshold/2.0))*3.14159265358979;
	Float64 bridgerectifier;
	Float64 sqdrive = GetParameter( kParam_Two );
	if (sqdrive > 1.0) sqdrive *= sqdrive;
	sqdrive = sqrt(sqdrive);
	Float64 indrive = GetParameter( kParam_Three );
	if (indrive > 1.0) indrive *= indrive;
	indrive *= (1.0-(0.1695*sqdrive));
	//no gain loss of convolution for APIcolypse
	//calibrate this to match noise level with character at 1.0
	//you get for instance 0.819 and 1.0-0.819 is 0.181
	Float64 randy;
	Float64 outlevel = GetParameter( kParam_Four );

	if (threshold < 1) hardness = 1.0 / (1.0-threshold);
	else hardness = 999999999999999999999.0;
	//set up hardness to exactly fill gap between threshold and 0db
	//if threshold is literally 1 then hardness is infinite, so we make it very big
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		inputSample *= indrive;

		//calibrated to match gain through convolution and -0.3 correction
		if (sqdrive > 0.0){
			b[23] = b[22]; b[22] = b[21]; b[21] = b[20]; b[20] = b[19]; b[19] = b[18]; b[18] = b[17]; b[17] = b[16]; b[16] = b[15]; 
			b[15] = b[14]; b[14] = b[13]; b[13] = b[12]; b[12] = b[11]; b[11] = b[10]; b[10] = b[9]; b[9] = b[8]; b[8] = b[7]; 
			b[7] = b[6]; b[6] = b[5]; b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1]; b[1] = b[0]; b[0] = inputSample * sqdrive;
			inputSample += (b[1] * (0.38856694371895023  + (0.14001177830115491*fabs(b[1]))));
			inputSample -= (b[2] * (0.17469488984546111  + (0.05204541941091459*fabs(b[2]))));
			inputSample += (b[3] * (0.11643521461774288  - (0.01193121216518472*fabs(b[3]))));
			inputSample -= (b[4] * (0.08874416268268183  - (0.05867502375036486*fabs(b[4]))));
			inputSample += (b[5] * (0.07222999223073785  - (0.08519974113692971*fabs(b[5]))));
			inputSample -= (b[6] * (0.06103207678880003  - (0.09230674983449150*fabs(b[6]))));
			inputSample += (b[7] * (0.05277389277465404  - (0.08487342372497046*fabs(b[7]))));
			inputSample -= (b[8] * (0.04631144388636078  - (0.06976851898821038*fabs(b[8]))));
			inputSample += (b[9] * (0.04102721072495113  - (0.05337974329110802*fabs(b[9]))));
			inputSample -= (b[10] * (0.03656047655964371  - (0.03990914278458497*fabs(b[10]))));
			inputSample += (b[11] * (0.03268677450573373  - (0.03090433934018759*fabs(b[11]))));
			inputSample -= (b[12] * (0.02926012259262895  - (0.02585223214266682*fabs(b[12]))));
			inputSample += (b[13] * (0.02618257163789973  - (0.02326667039588473*fabs(b[13]))));
			inputSample -= (b[14] * (0.02338568277879992  - (0.02167067760829789*fabs(b[14]))));
			inputSample += (b[15] * (0.02082142324645262  - (0.02013392273267951*fabs(b[15]))));
			inputSample -= (b[16] * (0.01845525966656259  - (0.01833038930966512*fabs(b[16]))));
			inputSample += (b[17] * (0.01626113504980445  - (0.01631893218593511*fabs(b[17]))));
			inputSample -= (b[18] * (0.01422084088669267  - (0.01427828125219885*fabs(b[18]))));
			inputSample += (b[19] * (0.01231993595709338  - (0.01233991521342998*fabs(b[19]))));
			inputSample -= (b[20] * (0.01054774630451013  - (0.01054774630542346*fabs(b[20]))));
			inputSample += (b[21] * (0.00889548162355088  - (0.00889548162263755*fabs(b[21]))));
			inputSample -= (b[22] * (0.00735749099304526  - (0.00735749099395860*fabs(b[22]))));
			inputSample += (b[23] * (0.00592812350468000  - (0.00592812350376666*fabs(b[23]))));
		} //the Character plugins as individual processors did this. BussColors applies an averaging factor to produce
		// more of a consistent variation between soft and loud convolutions. For years I thought this code was a
		//mistake and did nothing, but in fact what it's doing is producing slightly different curves for every single
		//convolution kernel location: this will be true of the Character individual plugins as well.
		
		if (fabs(inputSample) > threshold)
		{
			bridgerectifier = (fabs(inputSample)-threshold)*hardness;
			//skip flat area if any, scale to distortion limit
			if (bridgerectifier > breakup) bridgerectifier = breakup;
			//max value for sine function, 'breakup' modeling for trashed console tone
			//more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
			bridgerectifier = sin(bridgerectifier)/hardness;
			//do the sine factor, scale back to proper amount
			if (inputSample > 0) inputSample = bridgerectifier+threshold;
			else inputSample = -(bridgerectifier+threshold);
		} //otherwise we leave it untouched by the overdrive stuff
		//this is the notorious New Channel Density algorithm. It's much less popular than the original Density,
		//because it introduces a point where the saturation 'curve' changes from straight to curved.
		//People don't like these discontinuities, but you can use them for effect or to grit up the sound.

		randy = ((double(fpd)/UINT32_MAX)*0.022);
		bridgerectifier = ((inputSample*(1-randy))+(lastSample*randy)) * outlevel;
		lastSample = inputSample;
		inputSample = bridgerectifier; //applies a tiny 'fuzz' to highs: from original Crystal.
		//This is akin to the old Chrome Oxide plugin, applying a fuzz to only the slews. The noise only appears
		//when current and old samples are different from each other, otherwise you can't tell it's there.
		//This is not only during silence but the tops of low frequency waves: it scales down to affect lows more gently.
		
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
	for(int count = 0; count < 34; count++) {b[count] = 0;}
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
