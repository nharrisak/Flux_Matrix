#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Cider"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','i','d' )
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
{ .name = "Personality", .min = 0, .max = 3000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	indrive *= (1.0-(0.216*sqdrive));
	//correct for gain loss of convolution
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
			b[33] = b[32]; b[32] = b[31]; 
			b[31] = b[30]; b[30] = b[29]; b[29] = b[28]; b[28] = b[27]; b[27] = b[26]; b[26] = b[25]; b[25] = b[24]; b[24] = b[23]; 
			b[23] = b[22]; b[22] = b[21]; b[21] = b[20]; b[20] = b[19]; b[19] = b[18]; b[18] = b[17]; b[17] = b[16]; b[16] = b[15]; 
			b[15] = b[14]; b[14] = b[13]; b[13] = b[12]; b[12] = b[11]; b[11] = b[10]; b[10] = b[9]; b[9] = b[8]; b[8] = b[7]; 
			b[7] = b[6]; b[6] = b[5]; b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1]; b[1] = b[0]; b[0] = inputSample * sqdrive;
			inputSample += (b[1] * (0.61283288942201319  + (0.00024011410669522*fabs(b[1]))));
			inputSample -= (b[2] * (0.24036380659761222  - (0.00020789518206241*fabs(b[2]))));
			inputSample += (b[3] * (0.09104669761717916  + (0.00012829642741548*fabs(b[3]))));
			inputSample -= (b[4] * (0.02378290768554025  - (0.00017673646470440*fabs(b[4]))));
			inputSample -= (b[5] * (0.02832818490275965  - (0.00013536187747384*fabs(b[5]))));
			inputSample += (b[6] * (0.03268797679215937  + (0.00015035126653359*fabs(b[6]))));
			inputSample -= (b[7] * (0.04024464202655586  - (0.00015034923056735*fabs(b[7]))));
			inputSample += (b[8] * (0.01864890074318696  + (0.00014513281680642*fabs(b[8]))));
			inputSample -= (b[9] * (0.01632731954100322  - (0.00015509089075614*fabs(b[9]))));
			inputSample -= (b[10] * (0.00318907090555589  - (0.00014784812076550*fabs(b[10]))));
			inputSample -= (b[11] * (0.00208573465221869  - (0.00015350520779465*fabs(b[11]))));
			inputSample -= (b[12] * (0.00907033901519614  - (0.00015442964157250*fabs(b[12]))));
			inputSample -= (b[13] * (0.00199458794148013  - (0.00015595640046297*fabs(b[13]))));
			inputSample -= (b[14] * (0.00705979153201755  - (0.00015730069418051*fabs(b[14]))));
			inputSample -= (b[15] * (0.00429488975412722  - (0.00015743697943505*fabs(b[15]))));
			inputSample -= (b[16] * (0.00497724878704936  - (0.00016014760011861*fabs(b[16]))));
			inputSample -= (b[17] * (0.00506059305562353  - (0.00016194824072466*fabs(b[17]))));
			inputSample -= (b[18] * (0.00483432223285621  - (0.00016329050124225*fabs(b[18]))));
			inputSample -= (b[19] * (0.00495100420886005  - (0.00016297509798749*fabs(b[19]))));
			inputSample -= (b[20] * (0.00489319520555115  - (0.00016472839684661*fabs(b[20]))));
			inputSample -= (b[21] * (0.00489177657970308  - (0.00016791875866630*fabs(b[21]))));
			inputSample -= (b[22] * (0.00487900894707044  - (0.00016755993898534*fabs(b[22]))));
			inputSample -= (b[23] * (0.00486234009335561  - (0.00016968157345446*fabs(b[23]))));
			inputSample -= (b[24] * (0.00485737490288736  - (0.00017180713324431*fabs(b[24]))));
			inputSample -= (b[25] * (0.00484106070563455  - (0.00017251073661092*fabs(b[25]))));
			inputSample -= (b[26] * (0.00483219429408410  - (0.00017321683790891*fabs(b[26]))));
			inputSample -= (b[27] * (0.00482013597437550  - (0.00017392186866488*fabs(b[27]))));
			inputSample -= (b[28] * (0.00480949628051497  - (0.00017569098775602*fabs(b[28]))));
			inputSample -= (b[29] * (0.00479992055604049  - (0.00017746046369449*fabs(b[29]))));
			inputSample -= (b[30] * (0.00478750757986987  - (0.00017745630047554*fabs(b[30]))));
			inputSample -= (b[31] * (0.00477828651185740  - (0.00017958043287604*fabs(b[31]))));
			inputSample -= (b[32] * (0.00476906544384494  - (0.00018170456527653*fabs(b[32]))));
			inputSample -= (b[33] * (0.00475700712413634  - (0.00018099144598088*fabs(b[33]))));}
		//we apply the first samples of the Focusrite impulse- dynamically adjusted.
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
		}
		//otherwise we leave it untouched by the overdrive stuff
		randy = ((double(fpd)/UINT32_MAX)*0.057);
		inputSample = ((inputSample*(1-randy))+(lastSample*randy)) * outlevel;
		lastSample = inputSample;
		
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
