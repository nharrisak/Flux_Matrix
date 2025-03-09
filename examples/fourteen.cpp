#include <math.h>
#include <new>
#include <distingnt/api.h>

// Algorithm state
struct _midiCcConverterAlgorithm : public _NT_algorithm
{
    _midiCcConverterAlgorithm()
    {
        msb_value = 0;
        lsb_value = 0;
        msb_received = false;
        lsb_received = false;
        current_voltage = 0.0f;
    }

    // MIDI state
    int msb_value;     // Most significant byte value (0-127)
    int lsb_value;     // Least significant byte value (0-127)
    bool msb_received; // Flag to track if we've received an MSB message
    bool lsb_received; // Flag to track if we've received an LSB message

    // Output state
    float current_voltage; // Current output voltage
};

// Parameter indices
enum
{
    kParamOutput,      // CV output
    kParamOutputMode,  // CV output mode (add/replace)
    kParamMsbCC,       // MSB CC number
    kParamLsbCC,       // LSB CC number
    kParamMidiChannel, // MIDI channel (0 = all, 1-16 = specific)
    kParamBipolar,     // Bipolar mode (on/off)
    kParamSmoothing,   // Smoothing amount
};

// Parameter definitions
_NT_parameter parameters[] = {
    NT_PARAMETER_CV_OUTPUT_WITH_MODE("CV Output", 1, 13){.name = "MSB CC", .min = 0, .max = 127, .def = 1, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL},
    {.name = "LSB CC", .min = 0, .max = 127, .def = 33, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL},
    {.name = "MIDI Ch", .min = 0, .max = 16, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL},
    {.name = "Bipolar", .min = 0, .max = 1, .def = 0, .unit = kNT_unitNone, .scaling = 0, .enumStrings = NULL},
    {.name = "Smoothing", .min = 0, .max = 100, .def = 0, .unit = kNT_unitPercent, .scaling = 0, .enumStrings = NULL},
};

// Parameter pages
uint8_t page1[] = {kParamMsbCC, kParamLsbCC, kParamMidiChannel};
uint8_t page2[] = {kParamBipolar, kParamSmoothing};
uint8_t page3[] = {kParamOutput, kParamOutputMode};

_NT_parameterPage pages[] = {
    {.name = "Setup", .numParams = ARRAY_SIZE(page1), .params = page1},
    {.name = "CV", .numParams = ARRAY_SIZE(page2), .params = page2},
    {.name = "Routing", .numParams = ARRAY_SIZE(page3), .params = page3},
};

_NT_parameterPages parameterPages = {
    .numPages = ARRAY_SIZE(pages),
    .pages = pages,
};

// Calculate memory requirements for the algorithm
void calculateRequirements(_NT_algorithmRequirements &req)
{
    req.numParameters = ARRAY_SIZE(parameters);
    req.sram = sizeof(_midiCcConverterAlgorithm);
    req.dram = 0;
    req.dtc = 0;
    req.itc = 0;
}

// Construct the algorithm
_NT_algorithm *construct(const _NT_algorithmMemoryPtrs &ptrs, const _NT_algorithmRequirements &req)
{
    _midiCcConverterAlgorithm *alg = new (ptrs.sram) _midiCcConverterAlgorithm();
    alg->parameters = parameters;
    alg->parameterPages = &parameterPages;
    return alg;
}

// Process incoming MIDI messages
void midiMessage(_NT_algorithm *self, uint8_t byte0, uint8_t byte1, uint8_t byte2)
{
    _midiCcConverterAlgorithm *pThis = (_midiCcConverterAlgorithm *)self;

    // Only process CC messages (0xB0-0xBF)
    if ((byte0 & 0xF0) == 0xB0)
    {
        // Extract MIDI channel (0-15)
        int channel = byte0 & 0x0F;

        // Get the MIDI channel parameter (0 = all channels, 1-16 = specific channel)
        int midiChannel = (int)pThis->v[kParamMidiChannel];

        // Skip if we're filtering by channel and this isn't our channel
        if (midiChannel > 0 && channel != (midiChannel - 1))
        {
            return;
        }

        // Extract CC number and value
        int cc_number = byte1 & 0x7F; // Ensure 7-bit value
        int cc_value = byte2 & 0x7F;  // Ensure 7-bit value

        // Get MSB and LSB CC numbers from parameters
        int msb_cc = (int)pThis->v[kParamMsbCC];
        int lsb_cc = (int)pThis->v[kParamLsbCC];

        // Check for reset controllers message (CC 121) or all notes off (CC 123)
        if (cc_number == 121 || (cc_number == 123 && cc_value == 0))
        {
            pThis->msb_value = 0;
            pThis->lsb_value = 0;
            pThis->msb_received = false;
            pThis->lsb_received = false;
            pThis->current_voltage = 0.0f;
            return;
        }

        // Check if this is our MSB or LSB CC number
        if (cc_number == msb_cc)
        {
            pThis->msb_value = cc_value;
            pThis->msb_received = true;
        }
        else if (cc_number == lsb_cc)
        {
            pThis->lsb_value = cc_value;
            pThis->lsb_received = true;
        }

        // Calculate the voltage if we have received any CC messages
        if (pThis->msb_received || pThis->lsb_received)
        {
            // Calculate 14-bit value (0-16383)
            int value_14bit = 0;

            if (pThis->msb_received && pThis->lsb_received)
            {
                // We have both MSB and LSB - ideal case
                value_14bit = (pThis->msb_value << 7) | pThis->lsb_value;
            }
            else if (pThis->msb_received)
            {
                // We only have MSB, scale it up
                value_14bit = pThis->msb_value << 7;
            }
            else
            {
                // We only have LSB, just use it at low resolution
                value_14bit = pThis->lsb_value;
            }

            // Normalize to 0.0-1.0 range
            float normalized = (float)value_14bit / 16383.0f;

            // Apply bipolar setting if enabled
            if (pThis->v[kParamBipolar] > 0.5f)
            {
                // -5V to +5V range
                pThis->current_voltage = (normalized * 10.0f) - 5.0f;
            }
            else
            {
                // 0V to +10V range
                pThis->current_voltage = normalized * 10.0f;
            }
        }
    }
}

// DSP processing function
void step(_NT_algorithm *self, float *busFrames, int numFramesBy4)
{
    _midiCcConverterAlgorithm *pThis = (_midiCcConverterAlgorithm *)self;
    int numFrames = numFramesBy4 * 4;

    // Get output destinations with bounds checking
    int cvOutputIdx = (int)pThis->v[kParamOutput] - 1;
    if (cvOutputIdx < 0 || cvOutputIdx >= 28)
    {
        return; // Invalid output
    }

    float *cvOutput = busFrames + cvOutputIdx * numFrames;
    bool cvReplace = pThis->v[kParamOutputMode] > 0.5f;

    // Get smoothing value (0-1)
    float smoothing = pThis->v[kParamSmoothing] / 100.0f;

    // Calculate simple smoothing coefficient
    float smooth_coef = 0.0f;
    if (smoothing > 0.0f)
    {
        // Convert to a simple exponential smoothing coefficient
        // Higher smoothing = slower response
        smooth_coef = 1.0f - (smoothing * 0.99f);
    }
    else
    {
        smooth_coef = 1.0f; // No smoothing
    }

    // Apply CV to output bus with optional smoothing
    float target_voltage = pThis->current_voltage;
    float current_voltage = pThis->current_voltage;

    for (int i = 0; i < numFrames; ++i)
    {
        // Apply simple smoothing if enabled
        if (smoothing > 0.0f)
        {
            current_voltage = current_voltage + (smooth_coef * (target_voltage - current_voltage));
        }
        else
        {
            current_voltage = target_voltage;
        }

        // Output to CV
        if (cvReplace)
        {
            cvOutput[i] = current_voltage;
        }
        else
        {
            cvOutput[i] += current_voltage;
        }
    }

    // Save the last voltage for next time (for smoothing)
    pThis->current_voltage = current_voltage;
}

// Factory information
_NT_factory factory = {
    .guid = NT_MULTICHAR('T', 'h', '1', '4'),
    .name = "14-bit CC to CV",
    .description = "Converts 14-bit MIDI CC messages to CV",
    .calculateRequirements = calculateRequirements,
    .construct = construct,
    .parameterChanged = NULL,
    .step = step,
    .draw = NULL,
    .midiMessage = midiMessage,
};

// Plugin entry point
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
