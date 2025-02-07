
#include <math.h>
#include <new>
#include <distingnt/api.h>

struct _gainAlgorithm : public _NT_algorithm
{
	_gainAlgorithm() {}
	~_gainAlgorithm() {}
	
	float gain;
};

static const _NT_parameter	parameters[1] = {
	{ .unit = 0, .scaling = 0, .min = 0, .max = 100, .def = 50, .name = "Gain", .enumStrings = NULL },
};

void	calculateRequirements( _NT_algorithmRequirements& req )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_gainAlgorithm);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req )
{
	_gainAlgorithm* alg = new (ptrs.sram) _gainAlgorithm();
	alg->parameters = parameters;
	return alg;
}

void	parameterChanged( _NT_algorithm* self, int p )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	pThis->gain = pThis->v[0] / 100.0f;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_gainAlgorithm* pThis = (_gainAlgorithm*)self;
	float gain = pThis->gain;
	int numFrames = numFramesBy4 * 4;
	for ( int i=0; i<numFrames; ++i )
		busFrames[i] *= gain;
}

static const _NT_factory factory = 
{
	.guid = NT_MULTICHAR( 'E', 'x', 'g', 'a' ),
	.name = "Gain",
	.description = "Applies gain",
	.calculateRequirements = calculateRequirements,
	.construct = construct,
	.parameterChanged = parameterChanged,
	.step = step,
};

uintptr_t pluginEntry( _NT_selector selector, uint32_t data )
{
	switch ( selector )
	{
	case kNT_selector_version:
		return kNT_apiVersionCurrent;
	case kNT_selector_numFactories:
		return 1;
	case kNT_selector_factoryInfo:
		return (uintptr_t)( ( data == 0 ) ? &factory : NULL );
	}
	return 0;
}
