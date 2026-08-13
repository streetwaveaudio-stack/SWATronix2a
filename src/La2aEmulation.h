#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class La2aEmulation
{
public:
    La2aEmulation() = default;
    ~La2aEmulation() = default;

    void prepare(double newSampleRate, int samplesPerBlock);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, float peakReduction, float outputGain, int mode, float emphasis);

    float getGainReductionDb() const { return currentGainReductionDb; }
    float getOutputLevelDb() const { return currentOutputLevelDb; }

private:
    double sampleRate = 44100.0;
    double attackCoeff = 0.0;
    double releaseFastCoeff = 0.0;
    double releaseSlowCoeff = 0.0;
    double envelope = 0.0;
    double sidechainFilterL = 0.0;
    double sidechainFilterR = 0.0;
    double gainReductionDb = 0.0;
    double outputLevelDb = -60.0;
    float currentGainReductionDb = 0.0f;
    float currentOutputLevelDb = -60.0f;

    double getReleaseCoeff(double env) const;
    double applyEmphasis(double input, double& filterState, float emphasis, double dt);
};
