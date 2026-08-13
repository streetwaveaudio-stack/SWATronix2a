#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

enum class PluginTheme
{
    Photorealistic = 0,
    ModernMinimal,
    NeonPurple,
    AnimatedGradient
};

class SWALookAndFeel : public juce::LookAndFeel_V4
{
public:
    SWALookAndFeel();
    void setTheme(PluginTheme t) { theme = t; }
    PluginTheme getTheme() const { return theme; }

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    PluginTheme theme = PluginTheme::Photorealistic;
    juce::Colour getBaseColour() const;
    juce::Colour getAccentColour() const;
};
