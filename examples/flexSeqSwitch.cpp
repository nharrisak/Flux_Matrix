/*
 * Sequential Switch (Flex) - disting EX Algorithm
 * Author: Thorinside / Idea by Jodok31283 / Code by AI
 */

#include <distingnt/api.h>
#include <new>     // for placement new
#include <cstddef> // for size_t (though not strictly needed here)

// --- Use constants for specification indices ---
enum SpecIndex
{
    SPEC_CHANNELS = 0,
    SPEC_INPUTS = 1,
    SPEC_OUTPUTS = 2,
    NUM_SPECS // Keep track of count
};

// --- Use constants for hardware limits ---
constexpr int MAX_INPUT_GROUPS = 12;
constexpr int MAX_OUTPUT_GROUPS = 8;
constexpr int MAX_BUSSES = 28; // Max selectable bus number

// --- Constants ---

// Use constants for array sizes
constexpr size_t MAX_PARAMS_POSSIBLE = (2 /*Sig L+R*/ + 2 /*Clk+Rst*/ + MAX_OUTPUT_GROUPS * 4 /*OutL+ModeL+OutR+ModeR*/);
static_assert(MAX_PARAMS_POSSIBLE == 36, "Max params calculation mismatch");

// Maximum possible parameters: Base (36) + AntiClick (1)
constexpr size_t MAX_PARAMS_ACTUAL = MAX_PARAMS_POSSIBLE + 1;
static_assert(MAX_PARAMS_ACTUAL <= 40, "Total parameters exceed reasonable limit"); // Example check

// --- Parameter Indices Enum ---
enum ParamIndex
{
    // Indices will be assigned dynamically in construct based on mode
    // Placeholders for clarity in step function logic
    PARAM_IDX_INVALID = -1,

    // Demux Input Signals (Indices 0, [1])
    PARAM_IDX_DEMUX_SIG_L_IN = 0,
    PARAM_IDX_DEMUX_SIG_R_IN = 1,

    // Mux Output Signals (Indices vary)
    PARAM_IDX_MUX_SIG_L_OUT,
    PARAM_IDX_MUX_SIG_L_MODE,
    PARAM_IDX_MUX_SIG_R_OUT,
    PARAM_IDX_MUX_SIG_R_MODE,

    // Shared Clock/Reset (Indices vary)
    PARAM_IDX_CLK_IN,
    PARAM_IDX_RST_IN,

    // Anti-Click Toggle (Last parameter)
    PARAM_IDX_ANTI_CLICK
};

/* ───── specifications ───── */
// Add const
static const _NT_specification gSpecs[] = {
    {"Channels", 1, 2, 1, kNT_typeGeneric},               // SPEC_CHANNELS
    {"Inputs", 1, MAX_INPUT_GROUPS, 1, kNT_typeGeneric},  // SPEC_INPUTS
    {"Outputs", 1, MAX_OUTPUT_GROUPS, 2, kNT_typeGeneric} // SPEC_OUTPUTS
};
// Check if NUM_SPECS matches gSpecs size
static_assert(NUM_SPECS == sizeof(gSpecs) / sizeof(gSpecs[0]), "SpecIndex enum size mismatch");

/* short fixed-name tables */
// Add const
static const char *const monoIn[MAX_INPUT_GROUPS] = { // Use constant size
    "Input1", "Input2", "Input3", "Input4", "Input5", "Input6",
    "Input7", "Input8", "Input9", "Input10", "Input11", "Input12"};

// Add const
static const char *const monoOut[MAX_OUTPUT_GROUPS] = { // Use constant size
    "Output1", "Output2", "Output3", "Output4",
    "Output5", "Output6", "Output7", "Output8"};

// Add const
static const char *const sterInL[MAX_INPUT_GROUPS] = { // Use constant size
    "Input1 L", "Input2 L", "Input3 L", "Input4 L", "Input5 L", "Input6 L",
    "Input7 L", "Input8 L", "Input9 L", "Input10 L", "Input11 L", "Input12 L"};
// Add const
static const char *const sterInR[MAX_INPUT_GROUPS] = { // Use constant size
    "Input1 R", "Input2 R", "Input3 R", "Input4 R", "Input5 R", "Input6 R",
    "Input7 R", "Input8 R", "Input9 R", "Input10 R", "Input11 R", "Input12 R"};

// Add const
static const char *const sterOutL[MAX_OUTPUT_GROUPS] = { // Use constant size
    "Output1 L", "Output2 L", "Output3 L", "Output4 L",
    "Output5 L", "Output6 L", "Output7 L", "Output8 L"};
// Add const
static const char *const sterOutR[MAX_OUTPUT_GROUPS] = { // Use constant size
    "Output1 R", "Output2 R", "Output3 R", "Output4 R",
    "Output5 R", "Output6 R", "Output7 R", "Output8 R"}; // Fixed typo

/* helper to fill a parameter */
// Add const char* n
static inline void setP(_NT_parameter &p, const char *n, int16_t min, int16_t max,
                        int16_t def, uint8_t unit)
{
    p.name = n;
    p.min = min;
    p.max = max;
    p.def = def;
    p.unit = unit;
    p.scaling = 0;
    p.enumStrings = nullptr; // Explicit nullptr
}

/* ───── instance ───── */
struct Switch : _NT_algorithm
{
    // --- Configuration ---
    uint8_t lanes, gIn, gOut;
    bool demux;

    // --- Runtime State ---
    uint8_t idx; // Current active index
    bool clkHi, rstHi;

    // --- Fade State ---
    bool is_fading_out = false;
    bool is_fading_in = false;
    uint8_t pending_idx; // Index switching TO
    uint32_t fade_samples_remaining;
    uint32_t current_fade_samples; // Set by parameter, default 0

    // --- Parameter Storage ---
    uint8_t numParamsActual;
    _NT_parameter params[MAX_PARAMS_ACTUAL];     // Increased size
    uint8_t pageIdxRouting[MAX_PARAMS_POSSIBLE]; // Max possible routing params
    uint8_t pageIdxAudio[1];                     // For Anti-Click param
    _NT_parameterPage pageDefs[2];               // Routing Page + Audio Page
    _NT_parameterPages pages;

    // --- Parameter Index Mapping (set in construct) ---
    // These store the ACTUAL calculated index for key parameters based on config
    int paramIdxClk = PARAM_IDX_INVALID;
    int paramIdxRst = PARAM_IDX_INVALID;
    int paramIdxMuxOutL = PARAM_IDX_INVALID;
    int paramIdxMuxOutR = PARAM_IDX_INVALID;       // Only valid if lanes=2
    int paramIdxFadeSamples = PARAM_IDX_INVALID;   // Renamed from paramIdxAntiClick
    int firstGroupInputParam = PARAM_IDX_INVALID;  // Mux Mode: Index of "Input 1 L"
    int firstGroupOutputParam = PARAM_IDX_INVALID; // Demux Mode: Index of "Output 1 L"

    // Constructor can initialize members if desired (though construct() does it)
    Switch() : lanes(0), gIn(0), gOut(0), demux(false),
               idx(0), clkHi(false), rstHi(false),
               is_fading_out(false), is_fading_in(false), pending_idx(0),
               fade_samples_remaining(0), current_fade_samples(0),
               numParamsActual(0), pageDefs{}, pages{} {}
};

/* helpers */
// Use const float* b, const int N? No, buffer is modified by algorithm eventually.
// Use size_t N? API uses int. Stick to int.
static inline float *bus(float *b, int busIdx, int N)
{
    // Use MAX_BUSSES? API uses 28 directly in param definitions.
    // Keep check simple as before.
    return (busIdx > 0) ? (b + (busIdx - 1) * N) : nullptr;
}
// Use const float v
static inline bool rise(const float v, bool &state)
{
    const bool high = v > 1.f;
    const bool rising_edge = high && !state;
    state = high;
    return rising_edge;
}

/* ───── requirements ───── */
// Use const for sp pointer
static void calcReq(_NT_algorithmRequirements &r, const int32_t *const sp)
{
    // Use enum for spec indices
    const int lanes = sp[SPEC_CHANNELS];
    const int gIn = sp[SPEC_INPUTS];
    const int gOut = sp[SPEC_OUTPUTS];
    const bool demux = (gIn == 1);
    int numParams = 0; // Calculate locally

    if (demux)
    { // 1 -> N
        if (lanes == 1)
        {
            numParams = 1 /*Sig*/ + 2 /*Clk+Rst*/ + gOut * 2 /*Out+Mode*/;
        }
        else
        { // lanes == 2
            numParams = 2 /*Sig L+R*/ + 2 /*Clk+Rst*/ + gOut * 4 /*OutL+ModeL+OutR+ModeR*/;
        }
    }
    else
    { // N -> 1
        if (lanes == 1)
        {
            numParams = gIn * 1 /*In*/ + 2 /*Clk+Rst*/ + 2 /*SigOut+Mode*/;
        }
        else
        { // lanes == 2
            numParams = gIn * 2 /*InL+InR*/ + 2 /*Clk+Rst*/ + 4 /*SigOutL+ModeL+SigOutR+ModeR*/;
        }
    }

    // Add the Anti-Click parameter
    numParams += 1;

    // Validation using constants
    const bool invalid_config = (lanes * gIn > MAX_INPUT_GROUPS) ||
                                (lanes * gOut > MAX_OUTPUT_GROUPS) ||
                                (gIn == 1) == (gOut == 1); // XOR implies gIn=1 AND gOut=1, or gIn!=1 AND gOut!=1

    // Report total calculated parameters
    r.numParameters = invalid_config ? 0 : numParams; // Let's signal invalidity with 0 params

    // Ensure it doesn't exceed our adjusted max
    if (r.numParameters > MAX_PARAMS_ACTUAL)
    {
        r.numParameters = MAX_PARAMS_ACTUAL; // Safety clamp
    }

    r.sram = sizeof(Switch);
    r.dram = 0;
    r.dtc = 0;
    r.itc = 0;
}

// Forward declaration for parameterChanged
static void parameterChanged(_NT_algorithm *self_base, int p);

/* ───── constructor ───── */
// Use const for pointers where possible
static _NT_algorithm *construct(const _NT_algorithmMemoryPtrs &m,
                                const _NT_algorithmRequirements &r,
                                const int32_t *const sp)
{
    // Use enum for spec indices
    const uint8_t lanes = sp[SPEC_CHANNELS];
    const uint8_t gIn = sp[SPEC_INPUTS];
    const uint8_t gOut = sp[SPEC_OUTPUTS];

    // Validate config again (redundant with calcReq if it sets numParams=0, but safe)
    if ((lanes * gIn > MAX_INPUT_GROUPS) || (lanes * gOut > MAX_OUTPUT_GROUPS) || (gIn == 1) == (gOut == 1) || r.numParameters == 0)
    {
        return nullptr; // Invalid configuration
    }

    Switch *self = new (m.sram) Switch(); // Use constructor init + placement new

    self->lanes = lanes;
    self->gIn = gIn;
    self->gOut = gOut;
    self->demux = (gIn == 1);
    self->idx = 0;
    self->clkHi = false;
    self->rstHi = false;
    // self->numParamsActual will be set below

    int p = 0; // Parameter index counter

    /* Inputs */
    if (self->demux)
    { // 1 -> N : Define Sig L/[R] inputs
        // PARAM_IDX_DEMUX_SIG_L_IN is implicitly 0
        setP(self->params[p++], "Sig Left Input", 1, MAX_BUSSES, 1, kNT_unitAudioInput);
        if (self->lanes == 2)
        {
            // PARAM_IDX_DEMUX_SIG_R_IN is implicitly 1
            setP(self->params[p++], "Sig Right Input", 0, MAX_BUSSES, 0, kNT_unitAudioInput); // Allow 'None' for Right? Or default to 2? Let's keep 0.
        }
    }
    else
    {                                   // N -> 1 : Define group inputs
        self->firstGroupInputParam = p; // Store index of "Input 1 L"
        for (int g = 0; g < self->gIn; ++g)
        {
            setP(self->params[p++], self->lanes == 1 ? monoIn[g] : sterInL[g], 0, MAX_BUSSES, 0, kNT_unitAudioInput); // Allow 'None', default 0
            if (self->lanes == 2)
            {
                setP(self->params[p++], sterInR[g], 0, MAX_BUSSES, 0, kNT_unitAudioInput); // Allow 'None', default 0
            }
        }
    }

    /* Clock / Reset */
    self->paramIdxClk = p;                                               // Store Clock index
    setP(self->params[p++], "Clock", 0, MAX_BUSSES, 0, kNT_unitCvInput); // Allow 'None', default 0
    self->paramIdxRst = p;                                               // Store Reset index
    setP(self->params[p++], "Reset", 0, MAX_BUSSES, 0, kNT_unitCvInput); // Allow 'None', default 0

    /* Outputs */
    if (self->demux)
    {                                    // 1 -> N : Define group outputs
        self->firstGroupOutputParam = p; // Store index of "Output 1 L"
        for (int g = 0; g < self->gOut; ++g)
        {
            setP(self->params[p++], self->lanes == 1 ? monoOut[g] : sterOutL[g], 1, MAX_BUSSES, 1, kNT_unitAudioOutput); // Force selection, default 1
            setP(self->params[p++], "mode", 0, 1, 0, kNT_unitOutputMode);                                                // Mode L
            if (self->lanes == 2)
            {
                // Check index calculation again for sterOutR - fixed typo earlier
                setP(self->params[p++], sterOutR[g], 1, MAX_BUSSES, 1, kNT_unitAudioOutput); // Force selection, default 1
                setP(self->params[p++], "mode", 0, 1, 0, kNT_unitOutputMode);                // Mode R
            }
        }
    }
    else
    {                                                                                      // N -> 1 : Define Sig L/[R] outputs
        self->paramIdxMuxOutL = p;                                                         // Store Sig L Out index
        setP(self->params[p++], "Sig Left Output", 1, MAX_BUSSES, 1, kNT_unitAudioOutput); // Force selection, default 1
        setP(self->params[p++], "Sig Left mode", 0, 1, 0, kNT_unitOutputMode);
        if (self->lanes == 2)
        {
            self->paramIdxMuxOutR = p;                                                          // Store Sig R Out index
            setP(self->params[p++], "Sig Right Output", 1, MAX_BUSSES, 1, kNT_unitAudioOutput); // Force selection, default 1
            setP(self->params[p++], "Sig Right mode", 0, 1, 0, kNT_unitOutputMode);
        }
    }

    const int numRoutingParams = p; // Number of parameters defined so far

    // --- Define Audio Parameters ---
    self->paramIdxFadeSamples = p;           // Store index of this param (Renamed)
    const uint16_t default_fade_samples = 0; // Reset default to 0 = OFF
    const uint16_t max_fade_samples = 512;   // Reset max fade duration (~10.7ms @ 48k)
    setP(self->params[p++], "Fade Samples", 0, max_fade_samples, default_fade_samples, kNT_unitFrames);
    // No enum strings needed

    // --- Final Setup ---
    self->numParamsActual = p;

    // --- Parameter Pages ---
    // Routing Page
    int routingPageParamCount = 0;
    for (int i = 0; i < numRoutingParams; ++i)
    {
        self->pageIdxRouting[routingPageParamCount++] = i;
    }
    self->pageDefs[0] = {"Routing", (uint8_t)routingPageParamCount, self->pageIdxRouting};

    // Audio Page
    self->pageIdxAudio[0] = self->paramIdxFadeSamples; // Use renamed variable
    self->pageDefs[1] = {"Audio", 1, self->pageIdxAudio};

    // Assign pages struct
    self->pages = {2, self->pageDefs}; // 2 pages now

    self->parameters = self->params;     // Point to our params array
    self->parameterPages = &self->pages; // Point to our pages struct

    // Initialize fade state
    self->fade_samples_remaining = 0;
    self->current_fade_samples = default_fade_samples; // Initialize based on default
    self->is_fading_out = false;
    self->is_fading_in = false;
    self->pending_idx = 0;

    // NOTE: Relying on host to call parameterChanged after construct if needed for presets/defaults.
    return self;
}

// Implement parameterChanged to update fade duration
static void parameterChanged(_NT_algorithm *self_base, int p)
{
    Switch *mutable_self = static_cast<Switch *>(self_base); // Need non-const self
    const Switch *self = mutable_self;                       // Const self for reading v[]

    if (p == self->paramIdxFadeSamples)
    {                                                    // Check if "Fade Samples" param changed (Use renamed variable)
        mutable_self->current_fade_samples = self->v[p]; // Update stored fade duration
    }
}

/* ---- DSP: copy audio/CV with Add / Replace ---- */
// Structure to hold side lane info
struct Lane
{
    float *ptr = nullptr; // Initialize to nullptr
    uint8_t replace = 1;  // Default to replace (doesn't matter for input mux)
};

// Use const int nBy4
static void step(_NT_algorithm *b, float *buf, const int nBy4)
{
    // Casts (mutable needed for state updates)
    Switch *mutable_self = static_cast<Switch *>(b);
    const Switch *self = mutable_self;
    const int N = nBy4 * 4; // Number of frames

    // --- Get parameters (fade duration stored in struct) ---
    // const bool antiClickEnabled = self->v[self->paramIdxFadeSamples]; // UNUSED

    // --- Get Clock/Reset pointers ---
    const int busIdxClk = self->v[self->paramIdxClk];
    const int busIdxRst = self->v[self->paramIdxRst];
    float *clk = bus(buf, busIdxClk, N);
    float *rst = bus(buf, busIdxRst, N);

    // --- Get Main Output pointers (Mux mode only) ---
    float *outL = nullptr;
    float *outR = nullptr;
    if (!self->demux)
    {
        outL = bus(buf, self->v[self->paramIdxMuxOutL], N);
        if (self->lanes == 2)
        {
            outR = bus(buf, self->v[self->paramIdxMuxOutR], N);
        }
    }
    // --- Get Main Input pointers (Demux mode only) ---
    float *inL = nullptr;
    float *inR = nullptr;
    if (self->demux)
    {
        inL = bus(buf, self->v[PARAM_IDX_DEMUX_SIG_L_IN], N); // Param 0
        if (self->lanes == 2)
        {
            inR = bus(buf, self->v[PARAM_IDX_DEMUX_SIG_R_IN], N); // Param 1
        }
    }

    /* ------------------------------------------------------------------ */
    /* Build array of side lanes (pointers only needed now)             */
    /* ------------------------------------------------------------------ */
    const int groups = self->demux ? self->gOut : self->gIn;
    float *side_ptr[MAX_INPUT_GROUPS][2] = {}; // Pointers only

    // Calculate starting parameter index for side lanes
    int paramIdxSideStart = self->demux ? self->firstGroupOutputParam
                                        : self->firstGroupInputParam;
    int currentParamIdx = paramIdxSideStart;

    for (int g = 0; g < groups; ++g)
    {
        const int busIdxL = self->v[currentParamIdx];
        side_ptr[g][0] = bus(buf, busIdxL, N);
        currentParamIdx += self->demux ? 2 : 1; // Skip mode param if demux

        if (self->lanes == 2)
        {
            const int busIdxR = self->v[currentParamIdx];
            side_ptr[g][1] = bus(buf, busIdxR, N);
            currentParamIdx += self->demux ? 2 : 1; // Skip mode param if demux
        }
    }

    /* ------------------------------------------------------------------ */
    /* Audio-rate loop                                                    */
    /* ------------------------------------------------------------------ */
    for (int n = 0; n < N; ++n)
    {
        /* --- Clock & Reset Logic --- */
        if (clk && rise(clk[n], mutable_self->clkHi))
        {
            // Clock edge: Trigger a switch if currently idle
            if (!mutable_self->is_fading_out && !mutable_self->is_fading_in)
            { // If idle
                if (self->current_fade_samples > 0 && !self->demux)
                {
                    // Start fade OUT (don't switch idx yet)
                    mutable_self->is_fading_out = true;
                    mutable_self->fade_samples_remaining = self->current_fade_samples;
                }
                else
                {
                    // Instant switch (fade=0 or Demux mode)
                    mutable_self->idx = (mutable_self->idx + 1) % groups;
                }
            }
            // Else: Ignore clock edges during fade (implicit)
        }
        if (rst && rise(rst[n], mutable_self->rstHi))
        {
            // Reset edge: Always reset instantly, cancel fade
            mutable_self->idx = 0;
            mutable_self->is_fading_out = false;
            mutable_self->is_fading_in = false;
        }

        // --- Determine gain and index based on state ---
        float current_gain = 1.0f;
        uint8_t index_to_use = self->idx;

        if (mutable_self->is_fading_out)
        {
            if (mutable_self->fade_samples_remaining > 0)
            {
                mutable_self->fade_samples_remaining--;
                // Calculate fade out gain (1 -> 0)
                current_gain = (self->current_fade_samples > 0)
                                   ? ((float)mutable_self->fade_samples_remaining / (float)self->current_fade_samples)
                                   : 0.0f;
                index_to_use = self->idx; // Still using the old index
            }
            else
            {
                // Fade out finished: Switch index, start fade in
                mutable_self->idx = (mutable_self->idx + 1) % groups; // Switch index now
                mutable_self->is_fading_out = false;
                mutable_self->is_fading_in = true;
                mutable_self->fade_samples_remaining = self->current_fade_samples; // Reset counter
                index_to_use = self->idx;                                          // Use the NEW index now
                // Calculate gain for the FIRST sample of fade-in
                current_gain = (self->current_fade_samples > 0)
                                   ? (1.0f - (float)mutable_self->fade_samples_remaining / (float)self->current_fade_samples)
                                   : 1.0f;
                // Decrement counter for this first sample of fade-in
                if (mutable_self->fade_samples_remaining > 0)
                    mutable_self->fade_samples_remaining--;
            }
        }
        else if (mutable_self->is_fading_in)
        {
            if (mutable_self->fade_samples_remaining > 0)
            {
                mutable_self->fade_samples_remaining--;
                // Calculate fade in gain (0 -> 1) - Linear
                current_gain = (self->current_fade_samples > 0)
                                   ? (1.0f - (float)mutable_self->fade_samples_remaining / (float)self->current_fade_samples)
                                   : 1.0f;
                index_to_use = self->idx; // Using the new index
            }
            else
            {
                // Fade in finished
                mutable_self->is_fading_in = false;
                current_gain = 1.0f;
                index_to_use = self->idx;
            }
        }
        else
        { // Idle state
            current_gain = 1.0f;
            index_to_use = self->idx;
        }

        /* --- Apply Switching / Fading --- */
        if (self->demux) /* 1 → N (Demux) - STILL NO FADE */
        {
            // NOTE: This uses self->idx directly, not index_to_use, so it remains instant switch
            const int modeLParamIdx = self->firstGroupOutputParam + self->idx * (self->lanes == 1 ? 2 : 4) + 1;
            const bool replaceL = self->v[modeLParamIdx];
            float *destL = side_ptr[self->idx][0]; // Uses self->idx

            if (destL && inL)
            {
                destL[n] = replaceL ? inL[n] : destL[n] + inL[n];
            }

            if (self->lanes == 2)
            {
                const int modeRParamIdx = self->firstGroupOutputParam + self->idx * 4 + 3;
                const bool replaceR = self->v[modeRParamIdx];
                float *destR = side_ptr[self->idx][1]; // Uses self->idx
                if (destR && inR)
                {
                    destR[n] = replaceR ? inR[n] : destR[n] + inR[n];
                }
            }
        }
        else /* N → 1 (Mux) - APPLY FADE OUT/IN */
        {
            // Get input signal for the index being used in the current state
            float input_L = side_ptr[index_to_use][0] ? side_ptr[index_to_use][0][n] : 0.f;

            // Calculate output L
            if (outL)
            {
                outL[n] = input_L * current_gain;
            }

            if (self->lanes == 2)
            {
                float input_R = side_ptr[index_to_use][1] ? side_ptr[index_to_use][1][n] : 0.f;
                // Calculate output R
                if (outR)
                {
                    outR[n] = input_R * current_gain;
                }
            }
        }
    } // End audio sample loop
}

/* ───── factory & entry ───── */
// Make factory const
static const _NT_factory gFactory = {
    NT_MULTICHAR('S', 'S', 'W', '1'), "Seq Switch Flex",
    "Seq Switch (Mux/Demux, Clk, Rst). Mux: N->1 (Sig Out). Demux: 1->N (Sig In).", // Updated description
    sizeof(gSpecs) / sizeof(gSpecs[0]), gSpecs,
    nullptr, // No static requirements
    nullptr, // No static init
    calcReq,
    construct,
    parameterChanged,
    step,
    nullptr, // No draw function
    nullptr, // No midi realtime
    nullptr, // No midi message
    kNT_tagUtility};

extern "C" uintptr_t pluginEntry(_NT_selector s, uint32_t i)
{
    switch (s)
    {
    case kNT_selector_version:
        return kNT_apiVersionCurrent;
    case kNT_selector_numFactories:
        return 1;
    case kNT_selector_factoryInfo:
        return (i == 0) ? reinterpret_cast<uintptr_t>(&gFactory) : 0;
    default:
        return 0;
    }
}
