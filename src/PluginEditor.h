#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Themes.h"

class VUMeterComponent : public juce::Component
{
public:
    VUMeterComponent();
    void setGainReduction(float db) { gainReduction = db; repaint(); }
    void setOutputLevel(float db) { outputLevel = db; }
    void paint(juce::Graphics&) override;

private:
    float gainReduction = 0.0f;
    float outputLevel = -60.0f;
};

class SWATronix2AAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          public juce::Timer
{
public:
    SWATronix2AAudioProcessorEditor(SWATronix2AAudioProcessor&);
    ~SWATronix2AAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SWATronix2AAudioProcessor& processorRef;
    SWALookAndFeel lookAndFeel;

    juce::Slider peakReductionSlider, outputGainSlider, emphasisSlider;
    juce::ToggleButton modeButton;
    juce::ComboBox presetCombo, themeCombo;
    juce::TextButton infoButton;
    juce::Label peakReductionLabel, outputGainLabel, emphasisLabel, modeLabel, presetLabel, themeLabel;
    VUMeterComponent vuMeter;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakReductionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> emphasisAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeAttachment;

    void setupComponents();
    void updateTheme();
    void applyPresetFromCombo();
    void showInfo();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SWATronix2AAudioProcessorEditor)
};
