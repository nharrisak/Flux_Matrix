#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Calibre"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','a','l' )
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
	struct _dram* dram;
 
		Float64 lastSample;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
		Float64 b[35];
};
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
	indrive *= (1.0+(0.1935*sqdrive));
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

		
		inputSample = *sourceP * indrive;
		//calibrated to match gain through convolution and -0.3 correction
		if (sqdrive > 0.0){
			dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample * sqdrive;
			inputSample -= (dram->b[1] * (0.23505923670562212  - (0.00028312859289245*fabs(dram->b[1]))));
			inputSample += (dram->b[2] * (0.08188436704577637  - (0.00008817721351341*fabs(dram->b[2]))));
			inputSample -= (dram->b[3] * (0.05075798481700617  - (0.00018817166632483*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.00455811821873093  + (0.00001922902995296*fabs(dram->b[4]))));
			inputSample -= (dram->b[5] * (0.00027610521433660  - (0.00013252525469291*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.03529246280346626  - (0.00002772989223299*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.01784111585586136  + (0.00010230276997291*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.04394950700298298  - (0.00005910607126944*fabs(dram->b[8]))));
			inputSample += (dram->b[9] * (0.01990770780547606  + (0.00007640328340556*fabs(dram->b[9]))));
			inputSample -= (dram->b[10] * (0.04073629569741782  - (0.00007712327117090*fabs(dram->b[10]))));
			inputSample += (dram->b[11] * (0.01349648572795252  + (0.00005959130575917*fabs(dram->b[11]))));
			inputSample -= (dram->b[12] * (0.03191590248003717  - (0.00008418000575151*fabs(dram->b[12]))));
			inputSample += (dram->b[13] * (0.00348795527924766  + (0.00005489156318238*fabs(dram->b[13]))));
			inputSample -= (dram->b[14] * (0.02198496281481767  - (0.00008471601187581*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.00504771152505089  - (0.00005525060587917*fabs(dram->b[15]))));
			inputSample -= (dram->b[16] * (0.01391075698598491  - (0.00007929630732607*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.01142762504081717  - (0.00005967036737742*fabs(dram->b[17]))));
			inputSample -= (dram->b[18] * (0.00893541815021255  - (0.00007535697758141*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.01459704973464936  - (0.00005969199602841*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00694755135226282  - (0.00006930127097865*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.01516695630808575  - (0.00006365800069826*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.00705917318113651  - (0.00006497209096539*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.01420501209177591  - (0.00006555654576113*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.00815905656808701  - (0.00006105622534761*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.01274326525552961  - (0.00006542652857017*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.00937146927845488  - (0.00006051267868722*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.01146573981165209  - (0.00006381511607749*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.01021294359409007  - (0.00005930397856398*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.01065217095323532  - (0.00006371505438319*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.01058751196699751  - (0.00006042857480233*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.01026557827762401  - (0.00006007776163871*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.01060929183604604  - (0.00006114703012726*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.01014533525058528  - (0.00005963567932887*fabs(dram->b[33]))));}
		
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
		randy = ((double(fpd)/UINT32_MAX)*0.042);
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
	for(int count = 0; count < 34; count++) {dram->b[count] = 0;}
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
