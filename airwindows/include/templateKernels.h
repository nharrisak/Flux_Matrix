
void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	int32_t numChannels = specifications[0];

	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_airwindowsAlgorithm) + ( numChannels - 1 ) * sizeof(_airwindowsAlgorithm::_kernel);
	req.dram = sizeof(_dram) * numChannels;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	int32_t numChannels = specifications[0];
	_dram* dram = (_dram*)ptrs.dram;

	_airwindowsAlgorithm* alg = new (ptrs.sram) _airwindowsAlgorithm();
	alg->numChannels = numChannels;
	for ( int32_t i=0; i<numChannels; ++i )
	{
		alg->kernels[i].owner = alg;
		alg->kernels[i].dram = dram + i;
	}
	alg->reset();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	return alg;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_airwindowsAlgorithm* pThis = (_airwindowsAlgorithm*)self;
	int numFrames = numFramesBy4 * 4;
	const float* in0 = busFrames + ( pThis->v[kParamInput1] - 1 ) * numFrames;
	float* out0 = busFrames + ( pThis->v[kParamOutput1] - 1 ) * numFrames;
	bool replace0 = pThis->v[kParamOutput1mode];
	
	int numChannels = pThis->numChannels;
	numChannels = std::min( numChannels, 28 + 1 - pThis->v[kParamInput1] );
	numChannels = std::min( numChannels, 28 + 1 - pThis->v[kParamOutput1] );
	
	for ( int i=0; i<numChannels; ++i )
	{
		float* temp0 = NT_globals.workBuffer;
		
		pThis->kernels[i].render( in0, replace0 ? out0 : temp0, numFrames );
	
		if ( !replace0 )
		{
			for ( int i=0; i<numFrames; ++i )
			{
				out0[i] += temp0[i];
			}
		}
		
		in0 += numFrames;
		out0 += numFrames;
	}
}

int _airwindowsAlgorithm::reset(void)
{
	for ( uint32_t i=0; i<numChannels; ++i )
	{
		kernels[i].reset();
	}
	return noErr;
}
