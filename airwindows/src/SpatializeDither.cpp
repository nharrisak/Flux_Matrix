#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SpatializeDither"
#define AIRWINDOWS_DESCRIPTION "A high-performance clarity and accuracy dither."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','p','a' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	//Add your parameters here...
	kNumberOfParameters=2
};
static const int kCD = 0;
static const int kHD = 1;
static const int kDefaultValue_ParamOne = kHD;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static char const * const enumStrings0[] = { "CD 16 bit", "HD 24 bit", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Quantizer", .min = 0, .max = 1, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float64 contingentErr;
		bool flip;
		uint32_t fpd;
	
	struct _dram {
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
	
	double inputSample;
	Float64 contingentRnd;
	Float64 absSample;
	Float64 contingent;
	Float64 randyConstant = 1.61803398874989484820458683436563811772030917980576;
	Float64 omegaConstant = 0.56714329040978387299996866221035554975381578718651;
	Float64 expConstant = 0.06598803584531253707679018759684642493857704825279;
	
	bool highres = false;
	if (GetParameter( kParam_One ) == 1) highres = true;
	Float32 scaleFactor;
	if (highres) scaleFactor = 8388608.0;
	else scaleFactor = 32768.0;
	Float32 derez = GetParameter( kParam_Two );
	if (derez > 0.0) scaleFactor *= pow(1.0-derez,6);
	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	Float32 outScale = scaleFactor;
	if (outScale < 8.0) outScale = 8.0;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
		inputSample *= scaleFactor;
		//0-1 is now one bit, now we dither
		
		if (inputSample > 0) inputSample += 0.383;
		if (inputSample < 0) inputSample -= 0.383;
		//adjusting to permit more information drug outta the noisefloor
		contingentRnd = (double(fpd)/UINT32_MAX);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		contingentRnd += ((double(fpd)/UINT32_MAX)-1.0); 
		contingentRnd *= randyConstant; //produce TPDF dist, scale
        contingentRnd -= contingentErr*omegaConstant; //include err
		absSample = fabs(inputSample);
		contingentErr = absSample - floor(absSample); //get next err
		contingent = contingentErr * 2.0; //scale of quantization levels
		if (contingent > 1.0) contingent = ((-contingent+2.0)*omegaConstant) + expConstant;
		else contingent = (contingent * omegaConstant) + expConstant;
		//zero is next to a quantization level, one is exactly between them
		if (flip) contingentRnd = (contingentRnd * (1.0-contingent)) + contingent + 0.5;
		else contingentRnd = (contingentRnd * (1.0-contingent)) - contingent + 0.5;
		flip = !flip;
		inputSample += (contingentRnd * contingent);
		//Contingent Dither
		inputSample = floor(inputSample);
		//note: this does not dither for values exactly the same as 16 bit values-
		//which forces the dither to gate at 0.0. It goes to digital black,
		//and does a teeny parallel-compression thing when almost at digital black.
		
		inputSample /= outScale;

		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		//pseudorandom number updater
		
		*destP = inputSample;
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	contingentErr = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
