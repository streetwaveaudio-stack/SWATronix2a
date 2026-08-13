#include "PluginProcessor.h"
#include "PluginEditor.h"

SWATronix2AAudioProcessor::SWATronix2AAudioProcessor()
    : apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    themeValue.setValue(0);
    la2a.reset();
}

SWATronix2AAudioProcessor::~SWATronix2AAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout SWATronix2AAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "peakReduction", "Peak Reduction", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f,
        "Peak Reduction", juce::AudioProcessorParameter::genericParameter,
        [](float v, int) { return juce::String(v * 100.0f, 0) + "%"; }));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "outputGain", "Output Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f,
        "Output Gain", juce::AudioProcessorParameter::genericParameter,
        [](float v, int) { return juce::String(v * 40.0f - 20.0f, 1) + " dB"; }));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "mode", "Limit Mode", false, "Mode",
        [](bool v, int) { return juce::String(v ? "Limit" : "Compress"); }));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "emphasis", "Emphasis", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f,
        "Emphasis", juce::AudioProcessorParameter::genericParameter,
        [](float v, int) { return juce::String(v, 2); }));

    return layout;
}

void SWATronix2AAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    la2a.prepare(sampleRate, samplesPerBlock);
}

void SWATronix2AAudioProcessor::releaseResources()
{
    la2a.reset();
}

bool SWATronix2AAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void SWATronix2AAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float peakReduction = apvts.getRawParameterValue("peakReduction")->load();
    float outputGain = apvts.getRawParameterValue("outputGain")->load();
    bool mode = apvts.getRawParameterValue("mode")->load() > 0.5f;
    float emphasis = apvts.getRawParameterValue("emphasis")->load();

    la2a.process(buffer, peakReduction, outputGain, mode ? 1 : 0, emphasis);
}

juce::AudioProcessorEditor* SWATronix2AAudioProcessor::createEditor()
{
    return new SWATronix2AAudioProcessorEditor(*this);
}

void SWATronix2AAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("theme", themeValue.getValue(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SWATronix2AAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get())
        if (xml->hasTagName(apvts.state.getType()))
        {
            auto newState = juce::ValueTree::fromXml(*xml);
            if (newState.isValid())
            {
                if (newState.hasProperty("theme"))
                    themeValue.setValue((int)newState.getProperty("theme", 0));
                apvts.replaceState(newState);
            }
        }
}

void SWATronix2AAudioProcessor::applyPreset(const Preset& p)
{
    apvts.getParameter("peakReduction")->setValueNotifyingHost(p.peakReduction);
    apvts.getParameter("outputGain")->setValueNotifyingHost(p.outputGain);
    apvts.getParameter("mode")->setValueNotifyingHost(p.mode ? 1.0f : 0.0f);
    apvts.getParameter("emphasis")->setValueNotifyingHost(p.emphasis);
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SWATronix2AAudioProcessor();
}
