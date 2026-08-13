#include "Themes.h"
#include <cmath>

SWALookAndFeel::SWALookAndFeel() {}

juce::Colour SWALookAndFeel::getBaseColour() const
{
    switch (theme)
    {
        case PluginTheme::Photorealistic: return juce::Colour(0xff1a1a1a);
        case PluginTheme::ModernMinimal: return juce::Colour(0xfff5f5f5);
        case PluginTheme::NeonPurple: return juce::Colour(0xff2a0a3a);
        case PluginTheme::AnimatedGradient: return juce::Colour(0xff1a0a2e);
    }
    return juce::Colours::black;
}

juce::Colour SWALookAndFeel::getAccentColour() const
{
    switch (theme)
    {
        case PluginTheme::Photorealistic: return juce::Colour(0xffc8a96e);
        case PluginTheme::ModernMinimal: return juce::Colour(0xff2196F3);
        case PluginTheme::NeonPurple: return juce::Colour(0xffb44aff);
        case PluginTheme::AnimatedGradient: return juce::Colour(0xffff6ec7);
    }
    return juce::Colours::white;
}

void SWALookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                      float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                      juce::Slider&)
{
    const float radius = juce::jmin(width, height) * 0.5f - 4.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    if (theme == PluginTheme::Photorealistic)
    {
        juce::ColourGradient ringGrad(juce::Colour(0xff555555), centreX - radius, centreY - radius,
                                      juce::Colour(0xffcccccc), centreX + radius, centreY + radius, false);
        g.setGradientFill(ringGrad);
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2);
        g.setColour(juce::Colour(0xff222222));
        g.drawEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2, 2.0f);

        juce::ColourGradient knobGrad(juce::Colour(0xff333333), centreX - radius * 0.8f, centreY - radius * 0.8f,
                                      juce::Colour(0xff999999), centreX + radius * 0.8f, centreY + radius * 0.8f, false);
        g.setGradientFill(knobGrad);
        g.fillEllipse(centreX - radius * 0.8f, centreY - radius * 0.8f, radius * 1.6f, radius * 1.6f);

        g.setColour(juce::Colours::black);
        g.drawLine(centreX, centreY,
                   centreX + radius * 0.7f * std::sin(angle),
                   centreY - radius * 0.7f * std::cos(angle), 2.0f);
    }
    else if (theme == PluginTheme::ModernMinimal)
    {
        g.setColour(juce::Colour(0xffdddddd));
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2);
        g.setColour(juce::Colour(0xff2196F3));
        g.drawEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2, 3.0f);
        g.setColour(juce::Colours::black);
        g.drawLine(centreX, centreY,
                   centreX + radius * 0.7f * std::sin(angle),
                   centreY - radius * 0.7f * std::cos(angle), 2.0f);
    }
    else if (theme == PluginTheme::NeonPurple || theme == PluginTheme::AnimatedGradient)
    {
        juce::DropShadow shadow(juce::Colour(0xffb44aff), 10, {0, 0});
        shadow.drawForEllipse(g, juce::Rectangle<int>(x, y, width, height));
        g.setColour(juce::Colour(0xff3a1a4a));
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2);
        g.setColour(juce::Colour(0xffb44aff));
        g.drawEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2, 2.0f);
        g.setColour(juce::Colours::white);
        g.drawLine(centreX, centreY,
                   centreX + radius * 0.7f * std::sin(angle),
                   centreY - radius * 0.7f * std::cos(angle), 2.0f);
    }
}

void SWALookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                      bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();

    if (theme == PluginTheme::Photorealistic)
    {
        g.setColour(juce::Colour(0xff333333));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(button.getToggleState() ? juce::Colour(0xffc8a96e) : juce::Colour(0xff777777));
        g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
    }
    else if (theme == PluginTheme::ModernMinimal)
    {
        g.setColour(button.getToggleState() ? juce::Colour(0xff2196F3) : juce::Colour(0xffdddddd));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }
    else
    {
        juce::DropShadow shadow(button.getToggleState() ? juce::Colour(0xffb44aff) : juce::Colour(0xff444444), 6, {0,0});
        shadow.drawForRectangle(g, bounds.toNearestInt());
        g.setColour(button.getToggleState() ? juce::Colour(0xffb44aff) : juce::Colour(0xff3a1a4a));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
    }

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText(button.getButtonText(), bounds.toNearestInt(), juce::Justification::centred, 1);
}

void SWALookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                  int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();

    if (theme == PluginTheme::ModernMinimal)
    {
        g.setColour(juce::Colour(0xffe0e0e0));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colour(0xff2196F3));
        g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
    }
    else
    {
        g.setColour(getBaseColour());
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(getAccentColour());
        g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
    }

    // Flecha
    int arrowX = width - 12;
    int arrowY = height / 2;
    g.setColour(getAccentColour());
    g.drawLine(arrowX, arrowY - 2, arrowX + 4, arrowY + 2, 2.0f);
    g.drawLine(arrowX, arrowY + 2, arrowX + 4, arrowY - 2, 2.0f);

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText(box.getText(), 4, 0, width - 16, height, juce::Justification::centredLeft, 1);
}

void SWALookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();

    if (theme == PluginTheme::ModernMinimal)
    {
        g.setColour(button.isDown() ? juce::Colour(0xffbbbbbb) : juce::Colour(0xffdddddd));
        g.fillRoundedRectangle(bounds, 6.0f);
    }
    else
    {
        g.setColour(button.isDown() ? getAccentColour().darker() : getAccentColour());
        g.fillRoundedRectangle(bounds, 6.0f);
    }
}

void SWALookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
}
