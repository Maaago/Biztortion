/*
  ==============================================================================

    MeterModule.cpp
    Created: 1 Sep 2021 2:57:12pm
    Author:  gabri

  ==============================================================================
*/

#include "MeterModule.h"
#include "../PluginProcessor.h"

//==============================================================================

/* MeterModule DSP */

//==============================================================================

MeterModuleDSP::MeterModuleDSP(juce::AudioProcessorValueTreeState& _apvts, juce::String _type)
    : DSPModule(_apvts), type(_type) {
    setChainPosition(type == "Input" ? 0 : 9);
}

juce::String MeterModuleDSP::getType()
{
    return type;
}

MeterSettings MeterModuleDSP::getSettings(juce::AudioProcessorValueTreeState& apvts, juce::String type)
{
    MeterSettings settings;
    juce::String search = type + " Meter Level";
    settings.levelInDecibel = apvts.getRawParameterValue(search)->load();

    return settings;
}

void MeterModuleDSP::addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    layout.add(std::move(std::make_unique<juce::AudioParameterFloat>("Input Meter Level", "Input Meter Level", juce::NormalisableRange<float>(-60.f, 10.f, 0.5f), 0.f, "Input Meter")));
    layout.add(std::move(std::make_unique<juce::AudioParameterFloat>("Output Meter Level", "Output Meter Level", juce::NormalisableRange<float>(-60.f, 10.f, 0.5f), 0.f, "Output Meter")));
}

foleys::LevelMeterSource& MeterModuleDSP::getMeterSource()
{
    return meterSource;
}

void MeterModuleDSP::setModuleType()
{
    moduleType = ModuleType::Meter;
}

void MeterModuleDSP::updateDSPState(double)
{
    auto settings = getSettings(apvts, type);
    level.setGainDecibels(settings.levelInDecibel);
}

void MeterModuleDSP::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;

    level.prepare(spec);

    meterSource.resize(2, sampleRate * 0.1 / samplesPerBlock);
}

void MeterModuleDSP::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&, double sampleRate)
{
    updateDSPState(sampleRate);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    level.process(context);

    meterSource.measureBlock(buffer);
}

//==============================================================================

/* MeterModule GUI */

//==============================================================================

MeterModuleGUI::MeterModuleGUI(BiztortionAudioProcessor& p, juce::String _type)
    : GUIModule(), audioProcessor(p), type(_type),
    levelSlider(*audioProcessor.apvts.getParameter(type + " Meter Level"), "dB"),
    levelSliderAttachment(audioProcessor.apvts, type + " Meter Level", levelSlider)
{
    meterTitle.setText(type, juce::dontSendNotification);
    meterTitle.setFont(14);
    addAndMakeVisible(meterTitle);

    // custom colors
    lnf.setColour(foleys::LevelMeter::lmBackgroundColour, juce::Colours::black);
    lnf.setColour(foleys::LevelMeter::lmTicksColour, juce::Colours::white);
    lnf.setColour(foleys::LevelMeter::lmMeterGradientMidColour, juce::Colours::darkorange);
    lnf.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colour(0, 240, 48));
    meter.setLookAndFeel(&lnf);
    meter.setMeterSource(getMeterSource());
    addAndMakeVisible(meter);

    addAndMakeVisible(levelSlider);
    levelSlider.labels.add({ 0.f, "-60dB" });
    levelSlider.labels.add({ 1.f, "+10dB" });
}

MeterModuleGUI::~MeterModuleGUI()
{
    meter.setLookAndFeel(nullptr);
}

juce::String MeterModuleGUI::getType()
{
    return type;
}

foleys::LevelMeterSource* MeterModuleGUI::getMeterSource()
{
    foleys::LevelMeterSource* source = nullptr;
    for (auto it = audioProcessor.DSPmodules.cbegin(); it < audioProcessor.DSPmodules.cend(); ++it) {
        auto temp = dynamic_cast<MeterModuleDSP*>(&(**it));
        if (temp && temp->getType() == type) {
            source = &temp->getMeterSource();
        }
    }
    return source;
}

std::vector<juce::Component*> MeterModuleGUI::getComps()
{
    return std::vector<juce::Component*>();
}

void MeterModuleGUI::paint(juce::Graphics& g)
{
    drawContainer(g);
}

void MeterModuleGUI::resized()
{
    auto bounds = getContentRenderArea();

    auto titleArea = bounds.removeFromTop(bounds.getHeight() * (1.f / 8.f));
    auto meterArea = bounds.removeFromTop(bounds.getHeight() * (3.f / 4.f));
    meterArea.reduce(4, 4);
    bounds.reduce(4, 4);

    meterTitle.setBounds(titleArea);
    meterTitle.setJustificationType(juce::Justification::centred);
    meter.setBounds(meterArea);
    levelSlider.setBounds(bounds);
}
