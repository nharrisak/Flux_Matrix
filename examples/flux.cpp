#include <math.h>
#include <string.h>
#include <new>
#include <limits.h>
#include <distingnt/api.h>
#include "nh_utils.h"

/*
Flux - Dynamic Matrix Mixer
Author: Nathan Harris

Flux is a versatile N x M matrix mixer designed for creating dynamic, evolving signal routings and textures. Go beyond static patches with triggered sequencing, smooth interpolation, and adjustable transition times and curves.

Features:

Flexible N x M Matrix Mixing (up to 6x6, set via Specifications).
Dynamic X/Y Shifting via Manual Offset parameters (-100% to +100%).
Triggered Sequencing: Step input advances internal X/Y shift state.
Sequence Reset: Dedicated trigger input resets internal shift state to zero.
Adjustable Step Size: X/Y Increment parameters control triggered shift amount (-100% to +100%).
Smooth Transitions: Bilinear interpolation calculates effective gains during shifts.
Global Asymmetric Slew: Control the attack ('Slew Up') and decay ('Slew Down') time (ms) for all gain changes.
Gain Shape Control: Adjust the curve of gain transitions from logarithmic (-ve values) through linear (0) to exponential (+ve values).

*/

// --- Configuration ---
enum
{
    kMaxChannels = 6
}; // Maximum inputs or outputs allowed

// --- Parameter Counts ---
const uint32_t kNumShiftParams_count = 2;
const uint32_t kNumTriggerParams_count = 2;
const uint32_t kNumIncrementParams_count = 2;
const uint32_t kNumSlewParams_count = 2;
const uint32_t kNumShapeParams_count = 1;

// --- Fixed Parameter Indices --- (This enum defines the actual indices 0-7)
enum
{
    kParamXOffset = 0,
    kParamYOffset,        // 1
    kParamStepTriggerIn,  // 2
    kParamResetTriggerIn, // 3
    kParamXIncrement,     // 4
    kParamYIncrement,     // 5
    kParamSlewUp,         // 6
    kParamSlewDown,       // 7
    kParamGainShape,      // 8  
    
    // Total fixed parameters used for offsets and page setup
    kNumFixedParams
};

static_assert(kNumFixedParams == 
    kNumShiftParams_count + kNumTriggerParams_count + 
    kNumIncrementParams_count + kNumSlewParams_count + 
    kNumShapeParams_count);

// --- Dynamic Parameter Start Offset ---
const uint32_t kRoutingParamStartOffset = kNumFixedParams;

// --- Parameter Definitions ---
static _NT_parameter gainParameterTemplate = {
    .name = "",
    .min = 0,
    .max = 100,
    .def = 0,
    .unit = kNT_unitPercent,
    .scaling = 0,
    .enumStrings = NULL};

// --- Algorithm Structure ---
struct _fluxAlgorithm : public _NT_algorithm
{
    uint32_t numInputs;
    uint32_t numOutputs;
    _NT_parameter *parameterDefs;
    _NT_parameterPage *pageDefs;
    uint8_t *pageParamsRouting;
    uint8_t *pageParamsGains;
    uint8_t *pageParamsTransform;
    _NT_parameterPages pagesDefs;

    // Stores the final interpolated gain applied in the *previous* step buffer
    // Size: numOutputs * numInputs
    float *previousInterpolatedGains;

    uint32_t selectedInUI;       // Input selected by Pot 1 (UI)
    uint32_t selectedOutUI;      // Output selected by Pot 2 (UI)

    // --- Internal state for triggered shifting ---
    float triggeredXShift; // Accumulated triggered shift [0.0, 1.0)
    float triggeredYShift; // Accumulated triggered shift [0.0, 1.0)
    bool previousStepTriggerState;
    bool previousResetTriggerState;

    // --- Constructor ---
    _fluxAlgorithm(
        uint32_t numInputs_,
        uint32_t numOutputs_,
        _NT_parameter *paramDefsMem,
        _NT_parameterPage *pageDefsMem,
        uint8_t *pageParamsRoutingMem,
        uint8_t *pageParamsGainsMem,
        uint8_t *pageParamsTransformMem,
        float *previousInterpolatedGainsMem)
        : numInputs(numInputs_), numOutputs(numOutputs_),
          parameterDefs(paramDefsMem), pageDefs(pageDefsMem),
          pageParamsRouting(pageParamsRoutingMem),
          pageParamsGains(pageParamsGainsMem),
          pageParamsTransform(pageParamsTransformMem),
          previousInterpolatedGains(previousInterpolatedGainsMem),
          selectedInUI(0),         // Default UI selection to top-left cell
          selectedOutUI(0),
          triggeredXShift(0.0f),
          triggeredYShift(0.0f),
          previousStepTriggerState(false),
          previousResetTriggerState(false)
    {
        // --- Calculate Offsets ---
        uint32_t routingParamOffset = kNumFixedParams;                         // Routing starts after all fixed params
        uint32_t numRoutingParams_Base = numInputs + numOutputs * 2;           // Dynamic routing params
        uint32_t gainParamOffset = routingParamOffset + numRoutingParams_Base; // Gains start after fixed + routing

        // --- Define Parameters ---
        uint32_t pIdx = 0;

        parameterDefs[pIdx++] = _NT_parameter{.name = "X Offset", .min = -100, .max = 100};                                                                               // kParamXOffset = 0 (Implied)
        parameterDefs[pIdx++] = _NT_parameter{.name = "Y Offset", .min = -100, .max = 100};                                                                               // kParamYOffset = 1 (Implied)
        parameterDefs[pIdx++] = _NT_parameter{.name = "Step Trigger In", .min = 0, .max = 28, .unit = kNT_unitCvInput};                                                   // kParamStepTriggerIn = 2
        parameterDefs[pIdx++] = _NT_parameter{.name = "Reset Trigger In", .min = 0, .max = 28, .unit = kNT_unitCvInput};                                                  // kParamResetTriggerIn = 3
        parameterDefs[pIdx++] = _NT_parameter{.name = "X Increment", .min = -100, .max = 100, .def = (int16_t)((1.f / (float)numInputs) * 100), .unit = kNT_unitPercent}; // kParamXIncrement = 4
        parameterDefs[pIdx++] = _NT_parameter{.name = "Y Increment", .min = -100, .max = 100, .def = 0, .unit = kNT_unitPercent};                                         // kParamYIncrement = 5
        parameterDefs[pIdx++] = _NT_parameter{.name = "Slew Up (ms)", .min = 0, .max = 4000, .def = 1, .unit = kNT_unitMs};                                               // kParamSlewUp = 6
        parameterDefs[pIdx++] = _NT_parameter{.name = "Slew Down (ms)", .min = 0, .max = 4000, .def = 1, .unit = kNT_unitMs};                                             // kParamSlewDown = 7
        parameterDefs[pIdx++] = _NT_parameter{
            .name = "Log/Lin/Exp", 
            .min = -100, // -100% = Max Ease-Out
            .max = 100,  // +100% = Max Ease-In
            .def = 0,    // 0% = Linear
            .unit = kNT_unitPercent, // Display as %
            .scaling = 0,
            .enumStrings = NULL
        };

        // ** Dynamic Input Parameters (Indices routingParamOffset to routingParamOffset + numInputs - 1) **
        for (uint32_t i = 0; i < numInputs; ++i, ++pIdx)
        {
            static char inputNames[kMaxChannels][16];
            nh::snprintf(inputNames[i], sizeof(inputNames[i]), "Input %lu", (long unsigned int)(i + 1));
            parameterDefs[pIdx] = _NT_parameter{
                .name = inputNames[i],
                .min = 0,
                .max = 28,
                .def = (int16_t)((i < 4) ? (i + 1) : 0),
                .unit = kNT_unitAudioInput,
                .scaling = 0,
                .enumStrings = NULL};
        }
        // pIdx should now equal routingParamOffset + numInputs

        // ** Dynamic Output Parameters (Indices start after Inputs) **
        for (uint32_t i = 0; i < numOutputs; ++i)
        {
            static char outputNames[kMaxChannels][24];
            static char outputModeNames[kMaxChannels][32];
            nh::snprintf(outputNames[i], sizeof(outputNames[i]), "Output %lu", (i + 1));
            nh::snprintf(outputModeNames[i], sizeof(outputModeNames[i]), "Output %lu mode", (i + 1));
            // Output Bus Parameter
            parameterDefs[pIdx++] = _NT_parameter{
                .name = outputNames[i],
                .min = 0,
                .max = 28,
                .def = (int16_t)((i < 4) ? (i + 13) : 0),
                .unit = kNT_unitAudioOutput,
                .scaling = 0,
                .enumStrings = NULL};

            // Output Mode Parameter
            parameterDefs[pIdx++] = _NT_parameter{
                .name = outputModeNames[i],
                .min = 0,
                .max = 1,
                .def = 0,
                .unit = kNT_unitOutputMode,
                .scaling = 0,
                .enumStrings = NULL};
        }
        // pIdx should now equal routingParamOffset + numRoutingParams_Base == gainParamOffset

        // ** Dynamic Gain Parameters (Indices gainParamOffset onwards) **
        for (uint32_t out = 0; out < numOutputs; ++out)
        {
            for (uint32_t in = 0; in < numInputs; ++in, ++pIdx)
            {
                static char gainNames[kMaxChannels * kMaxChannels][32];
                uint32_t gainNameIdx = out * numInputs + in;
                nh::snprintf(gainNames[gainNameIdx], sizeof(gainNames[gainNameIdx]), "In %lu -> Out %lu", (long unsigned int)(in + 1), (long unsigned int)(out + 1));
                parameterDefs[pIdx] = gainParameterTemplate;
                parameterDefs[pIdx].name = gainNames[gainNameIdx];
                parameterDefs[pIdx].def = 0;
                float initialGain = parameterDefs[pIdx].def / 100.0f;
                previousInterpolatedGains[gainNameIdx] = initialGain;
            }
        }
        // pIdx should now equal total parameters

        // --- Define Parameter Pages ---
        uint32_t numGainPages = numOutputs;
        uint32_t numTotalPages = 1 + 1 + numGainPages; // Routing + Flux + Gains
        pagesDefs.numPages = numTotalPages;
        pagesDefs.pages = pageDefs;

        // ** Page 0: Routing (Now has Trigger params + Dynamic Input/Output params) **
        uint32_t pg = 0;
        pageDefs[pg].name = "Routing";
        uint32_t currentNumRoutingParams = numRoutingParams_Base + kNumTriggerParams_count; // Total params on this page
        pageDefs[pg].numParams = currentNumRoutingParams;
        pageDefs[pg].params = pageParamsRouting;
        uint32_t routingIdx = 0;
        // Add Trigger param indices FIRST (now fixed indices)
        pageParamsRouting[routingIdx++] = kParamStepTriggerIn;  // = 6
        pageParamsRouting[routingIdx++] = kParamResetTriggerIn; // = 7
        // Add Dynamic Input/Output param indices (now starting at routingParamOffset)
        for (uint32_t i = 0; i < numRoutingParams_Base; ++i)
        {
            pageParamsRouting[routingIdx++] = routingParamOffset + i;
        }
        ++pg; // pg is now 1

        // ** Page 1: Flux (Offset, Increment, Slew - now fixed indices) **
        pageDefs[pg].name = "Flux";
        uint32_t numFluxPageParams = kNumShiftParams_count + kNumIncrementParams_count + kNumSlewParams_count + kNumShapeParams_count;
        pageDefs[pg].numParams = numFluxPageParams;
        pageDefs[pg].params = pageParamsTransform;
        pageParamsTransform[0] = kParamXOffset;    // = 0
        pageParamsTransform[1] = kParamYOffset;    // = 1
        pageParamsTransform[2] = kParamXIncrement; // = 4
        pageParamsTransform[3] = kParamYIncrement; // = 5
        pageParamsTransform[4] = kParamSlewUp;     // = 6
        pageParamsTransform[5] = kParamSlewDown;   // = 7
        pageParamsTransform[6] = kParamGainShape;  // = 8
        ++pg;                                      // pg is now 2

        // ** Pages 2 onwards: Gain Pages (Dynamic gain indices starting from gainParamOffset) **
        uint8_t *currentGainPageParams = pageParamsGains;
        for (uint32_t out = 0; out < numOutputs; ++out)
        {
            static char pageNames[kMaxChannels][24];
            nh::snprintf(pageNames[out], sizeof(pageNames[out]), "Output %lu Gains", (long unsigned int)(out + 1));
            pageDefs[pg + out].name = pageNames[out]; // Index is 2+out
            pageDefs[pg + out].numParams = numInputs;
            pageDefs[pg + out].params = currentGainPageParams;
            for (uint32_t in = 0; in < numInputs; ++in)
            {
                currentGainPageParams[in] = gainParamOffset + out * numInputs + in;
            }
            currentGainPageParams += numInputs;
        }

        // --- Populate _NT_algorithm members ---
        parameters = parameterDefs;
        parameterPages = &pagesDefs;
    }

    // Helper to get the gain parameter index for a given UI cell
    // Returns UINT32_MAX if coordinates are invalid (shouldn't happen with uint32_t)
    // or if gain parameters aren't set up yet.
    uint32_t getGainParameterIndex(uint32_t out_idx, uint32_t in_idx) const {
        if (in_idx >= numInputs || out_idx >= numOutputs) {
            return UINT32_MAX; // Invalid coordinates
        }
        // Calculate dynamic start index of gain parameters (same logic as in constructor/step)
        uint32_t numRoutingParams_Base = numInputs + numOutputs * 2;
        uint32_t gainParamOffset = kNumFixedParams + numRoutingParams_Base;
        uint32_t gainIndexRelative = out_idx * numInputs + in_idx;
        uint32_t gainParamIdxAbsolute = gainParamOffset + gainIndexRelative;

        // Boundary check (ensure it's within the total number of parameters)
        uint32_t totalParams = gainParamOffset + numInputs * numOutputs;
         if (gainParamIdxAbsolute >= totalParams) {
              return UINT32_MAX; // Should not happen if calculation is correct
         }

        return gainParamIdxAbsolute;
    }
};

// --- Factory Functions ---

void calculateRequirements(_NT_algorithmRequirements &req, const int32_t *specifications)
{
    int32_t numInputs = specifications[0];
    int32_t numOutputs = specifications[1];
    if (numInputs < 1)
        numInputs = 1;
    if (numInputs > kMaxChannels)
        numInputs = kMaxChannels;
    if (numOutputs < 1)
        numOutputs = 1;
    if (numOutputs > kMaxChannels)
        numOutputs = kMaxChannels;

    uint32_t numGainParams = numInputs * numOutputs;
    uint32_t numRoutingParams_Base = numInputs + numOutputs * 2; // Dynamic routing params

    req.numParameters = kNumFixedParams + numRoutingParams_Base + numGainParams; // Correct total

    uint32_t numTotalPages = 1 + 1 + numOutputs; // Routing + Flux + Gains

    uint32_t numRoutingPageParams_Actual = kNumTriggerParams_count + numRoutingParams_Base;                       // Triggers + Dynamic IO
    
    uint32_t numFluxPageParams_Actual = 
        kNumShiftParams_count + kNumIncrementParams_count + 
        kNumSlewParams_count + kNumShapeParams_count; // = 7

    req.sram = sizeof(_fluxAlgorithm);
    req.sram += req.numParameters * sizeof(_NT_parameter);
    req.sram += numTotalPages * sizeof(_NT_parameterPage);
    req.sram += numRoutingPageParams_Actual * sizeof(uint8_t); // Routing page params
    req.sram += numGainParams * sizeof(uint8_t);               // Gain pages params
    req.sram += numFluxPageParams_Actual * sizeof(uint8_t);    // Flux page params
    req.sram += numGainParams * sizeof(float);                 // For previousInterpolatedGains
    req.sram += 128;                                           // Padding

    req.dram = 0;
    req.dtc = 0;
    req.itc = 0;
}

_NT_algorithm *construct(const _NT_algorithmMemoryPtrs &ptrs, const _NT_algorithmRequirements &req, const int32_t *specifications)
{
    int32_t numInputs = specifications[0];
    int32_t numOutputs = specifications[1];
    if (numInputs < 1)
        numInputs = 1;
    if (numInputs > kMaxChannels)
        numInputs = kMaxChannels;
    if (numOutputs < 1)
        numOutputs = 1;
    if (numOutputs > kMaxChannels)
        numOutputs = kMaxChannels;

    uint32_t numGainParams = numInputs * numOutputs;
    uint32_t numRoutingParams_Base = numInputs + numOutputs * 2;
    uint32_t numTotalPages = 1 + numOutputs + 1;
    // Calculate sizes for page index arrays
    uint32_t numRoutingPageParams_Actual = kNumTriggerParams_count + numRoutingParams_Base;
    uint32_t numFluxPageParams_Actual = kNumShiftParams_count + kNumIncrementParams_count + kNumSlewParams_count + kNumShapeParams_count; // = 7

    // Divide the allocated SRAM block
    uint8_t *memPtr = ptrs.sram;
    // ... allocate paramDefsMem, pageDefsMem ...
    memPtr = nh::align_pointer(memPtr, 4);
    _NT_parameter *paramDefsMem = (_NT_parameter *)memPtr;
    memPtr += req.numParameters * sizeof(_NT_parameter);
    memPtr = nh::align_pointer(memPtr, 4);
    _NT_parameterPage *pageDefsMem = (_NT_parameterPage *)memPtr;
    memPtr += numTotalPages * sizeof(_NT_parameterPage);

    // Allocate Routing page params list
    memPtr = nh::align_pointer(memPtr, 4);
    uint8_t *pageParamsRoutingMem = memPtr;
    memPtr += numRoutingPageParams_Actual * sizeof(uint8_t);

    // Allocate Gains page params list
    memPtr = nh::align_pointer(memPtr, 4);
    uint8_t *pageParamsGainsMem = memPtr;
    memPtr += numGainParams * sizeof(uint8_t);

    // Allocate Transform(Flux) page params list
    memPtr = nh::align_pointer(memPtr, 4);
    uint8_t *pageParamsTransformMem = memPtr;
    memPtr += numFluxPageParams_Actual * sizeof(uint8_t);

    memPtr = nh::align_pointer(memPtr, 4);
    float *previousInterpolatedGainsMem = (float *)memPtr;
    memPtr += numGainParams * sizeof(float);

    memPtr = nh::align_pointer(memPtr, 4);
    _fluxAlgorithm *alg = new (memPtr) _fluxAlgorithm(
        numInputs, numOutputs,
        paramDefsMem, pageDefsMem, pageParamsRoutingMem, pageParamsGainsMem, pageParamsTransformMem,
        previousInterpolatedGainsMem);
    return alg;
}

/*
void parameterChanged(_NT_algorithm *self, int p)
{
    _fluxAlgorithm *pThis = (_fluxAlgorithm *)self;
    uint32_t parameter_p = (uint32_t)p;

    // Calculate gain parameter range
    uint32_t numRoutingParams_Base = pThis->numInputs + pThis->numOutputs * 2;
    uint32_t gainParamOffset = kNumFixedParams + numRoutingParams_Base;
    uint32_t numGainParams = pThis->numInputs * pThis->numOutputs;
    uint32_t endGainParams = gainParamOffset + numGainParams; // End of gain param indices

    // Check if it's a gain parameter
    if (parameter_p >= gainParamOffset && parameter_p < endGainParams)
    {
        uint32_t gainIndex = parameter_p - gainParamOffset;
        // Update selected cell coordinates
        pThis->selectedOut = gainIndex / pThis->numInputs;
        pThis->selectedIn = gainIndex % pThis->numInputs;
    }
    else // Parameter is Fixed, Routing, or otherwise not Gain
    {
        // Reset selection highlight if focus moves off a gain cell
        pThis->selectedIn = UINT32_MAX;
        pThis->selectedOut = UINT32_MAX;
    }
}*/

uint32_t fluxHasCustomUi(_NT_algorithm *self)
{
    return 
    kNT_potL | 
    kNT_potC |
    kNT_potR | 
    kNT_encoderL | 
    kNT_encoderR | 
    kNT_button3 | 
    kNT_button4 ;
}

void fluxSetupUi(_NT_algorithm *self, _NT_float3 &pots)
{
    _fluxAlgorithm *pThis = (_fluxAlgorithm *)self;

    // Pot 1 (Left): Represents selectedInUI
    // Map [0, numInputs-1] to [0.0, 1.0]
    if (pThis->numInputs > 1) {
        pots[0] = (float)pThis->selectedInUI / (float)(pThis->numInputs - 1);
    } else {
        pots[0] = 0.5f; // Center if only one input
    }
    pots[0] = fmaxf(0.0f, fminf(1.0f, pots[0])); // Clamp just in case

    // Pot 2 (Centre): Represents selectedOutUI
    // Map [0, numOutputs-1] to [0.0, 1.0]
     if (pThis->numOutputs > 1) {
        pots[1] = (float)pThis->selectedOutUI / (float)(pThis->numOutputs - 1);
    } else {
        pots[1] = 0.5f; // Center if only one output
    }
    pots[1] = fmaxf(0.0f, fminf(1.0f, pots[1])); // Clamp

    // Pot 3 (Right): Represents gain of the cell at (selectedOutUI, selectedInUI)
    uint32_t gainParamIdx = pThis->getGainParameterIndex(pThis->selectedOutUI, pThis->selectedInUI);
    if (gainParamIdx != UINT32_MAX && gainParamIdx < (kNumFixedParams + (pThis->numInputs + pThis->numOutputs * 2) + (pThis->numInputs*pThis->numOutputs)) ) // Check validity and bounds
    {
        // Parameter value is [0, 100], map to [0.0, 1.0]
        pots[2] = (float)pThis->v[gainParamIdx] / 100.0f;
    } else {
         pots[2] = 0.0f; // Default to 0 if selection is invalid (shouldn't be)
    }
     pots[2] = fmaxf(0.0f, fminf(1.0f, pots[2])); // Clamp
}

void fluxCustomUi(_NT_algorithm *self, const _NT_uiData &data)
{
    _fluxAlgorithm *pThis = (_fluxAlgorithm *)self;
    int32_t algoIndex = NT_algorithmIndex(self); // Get algorithm index once

    // --- Handle Pot 1 (Left) - Select Input (X-axis) ---
    if (data.controls & kNT_potL)
    {
        uint32_t newSelectedIn = 0;
        if (pThis->numInputs > 0) {
             newSelectedIn = (uint32_t)roundf(data.pots[0] * (float)(pThis->numInputs - 1));
             newSelectedIn = (newSelectedIn >= pThis->numInputs) ? (pThis->numInputs > 0 ? pThis->numInputs - 1 : 0) : newSelectedIn;
        }
        if (newSelectedIn != pThis->selectedInUI) {
            pThis->selectedInUI = newSelectedIn;
        }
    }

    // --- Handle Pot 2 (Centre) - Select Output (Y-axis) ---
    if (data.controls & kNT_potC)
    {
         uint32_t newSelectedOut = 0;
         if (pThis->numOutputs > 0) {
            newSelectedOut = (uint32_t)roundf(data.pots[1] * (float)(pThis->numOutputs - 1));
            newSelectedOut = (newSelectedOut >= pThis->numOutputs) ? (pThis->numOutputs > 0 ? pThis->numOutputs - 1 : 0) : newSelectedOut;
         }
        if (newSelectedOut != pThis->selectedOutUI) {
            pThis->selectedOutUI = newSelectedOut;
        }
    }

    // --- Handle Pot 3 (Right) - Adjust Gain for selected cell ---
    if (data.controls & kNT_potR)
    {
        uint32_t gainParamIdx = pThis->getGainParameterIndex(pThis->selectedOutUI, pThis->selectedInUI);
        if (gainParamIdx != UINT32_MAX && algoIndex >= 0)
        {
             int16_t newValue = (int16_t)roundf(data.pots[2] * 100.0f);
             newValue = (newValue < 0) ? 0 : (newValue > 100 ? 100 : newValue);
             NT_setParameterFromUi((uint32_t)algoIndex, gainParamIdx + NT_parameterOffset(), newValue);
        }
    }

    // --- Handle Encoder 1 (Left) - Modify X Offset ---
    if (data.encoders[0] != 0 && algoIndex >= 0) // Left encoder changed
    {
        // Get current value of X Offset parameter
        int16_t currentValue = pThis->v[kParamXOffset];
        // Calculate new value - let 1 encoder click = 1% change
        int16_t newValue = currentValue + data.encoders[0];
        // Clamp to parameter range [-100, 100]
        newValue = (newValue < -100) ? -100 : (newValue > 100 ? 100 : newValue);
        // Set the parameter
        NT_setParameterFromUi((uint32_t)algoIndex, kParamXOffset + NT_parameterOffset(), newValue);
    }

    // --- Handle Encoder 2 (Right) - Modify Y Offset ---
     if (data.encoders[1] != 0 && algoIndex >= 0) // Right encoder changed
    {
        // Get current value of Y Offset parameter
        int16_t currentValue = pThis->v[kParamYOffset];
        // Calculate new value - let 1 encoder click = 1% change
        int16_t newValue = currentValue + data.encoders[1];
        // Clamp to parameter range [-100, 100]
        newValue = (newValue < -100) ? -100 : (newValue > 100 ? 100 : newValue);
        // Set the parameter
        NT_setParameterFromUi((uint32_t)algoIndex, kParamYOffset + NT_parameterOffset(), newValue);
    }

    // --- Handle Button 3 - Step Sequence ---
    // Check for rising edge (pressed now, wasn't pressed before)
    if ((data.controls & kNT_button3) && !(data.lastButtons & kNT_button3))
    {
        // Replicate logic from step() function for Step Trigger
        // Get current increment values
        float xIncNorm = pThis->v[kParamXIncrement] / 100.0f;
        float yIncNorm = pThis->v[kParamYIncrement] / 100.0f;

        // Apply increments to internal triggered shift state using fmodf for wrapping
        // Add 1.0f before fmodf to handle negative increments correctly with fmodf result range.
        pThis->triggeredXShift = nh::fmodf(pThis->triggeredXShift + xIncNorm + 1.0f, 1.0f);
        pThis->triggeredYShift = nh::fmodf(pThis->triggeredYShift + yIncNorm + 1.0f, 1.0f);
    }

    // --- Handle Button 4 - Reset Sequence ---
    // Check for rising edge
    if ((data.controls & kNT_button4) && !(data.lastButtons & kNT_button4))
    {
        // Replicate logic from step() function for Reset Trigger
        pThis->triggeredXShift = 0.0f;
        pThis->triggeredYShift = 0.0f;
    }
}

// Helper function to get the source gain value [0,1] directly from parameters
static inline float GetSourceGain(const _fluxAlgorithm *pThis, uint32_t m, uint32_t n)
{
    // Calculate the dynamic start index of gain parameters using formula
    uint32_t numRoutingParams_Base = pThis->numInputs + pThis->numOutputs * 2;
    uint32_t gainParamOffset = kNumFixedParams + numRoutingParams_Base;

    // Calculate the absolute parameter index for the gain at (m, n)
    uint32_t gainIndex = m * pThis->numInputs + n;
    uint32_t gainParamIdx = gainParamOffset + gainIndex;

    // Defensive check - Calculate end of valid gain params
    uint32_t numGainParams = pThis->numInputs * pThis->numOutputs;
    uint32_t endGainParams = gainParamOffset + numGainParams;
    if (gainParamIdx >= endGainParams)
        return 0.0f; // Check against correct end

    // Access parameter value directly and normalize
    return pThis->v[gainParamIdx] / 100.0f;
}

void step(_NT_algorithm *self, float *busFrames, int numFramesBy4)
{
    _fluxAlgorithm *pThis = (_fluxAlgorithm *)self;
    int numFrames = numFramesBy4 * 4;
    uint32_t numInputs = pThis->numInputs;
    uint32_t numOutputs = pThis->numOutputs;

    float sampleRate = (float)NT_globals.sampleRate; // Get sample rate
    // Avoid division by zero if sample rate isn't ready (unlikely)
    if (sampleRate < 1.0f)
        sampleRate = 48000.0f; // Default fallback

    // --- Calculate dynamic offsets ---
    uint32_t routingParamOffset = kNumFixedParams;
    uint32_t outputParamOffset_withinRouting = numInputs; // Offset of first output param *within its block*

    // --- Get parameter values using FIXED indices ---
    float xOffsetNorm = pThis->v[kParamXOffset] / 100.0f;
    float yOffsetNorm = pThis->v[kParamYOffset] / 100.0f;
    int stepTrigBus = pThis->v[kParamStepTriggerIn];
    int resetTrigBus = pThis->v[kParamResetTriggerIn];
    float xIncNorm = pThis->v[kParamXIncrement] / 100.0f;
    float yIncNorm = pThis->v[kParamYIncrement] / 100.0f;
    float slewUpMs = (float)pThis->v[kParamSlewUp];
    float slewDownMs = (float)pThis->v[kParamSlewDown];
    int shapeParamVal = pThis->v[kParamGainShape]; // Value -100 to +100
    // --- End fixed index access ---

    // --- Calculate Slew Rates ---
    // Calculate max change per sample (handle 0ms case -> very large rate)
    const float minSlewTimeMs = 0.1f; // Threshold below which rate is ~infinite
    float maxSlewUpPerSample = (slewUpMs < minSlewTimeMs) ? 1.0e9f : (1.0f / (slewUpMs * 0.001f * sampleRate));
    float maxSlewDownPerSample = (slewDownMs < minSlewTimeMs) ? 1.0e9f : (1.0f / (slewDownMs * 0.001f * sampleRate));

    // --- Calculate Shape Exponent ---
    // Map parameter (-100 to 100) to exponent (e.g., 0.2 to 5.0)
    const float kMinShapeExponent = 0.2f; // Adjust for desired max ease-out
    const float kMaxShapeExponent = 5.0f; // Adjust for desired max ease-in
    float shapeExponent = 1.0f;           // Default linear
    if (shapeParamVal < 0)
    {                                                              // Ease-out range (-100 to -1) -> (kMinShapeExponent to 1.0)
        float t = (float)shapeParamVal / -100.0f;                  // t = 0.01 to 1.0
        shapeExponent = kMinShapeExponent * t + 1.0f * (1.0f - t); // Linear interpolation
    }
    else if (shapeParamVal > 0)
    {                                                              // Ease-in range (1 to 100) -> (1.0 to kMaxShapeExponent)
        float t = (float)shapeParamVal / 100.0f;                   // t = 0.01 to 1.0
        shapeExponent = 1.0f * (1.0f - t) + kMaxShapeExponent * t; // Linear interpolation
    }

    // --- Trigger Detection & Internal State Update ---
    bool resetTrigJustFired = false;
    // Reset Trigger
    if (resetTrigBus > 0)
    {
        const float *resetSignal = busFrames + (resetTrigBus - 1) * numFrames;
        // Simple threshold check on first sample for edge detection
        bool resetState = resetSignal[0] >= 1.0f;
        if (resetState && !pThis->previousResetTriggerState)
        {
            pThis->triggeredXShift = 0.0f;
            pThis->triggeredYShift = 0.0f;
            resetTrigJustFired = true; // Note that reset happened
        }
        pThis->previousResetTriggerState = resetState;
    }
    else
    {
        pThis->previousResetTriggerState = false; // Ensure state is low if disconnected
    }

    // Step Trigger (only process if reset didn't just fire)
    if (!resetTrigJustFired)
    {
        if (stepTrigBus > 0)
        {
            const float *stepSignal = busFrames + (stepTrigBus - 1) * numFrames;
            bool stepState = stepSignal[0] >= 1.0f;
            if (stepState && !pThis->previousStepTriggerState)
            {
                // Rising edge: Apply increments
                pThis->triggeredXShift = nh::fmodf(pThis->triggeredXShift + xIncNorm + 1.0f, 1.0f);
                pThis->triggeredYShift = nh::fmodf(pThis->triggeredYShift + yIncNorm + 1.0f, 1.0f);
            }
            pThis->previousStepTriggerState = stepState;
        }
        else
        {
            pThis->previousStepTriggerState = false;
        }
    }
    else
    {
        // If reset just fired, make sure step trigger state doesn't persist falsely
        pThis->previousStepTriggerState = false;
    }
    // --- End Trigger Logic ---

    // --- Calculate Effective Shift ---
    float effectiveXShift = nh::fmodf(pThis->triggeredXShift + xOffsetNorm + 1.0f, 1.0f);
    float effectiveYShift = nh::fmodf(pThis->triggeredYShift + yOffsetNorm + 1.0f, 1.0f);

    // --- Prepare Output Buffers (Zeroing for 'Replace' mode) ---
    for (uint32_t m = 0; m < numOutputs; ++m)
    {
        int outputBusParamIdx = routingParamOffset + outputParamOffset_withinRouting + m * 2;
        int outputModeParamIdx = outputBusParamIdx + 1;
        int outputBus = pThis->v[outputBusParamIdx];
        bool replace = pThis->v[outputModeParamIdx];
        if (outputBus > 0 && replace)
        {
            float *out = busFrames + (outputBus - 1) * numFrames;
            memset(out, 0, numFrames * sizeof(float));
        }
    }

    // --- Process Matrix with Interpolation ---
    for (uint32_t m = 0; m < numOutputs; ++m)
    {
        // Target Output Row 'm'
        int outputBusParamIdx = routingParamOffset + outputParamOffset_withinRouting + m * 2;
        int outputBus = pThis->v[outputBusParamIdx];
        if (outputBus <= 0)
            continue;
        float *out = busFrames + (outputBus - 1) * numFrames;

        for (uint32_t n = 0; n < numInputs; ++n)
        {
            // Target Input Column 'n'
            int inputBusParamIdx = routingParamOffset + n; // Input params start at routingParamOffset
            int inputBus = pThis->v[inputBusParamIdx];
            if (inputBus <= 0)
                continue;
            const float *in = busFrames + (inputBus - 1) * numFrames;

            // === Bilinear Interpolation Logic ===

            // 1. Calculate float source coordinates based on target (m,n) and shifts
            // We subtract the shift because we want to know *which source(s)* map to the *current target* (m, n)
            float n_shifted = (float)n - effectiveXShift * (float)numInputs;  // Use effective shift
            float m_shifted = (float)m - effectiveYShift * (float)numOutputs; // Use effective shift

            // Use fmodf to wrap coordinates, ensuring the result is positive before the final modulo.
            // Adding 2*N before the first fmodf handles large negative shifts robustly.
            float source_n_float = nh::fmodf(nh::fmodf(n_shifted, (float)numInputs) + (float)numInputs, (float)numInputs);
            float source_m_float = nh::fmodf(nh::fmodf(m_shifted, (float)numOutputs) + (float)numOutputs, (float)numOutputs);

            // 2. Get integer indices of the 4 surrounding source cells
            int n0 = (int)floorf(source_n_float);
            int m0 = (int)floorf(source_m_float);

            // Indices must wrap around the matrix dimensions
            int n1 = (n0 + 1) % numInputs;
            int m1 = (m0 + 1) % numOutputs;

            // 3. Get fractional parts (interpolation weights)
            float frac_n = source_n_float - (float)n0; // Weight towards n1 as this increases
            float frac_m = source_m_float - (float)m0; // Weight towards m1 as this increases

            // 4. Get gains [0,1] from the 4 source neighbor parameters
            float g00 = GetSourceGain(pThis, m0, n0); // Gain at source row m0, col n0
            float g01 = GetSourceGain(pThis, m0, n1); // Gain at source row m0, col n1
            float g10 = GetSourceGain(pThis, m1, n0); // Gain at source row m1, col n0
            float g11 = GetSourceGain(pThis, m1, n1); // Gain at source row m1, col n1

            // 5. Perform bilinear interpolation
            // 5a. Interpolate along n (x-axis) for both rows
            float interp_m0 = g00 * (1.0f - frac_n) + g01 * frac_n; // Interpolated value on row m0
            float interp_m1 = g10 * (1.0f - frac_n) + g11 * frac_n; // Interpolated value on row m1

            // 5b. Interpolate along m (y-axis) between the results from step 5a
            float targetGain = interp_m0 * (1.0f - frac_m) + interp_m1 * frac_m; // Final interpolated gain for target cell (m, n)
            // === End Interpolation Logic ===

            // === Slew/Ramping Logic ===
            uint32_t targetIndex = m * numInputs + n;
            float startGain = pThis->previousInterpolatedGains[targetIndex];
            float diff = targetGain - startGain;
            float endGain = targetGain; // Default to target if no slew needed or diff is zero

            if (fabsf(diff) >= 1e-6f) // Only apply slew if change is needed
            {
                float maxChangeThisBuffer;
                if (diff > 0)
                { // Going Up
                    maxChangeThisBuffer = maxSlewUpPerSample * (float)numFrames;
                    endGain = startGain + fminf(diff, maxChangeThisBuffer); // Clamp upward change
                }
                else
                { // Going Down
                    maxChangeThisBuffer = maxSlewDownPerSample * (float)numFrames;
                    // Clamp downward change (diff is negative, maxChangeThisBuffer is positive)
                    endGain = startGain + fmaxf(diff, -maxChangeThisBuffer);
                }
            }
            // Now 'endGain' is the value we should linearly ramp towards *in this buffer*

            // === Audio Processing Loop (Apply Shape) ===
            if (fabsf(endGain - startGain) < 1e-6f) // If no change this buffer
            { 
                if (fabsf(startGain) > 1e-6f)
                {
                    // Apply potentially shaped constant gain
                    float linearGain = startGain;
                    float shapedGain = linearGain;
                    if (fabsf(shapeExponent - 1.0f) > 1e-6f) // Apply shaping if not linear
                    {                                                             
                        shapedGain = powf(fmaxf(0.0f, linearGain), shapeExponent); // Use powf
                    }
                    for (int i = 0; i < numFrames; ++i)
                    {
                        out[i] += in[i] * shapedGain; // Use shaped gain
                    }
                }
            }
            else // Ramp needed from startGain to endGain
            { 
#if 1
                float shapedStartGain = startGain;
                float shapedEndGain = endGain;
                if (fabsf(shapeExponent - 1.0f) > 1e-6f) // Apply shaping if not linear
                {                                                             
                    shapedStartGain = powf(fmaxf(0.0f, startGain), shapeExponent); // Use powf
                    shapedEndGain = powf(fmaxf(0.0f, endGain), shapeExponent); // Use powf
                }
                float gainStep = (shapedEndGain - shapedStartGain) / (float)numFrames;
                float currentFrameGain = shapedStartGain;
                for (int i = 0; i < numFrames; ++i)
                {
                    out[i] += in[i] * currentFrameGain; // Apply shaped gain
                    currentFrameGain += gainStep; // Step the *linear* gain for next iteration
                }
#else
                float gainStep = (endGain - startGain) / (float)numFrames;
                float currentFrameGain = startGain; // Linear value for ramp step
                for (int i = 0; i < numFrames; ++i)
                {
                    // Calculate shaped gain for this sample
                    float linearGain = currentFrameGain;
                    float shapedGain = linearGain;
                    if (fabsf(shapeExponent - 1.0f) > 1e-6f)
                    {                                                              // Apply shaping if not linear
                        shapedGain = powf(fmaxf(0.0f, linearGain), shapeExponent); // Use powf
                    }
                    out[i] += in[i] * shapedGain; // Apply shaped gain
                    currentFrameGain += gainStep; // Step the *linear* gain for next iteration
                }
#endif
            }

            // Update previous gain state with the value actually reached
            pThis->previousInterpolatedGains[targetIndex] = endGain;
            // === End Ramping and Processing ===

        } // End input loop (n)
    } // End output loop (m)
}

// --- Draw Function ---

// Drawing Constants
const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 64;
const int LABEL_AREA_WIDTH = 18;
const int LABEL_AREA_HEIGHT = 8;
const int PARAM_AREA_HEIGHT = 8;
const int GRID_PADDING = 1;

const int MIN_CELL_WIDTH_FOR_TEXT = 15;
const int MIN_CELL_HEIGHT_FOR_TEXT = 8;
const int BOTTOM_TEXT_MARGIN = 4;

// Highlight colors
const int BORDER_COLOR_NORMAL = 2;
const int BORDER_COLOR_SELECTED = 15;
const int GAIN_BAR_COLOR = 5;
const int HIGHLIGHT_COLOR = 15;
const int TEXT_VERT_ADJUST_TINY = 3;
const int TEXT_VERT_ADJUST_NORM = 4;

bool fluxDraw(_NT_algorithm *self)
{
    _fluxAlgorithm *pThis = (_fluxAlgorithm *)self;
    uint32_t numInputs = pThis->numInputs;
    uint32_t numOutputs = pThis->numOutputs;

    // --- Calculate Grid Geometry ---
    int gridStartX = LABEL_AREA_WIDTH;
    int gridStartY = LABEL_AREA_HEIGHT + PARAM_AREA_HEIGHT;
    int gridWidth = SCREEN_WIDTH - gridStartX - GRID_PADDING;
    int gridHeight = SCREEN_HEIGHT - gridStartY - GRID_PADDING - BOTTOM_TEXT_MARGIN;
    if (numInputs == 0 || numOutputs == 0 || gridWidth <= 0 || gridHeight <= 0)
    {
        NT_drawText(5, 5, "Invalid Config", 15, kNT_textLeft, kNT_textNormal);
        return true;
    }
    int cellWidth = gridWidth / numInputs;
    int cellHeight = gridHeight / numOutputs;
    if (cellWidth <= GRID_PADDING * 2 || cellHeight <= GRID_PADDING * 2)
    {
        NT_drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 4, "Grid too small", 15, kNT_textCentre, kNT_textNormal);
        return true;
    }
    bool drawTextValues = (cellWidth >= MIN_CELL_WIDTH_FOR_TEXT && cellHeight >= MIN_CELL_HEIGHT_FOR_TEXT);
    _NT_textSize valueTextSize = kNT_textNormal;

    // --- Get Manual Offsets & Calculate Effective Shift ---
    float manualXOffsetNorm = pThis->v[kParamXOffset] / 100.0f;
    float manualYOffsetNorm = pThis->v[kParamYOffset] / 100.0f;
    // Use current internal triggered state
    float effectiveXShift = nh::fmodf(pThis->triggeredXShift + manualXOffsetNorm + 1.0f, 1.0f);
    float effectiveYShift = nh::fmodf(pThis->triggeredYShift + manualYOffsetNorm + 1.0f, 1.0f);
    // --- End Effective Shift ---

    // --- Draw Labels ---
    char labelBuf[8];
    for (uint32_t n = 0; n < numInputs; ++n)
    {
        nh::snprintf(labelBuf, sizeof(labelBuf), "I%lu", (long unsigned int)(n + 1));
        int labelX = gridStartX + n * cellWidth + cellWidth / 2;
        int labelY = PARAM_AREA_HEIGHT + (LABEL_AREA_HEIGHT / 2) + TEXT_VERT_ADJUST_TINY;
        NT_drawText(labelX, labelY, labelBuf, 12, kNT_textCentre, kNT_textTiny);
    }
    for (uint32_t m = 0; m < numOutputs; ++m)
    {
        nh::snprintf(labelBuf, sizeof(labelBuf), "O%lu", (long unsigned int)(m + 1));
        int labelX = LABEL_AREA_WIDTH - GRID_PADDING - 1;
        int labelY = gridStartY + m * cellHeight + (cellHeight / 2) + TEXT_VERT_ADJUST_TINY;
        NT_drawText(labelX, labelY, labelBuf, 12, kNT_textRight, kNT_textTiny);
    }

    // --- Draw Grid and Cell Contents ---
    char valueBuf[5]; // For text display "100\0"

    for (uint32_t m = 0; m < numOutputs; ++m)
    { // Target Row 'm'
        for (uint32_t n = 0; n < numInputs; ++n)
        { // Target Column 'n'
            int cellX = gridStartX + n * cellWidth;
            int cellY = gridStartY + m * cellHeight;

            // Check if this cell corresponds to the focused PARAMETER
            bool isSelected = (m == pThis->selectedOutUI && n == pThis->selectedInUI); // Check if this cell is selected by Pots

            // === Calculate EFFECTIVE gain using effective shifts ===
            float n_shifted = (float)n - effectiveXShift * (float)numInputs;  // Use effective
            float m_shifted = (float)m - effectiveYShift * (float)numOutputs; // Use effective
            float source_n_float = nh::fmodf(nh::fmodf(n_shifted, (float)numInputs) + (float)numInputs, (float)numInputs);
            float source_m_float = nh::fmodf(nh::fmodf(m_shifted, (float)numOutputs) + (float)numOutputs, (float)numOutputs);
            int n0 = (int)floorf(source_n_float);
            int m0 = (int)floorf(source_m_float);
            int n1 = (n0 + 1) % numInputs;
            int m1 = (m0 + 1) % numOutputs;
            float frac_n = source_n_float - (float)n0;
            float frac_m = source_m_float - (float)m0;
            float g00 = GetSourceGain(pThis, m0, n0);
            float g01 = GetSourceGain(pThis, m0, n1);
            float g10 = GetSourceGain(pThis, m1, n0);
            float g11 = GetSourceGain(pThis, m1, n1);
            float interp_m0 = g00 * (1.0f - frac_n) + g01 * frac_n;
            float interp_m1 = g10 * (1.0f - frac_n) + g11 * frac_n;
            float effectiveGain = interp_m0 * (1.0f - frac_m) + interp_m1 * frac_m; // Gain [0, 1]

            // Convert float gain [0,1] to int [0,100] for display
            int displayGainValue = (int)roundf(effectiveGain * 100.0f);
            if (displayGainValue < 0)
                displayGainValue = 0;
            if (displayGainValue > 100)
                displayGainValue = 100;
            // === End effective gain calculation ===

            // 1. Draw Cell Background/Border (Highlight based on PARAMETER focus)
            int borderColor = isSelected ? BORDER_COLOR_SELECTED : BORDER_COLOR_NORMAL;
            NT_drawShapeI(kNT_box, cellX, cellY, cellX + cellWidth - 1, cellY + cellHeight - 1, borderColor);

            // 2. Draw Gain Bar (use EFFECTIVE interpolated gain)
            int barMaxHeight = cellHeight - GRID_PADDING * 2;
            if (barMaxHeight < 1)
                barMaxHeight = 1;
            int barHeight = (int)(((float)displayGainValue / 100.0f) * barMaxHeight); // Use displayGainValue
            if (barHeight < 0)
                barHeight = 0;
            if (barHeight > barMaxHeight)
                barHeight = barMaxHeight; // Clamp
            int barWidth = cellWidth - GRID_PADDING * 2;
            if (barWidth < 1)
                barWidth = 1;
            int barX = cellX + GRID_PADDING;
            int barY = cellY + GRID_PADDING + (barMaxHeight - barHeight); // Bar grows from bottom
            if (barHeight > 0)
            {
                NT_drawShapeI(kNT_rectangle, barX, barY, barX + barWidth - 1, barY + barHeight - 1, GAIN_BAR_COLOR);
            }

            // 3. Draw Text Value (use EFFECTIVE interpolated gain)
            if (drawTextValues)
            {
                nh::snprintf(valueBuf, sizeof(valueBuf), "%d", displayGainValue); // Use displayGainValue
                int textX = cellX + cellWidth / 2;
                int textY = cellY + (cellHeight / 2) + TEXT_VERT_ADJUST_NORM;
                NT_drawText(textX, textY, valueBuf, isSelected ? 15 : 10, kNT_textCentre, valueTextSize);
            }

            // 4. Draw Extra Highlight Border (Highlight based on PARAMETER focus)
            if (isSelected)
            {
                NT_drawShapeI(kNT_box, cellX + 1, cellY + 1, cellX + cellWidth - 2, cellY + cellHeight - 2, HIGHLIGHT_COLOR);
            }

        } // End input loop (n)
    } // End output loop (m)

    // --- Draw EFFECTIVE Shift Parameter Values ---
    char shiftBuf[24]; // Might need slightly more space "X:-100% Y:-100%" ? No, effective is 0-100
    // Calculate effective shift for display
    int xEffDisplay = (int)roundf(effectiveXShift * 100.0f);
    int yEffDisplay = (int)roundf(effectiveYShift * 100.0f);
    if (xEffDisplay == 100)
        xEffDisplay = 0; // Wrap 100% back to 0% for display consistency? Optional.
    if (yEffDisplay == 100)
        yEffDisplay = 0;
    nh::snprintf(shiftBuf, sizeof(shiftBuf), "X:%d%% Y:%d%%", xEffDisplay, yEffDisplay); // Changed label
    NT_drawText(gridStartX, SCREEN_HEIGHT - 1, shiftBuf, 14, kNT_textLeft, kNT_textTiny);

    return false; // Return false, it is useful to see the default parameter line at the top.
}

// --- Specifications ---
static const _NT_specification specifications[] = {
    {.name = "Inputs", .min = 1, .max = kMaxChannels, .def = 4, .type = kNT_typeGeneric},
    {.name = "Outputs", .min = 1, .max = kMaxChannels, .def = 4, .type = kNT_typeGeneric},
};

// --- Factory Definition ---
static const _NT_factory factory =
    {
        .guid = NT_MULTICHAR('F', 'l', 'u', 'x'),
        .name = "Flux Matrix",
        .description = "A matrix mixer with shift/step transformations.",
        .numSpecifications = ARRAY_SIZE(specifications),
        .specifications = specifications,
        .calculateStaticRequirements = NULL,
        .initialise = NULL,
        .calculateRequirements = calculateRequirements,
        .construct = construct,
        .parameterChanged = NULL,
        .step = step,
        .draw = fluxDraw,                     // Keep existing draw function
        .midiRealtime = NULL,
        .midiMessage = NULL,
        .tags = kNT_tagEffect | kNT_tagUtility, // Add appropriate tags
        .hasCustomUi = fluxHasCustomUi,         // Add pointer to hasCustomUi
        .customUi = fluxCustomUi,               // Add pointer to customUi
        .setupUi = fluxSetupUi,                 // Add pointer to setupUi
};

// --- Plugin Entry Point ---
extern "C"
{
    uintptr_t pluginEntry(_NT_selector selector, uint32_t data)
    {
        switch (selector)
        {
        case kNT_selector_version:
            return kNT_apiVersionCurrent;
        case kNT_selector_numFactories:
            return 1;
        case kNT_selector_factoryInfo:
            return (uintptr_t)((data == 0) ? &factory : NULL);
        }
        return 0;
    }
} // extern "C"
