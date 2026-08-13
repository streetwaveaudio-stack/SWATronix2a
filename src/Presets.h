#pragma once

#include <juce_core/juce_core.h>
#include <vector>

struct Preset
{
    juce::String name;
    float peakReduction;   // 0.0 – 1.0
    float outputGain;      // 0.0 – 1.0
    bool mode;             // false = Compress, true = Limit
    float emphasis;        // 0.0 – 1.0
};

extern const std::vector<Preset> presets;
