#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Neverland"
#define AIRWINDOWS_DESCRIPTION "A re-release of my old Neve-style color adder, exacly as it was."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','N','e','v' )
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
{ .name = "Hardness", .min = 0, .max = 1000, .def = 740, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
	Float64 sqdrive = GetParameter( kParam_Two )*0.788;
	if (sqdrive > 1.0) sqdrive *= sqdrive;
	sqdrive = sqrt(sqdrive);
	Float64 indrive = GetParameter( kParam_Three );
	if (indrive > 1.0) indrive *= indrive;
	indrive /= (1.0+(0.2*sqdrive));
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
		
		if (sqdrive > 0.0){
			dram->b[33] = dram->b[32]; dram->b[32] = dram->b[31]; 
			dram->b[31] = dram->b[30]; dram->b[30] = dram->b[29]; dram->b[29] = dram->b[28]; dram->b[28] = dram->b[27]; dram->b[27] = dram->b[26]; dram->b[26] = dram->b[25]; dram->b[25] = dram->b[24]; dram->b[24] = dram->b[23]; 
			dram->b[23] = dram->b[22]; dram->b[22] = dram->b[21]; dram->b[21] = dram->b[20]; dram->b[20] = dram->b[19]; dram->b[19] = dram->b[18]; dram->b[18] = dram->b[17]; dram->b[17] = dram->b[16]; dram->b[16] = dram->b[15]; 
			dram->b[15] = dram->b[14]; dram->b[14] = dram->b[13]; dram->b[13] = dram->b[12]; dram->b[12] = dram->b[11]; dram->b[11] = dram->b[10]; dram->b[10] = dram->b[9]; dram->b[9] = dram->b[8]; dram->b[8] = dram->b[7]; 
			dram->b[7] = dram->b[6]; dram->b[6] = dram->b[5]; dram->b[5] = dram->b[4]; dram->b[4] = dram->b[3]; dram->b[3] = dram->b[2]; dram->b[2] = dram->b[1]; dram->b[1] = dram->b[0]; dram->b[0] = inputSample * sqdrive;
			inputSample += (dram->b[1] * (0.20641602693167951  - (0.00078952185394898*fabs(dram->b[1]))));
			inputSample -= (dram->b[2] * (0.07601816702459827  + (0.00022786334179951*fabs(dram->b[2]))));
			inputSample += (dram->b[3] * (0.03929765560019285  - (0.00054517993246352*fabs(dram->b[3]))));
			inputSample += (dram->b[4] * (0.00298333157711103  - (0.00033083756545638*fabs(dram->b[4]))));
			inputSample -= (dram->b[5] * (0.00724006282304610  + (0.00045483683460812*fabs(dram->b[5]))));
			inputSample += (dram->b[6] * (0.03073108963506036  - (0.00038190060537423*fabs(dram->b[6]))));
			inputSample -= (dram->b[7] * (0.02332434692533051  + (0.00040347288688932*fabs(dram->b[7]))));
			inputSample += (dram->b[8] * (0.03792606869061214  - (0.00039673687335892*fabs(dram->b[8]))));
			inputSample -= (dram->b[9] * (0.02437059376675688  + (0.00037221210539535*fabs(dram->b[9]))));
			inputSample += (dram->b[10] * (0.03416764311979521  - (0.00040314850796953*fabs(dram->b[10]))));
			inputSample -= (dram->b[11] * (0.01761669868102127  + (0.00035989484330131*fabs(dram->b[11]))));
			inputSample += (dram->b[12] * (0.02538237753523052  - (0.00040149119125394*fabs(dram->b[12]))));
			inputSample -= (dram->b[13] * (0.00770737340728377  + (0.00035462118723555*fabs(dram->b[13]))));
			inputSample += (dram->b[14] * (0.01580706228482803  - (0.00037563141307594*fabs(dram->b[14]))));
			inputSample += (dram->b[15] * (0.00055119240005586  - (0.00035409299268971*fabs(dram->b[15]))));
			inputSample += (dram->b[16] * (0.00818552143438768  - (0.00036507661042180*fabs(dram->b[16]))));
			inputSample += (dram->b[17] * (0.00661842703548304  - (0.00034550528559056*fabs(dram->b[17]))));
			inputSample += (dram->b[18] * (0.00362447476272098  - (0.00035553012761240*fabs(dram->b[18]))));
			inputSample += (dram->b[19] * (0.00957098027225745  - (0.00034091691045338*fabs(dram->b[19]))));
			inputSample += (dram->b[20] * (0.00193621774016660  - (0.00034554529131668*fabs(dram->b[20]))));
			inputSample += (dram->b[21] * (0.01005433027357935  - (0.00033878223153845*fabs(dram->b[21]))));
			inputSample += (dram->b[22] * (0.00221712428802004  - (0.00033481410137711*fabs(dram->b[22]))));
			inputSample += (dram->b[23] * (0.00911255639207995  - (0.00033263425232666*fabs(dram->b[23]))));
			inputSample += (dram->b[24] * (0.00339667169034909  - (0.00032634428038430*fabs(dram->b[24]))));
			inputSample += (dram->b[25] * (0.00774096948249924  - (0.00032599868802996*fabs(dram->b[25]))));
			inputSample += (dram->b[26] * (0.00463907626773794  - (0.00032131993173361*fabs(dram->b[26]))));
			inputSample += (dram->b[27] * (0.00658222997260378  - (0.00032014977430211*fabs(dram->b[27]))));
			inputSample += (dram->b[28] * (0.00550347079924993  - (0.00031557153256653*fabs(dram->b[28]))));
			inputSample += (dram->b[29] * (0.00588754981375325  - (0.00032041307242303*fabs(dram->b[29]))));
			inputSample += (dram->b[30] * (0.00590293898419892  - (0.00030457857428714*fabs(dram->b[30]))));
			inputSample += (dram->b[31] * (0.00558952010441800  - (0.00030448053548086*fabs(dram->b[31]))));
			inputSample += (dram->b[32] * (0.00598183557634295  - (0.00030715064323181*fabs(dram->b[32]))));
			inputSample += (dram->b[33] * (0.00555223929714115  - (0.00030319367948553*fabs(dram->b[33]))));
			//we apply the first samples of the Neve impulse- dynamically adjusted.
		}
		randy = ((double(fpd)/UINT32_MAX)*0.034);
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
