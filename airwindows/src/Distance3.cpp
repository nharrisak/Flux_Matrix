#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Distance3"
#define AIRWINDOWS_DESCRIPTION "Combines the best parts of Distance and Discontinuity."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','w' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
const int dscBuf = 90;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Distance", .min = 0, .max = 10000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Top dB", .min = 7000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		
		double lastclampAL;
		double clampAL;
		double changeAL;
		double prevresultAL;
		double lastAL;
		
		double lastclampBL;
		double clampBL;
		double changeBL;
		double prevresultBL;
		double lastBL;
		
		double lastclampCL;
		double clampCL;
		double changeCL;
		double prevresultCL;
		double lastCL;
		
		double dBaL[dscBuf+5];
		double dBaPosL;
		int dBaXL;
		
		double dBbL[dscBuf+5];
		double dBbPosL;
		int dBbXL;
		
		double dBcL[dscBuf+5];
		double dBcPosL;
		int dBcXL;
				
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
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();

	double softslew = (GetParameter( kParam_One )*10.0)+0.5;
	softslew *= overallscale;
	double outslew = softslew * (1.0-(GetParameter( kParam_One )*0.0333));
	double refdB = GetParameter( kParam_Two );
	double topdB = 0.000000075 * pow(10.0,refdB/20.0) * overallscale;
	double wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		double inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpd * 1.18e-17;
		double drySampleL = inputSampleL;
		
		inputSampleL *= softslew;
		lastclampAL = clampAL; clampAL = inputSampleL - lastAL;
		double postfilter = changeAL = fabs(clampAL - lastclampAL);
		postfilter += (softslew / 2.0);
		inputSampleL /= outslew;
		inputSampleL += (prevresultAL * postfilter);
		inputSampleL /= (postfilter + 1.0);
		prevresultAL = inputSampleL;
		//do an IIR like thing to further squish superdistant stuff
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222) inputSampleL = -0.222; if (inputSampleL > 0.222) inputSampleL = 0.222;
		//Air Discontinuity A begin
		dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
		int dBdly = floor(dBaPosL*dscBuf);
		double dBi = (dBaPosL*dscBuf)-dBdly;
		inputSampleL = dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleL += dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		//Air Discontinuity A end
		inputSampleL /= topdB;
		
		inputSampleL *= softslew;
		lastclampBL = clampBL; clampBL = inputSampleL - lastBL;
		postfilter = changeBL = fabs(clampBL - lastclampBL);
		postfilter += (softslew / 2.0);
		lastBL = inputSampleL;
		inputSampleL /= outslew;
		inputSampleL += (prevresultBL * postfilter);
		inputSampleL /= (postfilter + 1.0);
		prevresultBL = inputSampleL;
		//do an IIR like thing to further squish superdistant stuff
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222) inputSampleL = -0.222; if (inputSampleL > 0.222) inputSampleL = 0.222;
		//Air Discontinuity B begin
		dBbL[dBbXL] = inputSampleL;  dBbPosL *= 0.5; dBbPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
		dBdly = floor(dBbPosL*dscBuf); dBi = (dBbPosL*dscBuf)-dBdly;
		inputSampleL = dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleL += dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBbXL++; if (dBbXL < 0 || dBbXL >= dscBuf) dBbXL = 0;
		//Air Discontinuity B end
		inputSampleL /= topdB;
		
		inputSampleL *= softslew;
		lastclampCL = clampCL; clampCL = inputSampleL - lastCL;
		postfilter = changeCL = fabs(clampCL - lastclampCL);
		postfilter += (softslew / 2.0);
		lastCL = inputSampleL;
		inputSampleL /= softslew; //don't boost the final time!
		inputSampleL += (prevresultCL * postfilter);
		inputSampleL /= (postfilter + 1.0);
		prevresultCL = inputSampleL;
		//do an IIR like thing to further squish superdistant stuff
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222) inputSampleL = -0.222; if (inputSampleL > 0.222) inputSampleL = 0.222;		
		//Air Discontinuity C begin
		dBcL[dBcXL] = inputSampleL;  dBcPosL *= 0.5; dBcPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
		dBdly = floor(dBcPosL*dscBuf); dBi = (dBcPosL*dscBuf)-dBdly;
		inputSampleL = dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
		dBdly++; inputSampleL += dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBcXL++; if (dBcXL < 0 || dBcXL >= dscBuf) dBcXL = 0;
		//Air Discontinuity C end
		inputSampleL /= topdB;
		
		if (wet < 1.0) inputSampleL = (drySampleL * (1.0-wet))+(inputSampleL*wet);
		
		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSampleL += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	prevresultAL = lastclampAL = clampAL = changeAL = lastAL = 0.0;
	prevresultBL = lastclampBL = clampBL = changeBL = lastBL = 0.0;
	prevresultCL = lastclampCL = clampCL = changeCL = lastCL = 0.0;

	for(int count = 0; count < dscBuf+2; count++) {
		dBaL[count] = 0.0;
		dBbL[count] = 0.0;
		dBcL[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBbPosL = 0.0;
	dBcPosL = 0.0;
	dBaXL = 1;
	dBbXL = 1;
	dBcXL = 1;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
