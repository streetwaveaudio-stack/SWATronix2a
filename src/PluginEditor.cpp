#include "PluginEditor.h"
#include "Presets.h"
#include <cmath>

VUMeterComponent::VUMeterComponent()
{
    setOpaque(true);
}

void VUMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(juce::Colour(0xff444444));
    g.drawRoundedRectangle(bounds, 8.0f, 2.0f);

    auto meterArea = bounds.reduced(10);
    float meterHeight = meterArea.getHeight();
    float zeroY = meterArea.getY() + meterHeight * 0.8f;
    float normGR = juce::jlimit(0.0f, 20.0f, gainReduction) / 20.0f;
    float needleY = meterArea.getY() + meterHeight * (0.8f - 0.6f * normGR);

    g.setColour(juce::Colours::red);
    g.drawLine(meterArea.getX(), needleY, meterArea.getRight(), needleY, 2.0f);

    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("Gain Reduction", meterArea.toNearestInt(), juce::Justification::centredTop, 1);
}

SWATronix2AAudioProcessorEditor::SWATronix2AAudioProcessorEditor(SWATronix2AAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&lookAndFeel);
    setResizable(true, true);
    setResizeLimits(600, 400, 1200, 800);
    setSize(900, 600);

    setupComponents();

    int themeIndex = (int)processorRef.themeValue.getValue();
    themeCombo.setSelectedItemIndex(themeIndex, juce::dontSendNotification);
    updateTheme();

    startTimerHz(30);
}

SWATronix2AAudioProcessorEditor::~SWATronix2AAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void SWATronix2AAudioProcessorEditor::setupComponents()
{
    // Sliders
    auto setupRotary = [](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::Rotary);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        s.setRange(0.0, 1.0, 0.001);
    };

    setupRotary(peakReductionSlider);
    setupRotary(outputGainSlider);
    setupRotary(emphasisSlider);

    addAndMakeVisible(peakReductionSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(emphasisSlider);

    // Toggle Button
    modeButton.setButtonText("Compress");
    addAndMakeVisible(modeButton);

    // ComboBoxes
    presetCombo.setTextWhenNothingSelected("Select Preset");
    for (const auto& p : presets)
        presetCombo.addItem(p.name, presets.size() == 1 ? 1 : presetCombo.getNumItems() + 1);
    addAndMakeVisible(presetCombo);

    themeCombo.addItem("Photorealistic", 1);
    themeCombo.addItem("Modern Minimal", 2);
    themeCombo.addItem("Neon Purple", 3);
    themeCombo.addItem("Animated Gradient", 4);
    addAndMakeVisible(themeCombo);

    infoButton.setButtonText("Info");
    addAndMakeVisible(infoButton);

    // Labels
    peakReductionLabel.setText("Peak Reduction", juce::dontSendNotification);
    outputGainLabel.setText("Output Gain", juce::dontSendNotification);
    emphasisLabel.setText("Emphasis", juce::dontSendNotification);
    modeLabel.setText("Mode", juce::dontSendNotification);
    presetLabel.setText("Presets", juce::dontSendNotification);
    themeLabel.setText("Theme", juce::dontSendNotification);

    for (auto* l : {&peakReductionLabel, &outputGainLabel, &emphasisLabel, &modeLabel, &presetLabel, &themeLabel})
    {
        addAndMakeVisible(l);
        l->setJustificationType(juce::Justification::centred);
    }

    // Attachments
    peakReductionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "peakReduction", peakReductionSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "outputGain", outputGainSlider);
    emphasisAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "emphasis", emphasisSlider);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "mode", modeButton);

    // Listeners
    modeButton.onClick = [this]
    {
        bool limit = processorRef.apvts.getRawParameterValue("mode")->load() > 0.5f;
        modeButton.setButtonText(limit ? "Limit" : "Compress");
    };

    presetCombo.onChange = [this] { applyPresetFromCombo(); };
    themeCombo.onChange = [this] { updateTheme(); };
    infoButton.onClick = [this] { showInfo(); };
}

void SWATronix2AAudioProcessorEditor::updateTheme()
{
    int idx = themeCombo.getSelectedItemIndex();
    PluginTheme t = static_cast<PluginTheme>(idx);
    lookAndFeel.setTheme(t);
    processorRef.themeValue.setValue(idx);
    repaint();
}

void SWATronix2AAudioProcessorEditor::applyPresetFromCombo()
{
    int idx = presetCombo.getSelectedItemIndex();
    if (idx >= 0 && idx < presets.size())
        processorRef.applyPreset(presets[idx]);
}

void SWATronix2AAudioProcessorEditor::showInfo()
{
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
        "About SWA Tronix 2A",
        "SWA Tronix 2A\nDeveloped by StreetWave Audio\n\n"
        "Emulación del legendario compresor óptico Teletronix LA-2A.\n"
        "Incluye temas visuales, presets y ventana redimensionable.");
}

void SWATronix2AAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    PluginTheme theme = lookAndFeel.getTheme();

    if (theme == PluginTheme::Photorealistic)
    {
        g.fillAll(juce::Colour(0xff1a1a1a));
        g.setColour(juce::Colour(0xff333333));
        g.drawRect(bounds, 2);
    }
    else if (theme == PluginTheme::ModernMinimal)
    {
        g.fillAll(juce::Colour(0xfff5f5f5));
    }
    else if (theme == PluginTheme::NeonPurple)
    {
        g.fillAll(juce::Colour(0xff2a0a3a));
        juce::DropShadow shadow(juce::Colour(0xffb44aff), 20, {0,0});
        shadow.drawForRectangle(g, bounds);
    }
    else // AnimatedGradient
    {
        float t = (float)juce::Time::getMillisecondCounter() / 1000.0f;
        juce::Colour c1 = juce::Colour::fromHSV(std::fmod(t * 0.05f, 1.0f), 0.7f, 0.3f, 1.0f);
        juce::Colour c2 = juce::Colour::fromHSV(std::fmod(t * 0.05f + 0.5f, 1.0f), 0.8f, 0.4f, 1.0f);
        juce::ColourGradient grad(c1, 0, 0, c2, (float)bounds.getWidth(), (float)bounds.getHeight(), false);
        g.setGradientFill(grad);
        g.fillAll();
    }
}

void SWATronix2AAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    int meterWidth = (int)(area.getWidth() * 0.35f);
    vuMeter.setBounds(area.getX(), area.getY(), meterWidth, area.getHeight());

    auto controls = area.withTrimmedLeft(meterWidth + 10);
    int controlHeight = controls.getHeight() / 6;

    int y = controls.getY();
    auto setRow = [&](juce::Component& comp, juce::Component& label)
    {
        label.setBounds(controls.getX(), y, controls.getWidth(), 20);
        comp.setBounds(controls.getX(), y + 20, controls.getWidth(), controlHeight - 20);
        y += controlHeight;
    };

    setRow(presetLabel, presetCombo);
    setRow(themeLabel, themeCombo);
    setRow(peakReductionLabel, peakReductionSlider);
    setRow(outputGainLabel, outputGainSlider);
    setRow(modeLabel, modeButton);
    setRow(emphasisLabel, emphasisSlider);

    infoButton.setBounds(controls.getX(), y, 80, 30);
}

void SWATronix2AAudioProcessorEditor::timerCallback()
{
    vuMeter.setGainReduction(processorRef.la2a.getGainReductionDb());
    vuMeter.setOutputLevel(processorRef.la2a.getOutputLevelDb());

    if (lookAndFeel.getTheme() == PluginTheme::AnimatedGradient)
        repaint();
}
