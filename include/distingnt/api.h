/*
MIT License

Copyright (c) 2025 Expert Sleepers Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _DISTINGNT_API_H
#define _DISTINGNT_API_H

#include <stdint.h>

enum _NT_version
{
	kNT_apiVersion1 			= 1,

	kNT_apiVersionCurrent 		= kNT_apiVersion1
};

enum _NT_selector
{
	kNT_selector_version,
	kNT_selector_numFactories,
	kNT_selector_factoryInfo,
};

#define NT_MULTICHAR( a, b, c, d )	( ( (uint32_t)a << 0 ) | ( (uint32_t)b << 8 ) | ( (uint32_t)c << 16 ) | ( (uint32_t)d << 24 ) )

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

struct _NT_globals
{
	uint32_t	sampleRate;
};

struct _NT_algorithmRequirements
{
	uint32_t	numParameters;
	uint32_t	sram;
	uint32_t	dram;
	uint32_t	dtc;
	uint32_t	itc;
};

struct _NT_algorithmMemoryPtrs
{
	uint8_t*	sram;
	uint8_t*	dram;
	uint8_t*	dtc;
	uint8_t*	itc;
};

enum
{
	kNT_unitNone,
	kNT_unitEnum,
	kNT_unitDb,
	kNT_unitDb_minInf,
	kNT_unitPercent,
	kNT_unitHz,
	kNT_unitSemitones,
	kNT_unitCents,
	kNT_unitMs,
	kNT_unitSeconds,
	kNT_unitFrames,
	kNT_unitMIDINote,
	kNT_unitMillivolts,
	kNT_unitVolts,
	kNT_unitBPM,

	kNT_unitAudioInput = 100,
	kNT_unitCvInput,
	kNT_unitAudioOutput,
	kNT_unitCvOutput,
	kNT_unitOutputMode,
};

enum
{
	kNT_scalingNone,
	kNT_scaling10,
	kNT_scaling100,
	kNT_scaling1000,
};

struct _NT_parameter
{
	const char* 			name;
	int16_t	 				min;
	int16_t					max;
	int16_t					def;
	uint8_t     			unit;
	uint8_t					scaling;
	char const * const *	enumStrings;
};

struct _NT_parameterPage
{
	const char* 	name;
	uint8_t			numParams;
	const uint8_t*	params;
};

struct _NT_parameterPages
{
	uint32_t					numPages;
	const _NT_parameterPage*	pages;
};

#define NT_PARAMETER_IO( n, m, d, u )	\
		{ .name = n, .min = m, .max = 28, .def = d, .unit = u, .scaling = 0, .enumStrings = NULL },
#define NT_PARAMETER_OUTPUT_MODE( n )	\
		{ .name = n, .min = 0, .max = 1, .def = 0, .unit = kNT_unitOutputMode, .scaling = 0, .enumStrings = NULL },

#define NT_PARAMETER_AUDIO_INPUT( n, m, d )		NT_PARAMETER_IO( n, m, d, kNT_unitAudioInput )
#define NT_PARAMETER_CV_INPUT( n, m, d )		NT_PARAMETER_IO( n, m, d, kNT_unitCvInput )
#define NT_PARAMETER_AUDIO_OUTPUT( n, m, d )	NT_PARAMETER_IO( n, m, d, kNT_unitAudioOutput )
#define NT_PARAMETER_CV_OUTPUT( n, m, d )		NT_PARAMETER_IO( n, m, d, kNT_unitCvOutput )
#define NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( n, m, d )	\
	NT_PARAMETER_IO( n, m, d, kNT_unitAudioOutput )		\
	NT_PARAMETER_OUTPUT_MODE( n " mode" )
#define NT_PARAMETER_CV_OUTPUT_WITH_MODE( n, m, d )		\
	NT_PARAMETER_IO( n, m, d, kNT_unitCvOutput )		\
	NT_PARAMETER_OUTPUT_MODE( n " mode" )

struct _NT_algorithm
{
	const _NT_parameter*		parameters;
	const _NT_parameterPages*	parameterPages;
	const int16_t*				vIncludingCommon;
	const int16_t*				v;
};

struct _NT_factory
{
	uint32_t		guid;
	const char*		name;
	const char*		description;

	void			(*calculateRequirements)( _NT_algorithmRequirements& req );
	_NT_algorithm*	(*construct)( const _NT_algorithmMemoryPtrs& ptrs, const _NT_algorithmRequirements& req );
	void			(*parameterChanged)( _NT_algorithm* self, int p );
	void 			(*step)( _NT_algorithm* self, float* busFrames, int numFramesBy4 );
};

extern "C" {
typedef uintptr_t (_pluginEntry)( _NT_selector selector, uint32_t data );
uintptr_t pluginEntry( _NT_selector selector, uint32_t data );
extern const _NT_globals NT_globals;
}

#endif // _DISTINGNT_API_H
