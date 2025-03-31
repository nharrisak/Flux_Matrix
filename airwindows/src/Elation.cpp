#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Elation"
#define AIRWINDOWS_DESCRIPTION "A re-release of another old Character plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','E','l','a' )
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
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		Float64 compA;
		Float64 compB;
		Float64 compC;
		Float64 compD;
		Float64 previous;
		Float64 previousB;
		Float64 lastSample;
		bool flip;
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

	Float64 wet = GetParameter( kParam_One );
	Float64 sqdrive = GetParameter( kParam_Two );
	if (sqdrive > 1.0) sqdrive *= sqdrive;
	sqdrive = sqrt(sqdrive);
	Float64 indrive = GetParameter( kParam_Three );
	Float64 compthreshold = (6.0 - indrive)/6.0;
	Float64 recoveryspd = indrive / 32.0;
	if (indrive > 1.0) indrive *= indrive;
	indrive *= (1.0+(0.226*sqdrive));
	//no gain loss of convolution for APIcolypse
	//calibrate this to match noise level with character at 1.0
	//you get for instance 0.819 and 1.0-0.819 is 0.181
	Float64 randy;
	Float64 outlevel = GetParameter( kParam_Four );
	Float64 threshSample;
	Float64 abSample;
	Float64 outputSample;
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		double drySample = inputSample;

		inputSample *= indrive;
		
		abSample = fabs(inputSample);
		if (abSample > previous) threshSample = previous;
		else threshSample = abSample;
		//calibrated to match gain through convolution and -0.3 correction
		//quick lil compression
		if (threshSample > 2.0)
		{
			compA = (compA + (1.0 / threshSample)) / 2.0;
			compB = (compB + (1.0 / threshSample)) / 2.0;
		}
		
		if (flip)
		{
			inputSample *= compA;
			threshSample *= compA;
			if (threshSample > compthreshold)
			{compA = (compA + compA + (compthreshold / threshSample)) / 3.0;}
			else
			{if (compA < 1.0) compA += pow((1.0-compA)*recoveryspd,7);}
		}
		else
		{
			inputSample *= compB;
			threshSample *= compB;
			if (threshSample > compthreshold)
			{compB = (compB + compB + (compthreshold / threshSample)) / 3.0;}
			else
			{if (compB < 1.0) compB += pow((1.0-compB)*recoveryspd,7);}			
		}
		previous = abSample;
		//now the convolution
		if (sqdrive > 0.0){
			b[33] = b[32]; b[32] = b[31]; 
			b[31] = b[30]; b[30] = b[29]; b[29] = b[28]; b[28] = b[27]; b[27] = b[26]; b[26] = b[25]; b[25] = b[24]; b[24] = b[23]; 
			b[23] = b[22]; b[22] = b[21]; b[21] = b[20]; b[20] = b[19]; b[19] = b[18]; b[18] = b[17]; b[17] = b[16]; b[16] = b[15]; 
			b[15] = b[14]; b[14] = b[13]; b[13] = b[12]; b[12] = b[11]; b[11] = b[10]; b[10] = b[9]; b[9] = b[8]; b[8] = b[7]; 
			b[7] = b[6]; b[6] = b[5]; b[5] = b[4]; b[4] = b[3]; b[3] = b[2]; b[2] = b[1]; b[1] = b[0]; b[0] = inputSample * sqdrive;
			//inputSample -= ((b[1] * (0.25867935358656502  - (0.00045755657070112*fabs(b[1]))))*threshold);
			inputSample -= (b[1] * (0.25867935358656502  - (0.00045755657070112*fabs(b[1]))));
			inputSample += (b[2] * (0.11509367290253694  - (0.00017494270657228*fabs(b[2]))));
			inputSample -= (b[3] * (0.06709853575891785  - (0.00058913102597723*fabs(b[3]))));
			inputSample += (b[4] * (0.01871006356851681  - (0.00003387358004645*fabs(b[4]))));
			inputSample -= (b[5] * (0.00794797957360465  - (0.00044224784691203*fabs(b[5]))));
			inputSample -= (b[6] * (0.01956921817394220  - (0.00006718936750076*fabs(b[6]))));
			inputSample += (b[7] * (0.01682120257195205  + (0.00032857446292230*fabs(b[7]))));
			inputSample -= (b[8] * (0.03401069039824205  - (0.00013634182872897*fabs(b[8]))));
			inputSample += (b[9] * (0.02369950268232634  + (0.00023112685751657*fabs(b[9]))));
			inputSample -= (b[10] * (0.03477071178117132  - (0.00018029792231600*fabs(b[10]))));
			inputSample += (b[11] * (0.02024369717958201  + (0.00017337813374202*fabs(b[11]))));
			inputSample -= (b[12] * (0.02819087729102172  - (0.00021438538665420*fabs(b[12]))));
			inputSample += (b[13] * (0.01147946743141303  + (0.00014424066034649*fabs(b[13]))));
			inputSample -= (b[14] * (0.01894777011468867  - (0.00021549146262408*fabs(b[14]))));
			inputSample += (b[15] * (0.00301370330346873  + (0.00013527460148394*fabs(b[15]))));
			inputSample -= (b[16] * (0.01067147835815486  - (0.00020960689910868*fabs(b[16]))));
			inputSample -= (b[17] * (0.00402715397506384  - (0.00014421582712470*fabs(b[17]))));
			inputSample -= (b[18] * (0.00502221703392005  - (0.00019805767015024*fabs(b[18]))));
			inputSample -= (b[19] * (0.00808788533308497  - (0.00016095444141931*fabs(b[19]))));
			inputSample -= (b[20] * (0.00232696588842683  - (0.00018384470981829*fabs(b[20]))));
			inputSample -= (b[21] * (0.00943950821324531  - (0.00017098987347593*fabs(b[21]))));
			inputSample -= (b[22] * (0.00193709517200834  - (0.00018151995939591*fabs(b[22]))));
			inputSample -= (b[23] * (0.00899713952612659  - (0.00017385835059948*fabs(b[23]))));
			inputSample -= (b[24] * (0.00280584331659089  - (0.00017742164162470*fabs(b[24]))));
			inputSample -= (b[25] * (0.00780381001954970  - (0.00018002500755708*fabs(b[25]))));
			inputSample -= (b[26] * (0.00400370310490333  - (0.00017471691087957*fabs(b[26]))));
			inputSample -= (b[27] * (0.00661527728186928  - (0.00018137323370347*fabs(b[27]))));
			inputSample -= (b[28] * (0.00496545526864518  - (0.00017681872601767*fabs(b[28]))));
			inputSample -= (b[29] * (0.00580728820997532  - (0.00018186220389790*fabs(b[29]))));
			inputSample -= (b[30] * (0.00549309984725666  - (0.00017722985399075*fabs(b[30]))));
			inputSample -= (b[31] * (0.00542194777529239  - (0.00018486900185338*fabs(b[31]))));
			inputSample -= (b[32] * (0.00565992080998939  - (0.00018005824393118*fabs(b[32]))));
			inputSample -= (b[33] * (0.00532121562846656  - (0.00018643189636216*fabs(b[33]))));}
		//we apply the first samples of the impulse- dynamically adjusted.
		
		abSample = fabs(inputSample);
		if (abSample > previousB) threshSample = previousB;
		else threshSample = abSample;
		//calibrated to match gain through convolution and -0.3 correction
		//quick lil compression
		if (threshSample > 2.0)
		{
			compC = (compC + (1.0 / threshSample)) / 2.0;
			compD = (compD + (1.0 / threshSample)) / 2.0;
		}
		
		if (flip)
		{
			inputSample *= compC;
			threshSample *= compC;
			if (threshSample > compthreshold)
			{compC = (compC + compC + (compthreshold / threshSample)) / 3.0;}
			else
			{if (compC < 1.0) compC += pow((1.0-compC)*recoveryspd,7);}
		}
		else
		{
			inputSample *= compD;
			threshSample *= compD;
			if (threshSample > compthreshold)
			{compD = (compD + compD + (compthreshold / threshSample)) / 3.0;}
			else
			{if (compD < 1.0) compD += pow((1.0-compD)*recoveryspd,7);}			
		}
		previousB = abSample;
		//another dose of compression, please
		
		flip = !flip;
		
		randy = ((double(fpd)/UINT32_MAX)*0.054);
		
		outputSample = ((((inputSample*(1-randy))+(lastSample*randy))*wet)+(drySample*(1.0-wet))) * outlevel;
		lastSample = inputSample;

		inputSample = outputSample;
		
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
	compA = 1.0; compB = 1.0; flip = false; previous = 0.0;
	compC = 1.0; compD = 1.0; previousB = 0.0;
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
