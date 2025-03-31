#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Precious"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','e' )
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
 
		Float64 lastSample;
		uint32_t fpd;
	
	struct _dram {
			Float64 b[35];
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
	Float64 threshold = GetParameter( kParam_One );
	Float64 hardness;
	Float64 breakup = (1.0-(threshold/2.0))*3.14159265358979;
	Float64 bridgerectifier;
	Float64 sqdrive = GetParameter( kParam_Two );
	if (sqdrive > 1.0) sqdrive *= sqdrive;
	sqdrive = sqrt(sqdrive);
	Float64 indrive = GetParameter( kParam_Three );
	if (indrive > 1.0) indrive *= indrive;
	indrive *= (1.0-(0.2095*sqdrive));
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
			dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample * sqdrive;
			inputSample += (dram->b[1] * (0.59188440274551890  - (0.00008361469668405*fabs(dram->b[1]))));
			inputSample -= (dram->b[2] * (0.24439750948076133  + (0.00002651678396848*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (0.14109876103205621  - (0.00000840487181372*fabs(dram->b[3]))));
			inputSample -= (dram->b[4] * (0.10053507128157971  + (0.00001768100964598*fabs(dram->b[4]))));
			inputSample += (dram->b[5] * (0.05859287880626238  - (0.00000361398065989*fabs(dram->b[5]))));
			inputSample -= (dram->b[6] * (0.04337406889823660  + (0.00000735941182117*fabs(dram->b[6]))));
			inputSample += (dram->b[7] * (0.01589900680531097  + (0.00000207347387987*fabs(dram->b[7]))));
			inputSample -= (dram->b[8] * (0.01087234854973281  + (0.00000732123412029*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.00845782429679176  - (0.00000133058605071*fabs(dram->b[9]))));
			inputSample += (dram->b[10] * (0.00662278586618295  - (0.00000424594730611*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.02000592193760155  + (0.00000632896879068*fabs(dram->b[11]))));
			inputSample += (dram->b[12] * (0.01321157777167565  - (0.00001421171592570*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.02249955362988238  + (0.00000163937127317*fabs(dram->b[13]))));
			inputSample += (dram->b[14] * (0.01196492077581504  - (0.00000535385220676*fabs(dram->b[14]))));
			inputSample -= (dram->b[15] * (0.01905917427000097  + (0.00000121672882030*fabs(dram->b[15]))));
			inputSample += (dram->b[16] * (0.00761909482108073  - (0.00000326242895115*fabs(dram->b[16]))));
			inputSample -= (dram->b[17] * (0.01362744780256239  + (0.00000359274216003*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.00200183122683721  - (0.00000089207452791*fabs(dram->b[18]))));
			inputSample -= (dram->b[19] * (0.00833042637239315  + (0.00000946767677294*fabs(dram->b[19]))));
			inputSample -= (dram->b[20] * (0.00258481175207224  - (0.00000087429351464*fabs(dram->b[20]))));
			inputSample -= (dram->b[21] * (0.00459744479712244  - (0.00000049519758701*fabs(dram->b[21]))));
			inputSample -= (dram->b[22] * (0.00534277030993820  + (0.00000397547847155*fabs(dram->b[22]))));
			inputSample -= (dram->b[23] * (0.00272332919605675  + (0.00000040077229097*fabs(dram->b[23]))));
			inputSample -= (dram->b[24] * (0.00637243782359372  - (0.00000139419072176*fabs(dram->b[24]))));
			inputSample -= (dram->b[25] * (0.00233001590327504  + (0.00000420129915747*fabs(dram->b[25]))));
			inputSample -= (dram->b[26] * (0.00623296727793041  + (0.00000019010664856*fabs(dram->b[26]))));
			inputSample -= (dram->b[27] * (0.00276177096376805  + (0.00000580301901385*fabs(dram->b[27]))));
			inputSample -= (dram->b[28] * (0.00559184754866264  + (0.00000080597287792*fabs(dram->b[28]))));
			inputSample -= (dram->b[29] * (0.00343180144395919  - (0.00000243701142085*fabs(dram->b[29]))));
			inputSample -= (dram->b[30] * (0.00493325428861701  + (0.00000300985740900*fabs(dram->b[30]))));
			inputSample -= (dram->b[31] * (0.00396140827680823  - (0.00000051459681789*fabs(dram->b[31]))));
			inputSample -= (dram->b[32] * (0.00448497879902493  + (0.00000744412841743*fabs(dram->b[32]))));
			inputSample -= (dram->b[33] * (0.00425146888772076  - (0.00000082346016542*fabs(dram->b[33]))));}
		//Precision 8
		
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
		randy = ((double(fpd)/UINT32_MAX)*0.017);
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
