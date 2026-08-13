#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "La2aEmulation.h"
#include "Presets.h"

class SWATronix2AAudioProcessor : public juce::AudioProcessor
{
public:
    SWATronix2AAudioProcessor();
    ~SWATronix2AAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SWA Tronix 2A"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    La2aEmulation la2a;
    juce::Value themeValue;

    void applyPreset(const Preset& p);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SWATronix2AAudioProcessor)
};
