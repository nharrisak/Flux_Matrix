
void	calculateRequirements( _NT_algorithmRequirements& req, const int32_t* specifications )
{
	req.numParameters = ARRAY_SIZE(parameters);
	req.sram = sizeof(_airwindowsAlgorithm);
	req.dram = 0;
	req.dtc = 0;
	req.itc = 0;
}

_NT_algorithm*	construct( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req, const int32_t* specifications )
{
	_airwindowsAlgorithm* alg = new (ptrs.sram) _airwindowsAlgorithm();
	alg->reset();
	alg->parameters = parameters;
	alg->parameterPages = &parameterPages;
	return alg;
}

void 	step( _NT_algorithm* self, float* busFrames, int numFramesBy4 )
{
	_airwindowsAlgorithm* pThis = (_airwindowsAlgorithm*)self;
	int numFrames = numFramesBy4 * 4;
	const float* in0 = busFrames + ( pThis->v[kParamInputL] - 1 ) * numFrames;
	const float* in1 = busFrames + ( pThis->v[kParamInputR] - 1 ) * numFrames;
	float* out0 = busFrames + ( pThis->v[kParamOutputL] - 1 ) * numFrames;
	float* out1 = busFrames + ( pThis->v[kParamOutputR] - 1 ) * numFrames;
	bool replace0 = pThis->v[kParamOutputLmode];
	bool replace1 = pThis->v[kParamOutputRmode];
	
	float* temp0 = NT_globals.workBuffer;
	float* temp1 = temp0 + numFrames;
	
	pThis->render( in0, in1, replace0 ? out0 : temp0, replace1 ? out1 : temp1, numFrames );
	
	if ( !replace0 )
	{
		for ( int i=0; i<numFrames; ++i )
		{
			out0[i] += temp0[i];
		}
	}
	if ( !replace1 )
	{
		for ( int i=0; i<numFrames; ++i )
		{
			out1[i] += temp1[i];
		}
	}
}
