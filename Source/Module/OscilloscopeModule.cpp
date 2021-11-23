/*
  ==============================================================================

    OscilloscopeModule.cpp

    Copyright (c) 2021 KillBizz - Gabriel Bizzo

  ==============================================================================
*/

/*

This file is part of Biztortion software.

Biztortion is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Biztortion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Biztortion. If not, see < http://www.gnu.org/licenses/>.

*/

//==============================================================================

/* OscilloscopeModule DSP */

//==============================================================================

#include "OscilloscopeModule.h"
#include "../PluginProcessor.h"

OscilloscopeModuleDSP::OscilloscopeModuleDSP(juce::AudioProcessorValueTreeState& _apvts)
    : DSPModule(_apvts)
{
    leftOscilloscope.clear();
    rightOscilloscope.clear();
}

drow::AudioOscilloscope* OscilloscopeModuleDSP::getLeftOscilloscope()
{
    return &leftOscilloscope;
}

drow::AudioOscilloscope* OscilloscopeModuleDSP::getRightOscilloscope()
{
    return &rightOscilloscope;
}

OscilloscopeSettings OscilloscopeModuleDSP::getSettings(juce::AudioProcessorValueTreeState& apvts, unsigned int chainPosition)
{
    OscilloscopeSettings settings;
    settings.hZoom = apvts.getRawParameterValue("Oscilloscope H Zoom " + std::to_string(chainPosition))->load();
    settings.vZoom = apvts.getRawParameterValue("Oscilloscope V Zoom " + std::to_string(chainPosition))->load();
    settings.bypassed = apvts.getRawParameterValue("Oscilloscope Bypassed " + std::to_string(chainPosition))->load() > 0.5f;

    return settings;
}

void OscilloscopeModuleDSP::addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    for (int i = 1; i < 9; ++i) {
        layout.add(std::move(std::make_unique<juce::AudioParameterFloat>("Oscilloscope H Zoom " + std::to_string(i), 
            "Oscilloscope H Zoom " + std::to_string(i), 
            juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 0.f, "Oscilloscope " + std::to_string(i))));
        layout.add(std::move(std::make_unique<juce::AudioParameterFloat>("Oscilloscope V Zoom " + std::to_string(i), 
            "Oscilloscope V Zoom " + std::to_string(i), 
            juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 1.f, "Oscilloscope " + std::to_string(i))));
        // bypass button
        layout.add(std::make_unique<AudioParameterBool>("Oscilloscope Bypassed " + std::to_string(i), "Oscilloscope Bypassed " + std::to_string(i), 
            false, "Oscilloscope " + std::to_string(i)));
    }
}

void OscilloscopeModuleDSP::setModuleType()
{
    moduleType = ModuleType::Oscilloscope;
}

void OscilloscopeModuleDSP::updateDSPState(double sampleRate)
{
    auto settings = getSettings(apvts, getChainPosition());
    bypassed = settings.bypassed;
    leftOscilloscope.setHorizontalZoom(settings.hZoom);
    leftOscilloscope.setVerticalZoom(settings.vZoom);
    rightOscilloscope.setHorizontalZoom(settings.hZoom);
    rightOscilloscope.setVerticalZoom(settings.vZoom);
}

void OscilloscopeModuleDSP::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    leftOscilloscope.clear();
    rightOscilloscope.clear();
    updateDSPState(sampleRate);
}

void OscilloscopeModuleDSP::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, double sampleRate)
{
    updateDSPState(sampleRate);
    if (!bypassed) {
        leftOscilloscope.processBlock(buffer.getReadPointer(0), buffer.getNumSamples());
        rightOscilloscope.processBlock(buffer.getReadPointer(1), buffer.getNumSamples());
    }
}

//==============================================================================

/* OscilloscopeModule GUI */

//==============================================================================

OscilloscopeModuleGUI::OscilloscopeModuleGUI(BiztortionAudioProcessor& p, drow::AudioOscilloscope* _leftOscilloscope, drow::AudioOscilloscope* _rightOscilloscope, unsigned int chainPosition)
    : GUIModule(), audioProcessor(p), leftOscilloscope(_leftOscilloscope), rightOscilloscope(_rightOscilloscope),
    hZoomSlider(*audioProcessor.apvts.getParameter("Oscilloscope H Zoom " + std::to_string(chainPosition)), ""),
    vZoomSlider(*audioProcessor.apvts.getParameter("Oscilloscope V Zoom " + std::to_string(chainPosition)), ""),
    hZoomSliderAttachment(audioProcessor.apvts, "Oscilloscope H Zoom " + std::to_string(chainPosition), hZoomSlider),
    vZoomSliderAttachment(audioProcessor.apvts, "Oscilloscope V Zoom " + std::to_string(chainPosition), vZoomSlider),
    bypassButtonAttachment(audioProcessor.apvts, "Oscilloscope Bypassed " + std::to_string(chainPosition), bypassButton)
{
    // title setup
    title.setText("Oscilloscope", juce::dontSendNotification);
    title.setFont(juce::Font("Prestige Elite Std", 24, 0));

    // labels
    hZoomLabel.setText("H Zoom", juce::dontSendNotification);
    hZoomLabel.setFont(juce::Font("Prestige Elite Std", 10, 0));
    vZoomLabel.setText("V Zoom", juce::dontSendNotification);
    vZoomLabel.setFont(juce::Font("Prestige Elite Std", 10, 0));

    hZoomSlider.labels.add({ 0.f, "0" });
    hZoomSlider.labels.add({ 1.f, "1" });
    vZoomSlider.labels.add({ 0.f, "0" });
    vZoomSlider.labels.add({ 1.f, "1" });

    // bypass button
    bypassButton.setLookAndFeel(&lnf);

    auto safePtr = juce::Component::SafePointer<OscilloscopeModuleGUI>(this);
    bypassButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto bypassed = comp->bypassButton.getToggleState();
            comp->handleParamCompsEnablement(bypassed);
        }
    };

    // freeze button
    freezeButton.setClickingTogglesState(true);
    freezeButton.setToggleState(false, juce::dontSendNotification);

    freezeLnf.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    freezeLnf.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    freezeLnf.setColour(juce::TextButton::buttonOnColourId, juce::Colours::black);
    freezeLnf.setColour(juce::TextButton::textColourOnId, juce::Colours::lightgreen);

    freezeButton.setLookAndFeel(&freezeLnf);

    freezeButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto freeze = comp->freezeButton.getToggleState();
            if (freeze) {
                comp->leftOscilloscope->stopTimer();
                comp->rightOscilloscope->stopTimer();
            }
            else {
                comp->leftOscilloscope->startTimerHz(59);
                comp->rightOscilloscope->startTimerHz(59);
            }
            
        }
    };

    // tooltips
    bypassButton.setTooltip("Bypass this module");
    hZoomSlider.setTooltip("Adjust the horizontal zoom of the oscilloscope");
    vZoomSlider.setTooltip("Adjust the vertical zoom of the oscilloscope");
    freezeButton.setTooltip("Take a snapshot of the oscilloscope");

    leftOscilloscope->startTimerHz(59);
    rightOscilloscope->startTimerHz(59);

    for (auto* comp : getAllComps())
    {
        addAndMakeVisible(comp);
    }

    handleParamCompsEnablement(bypassButton.getToggleState());
}

OscilloscopeModuleGUI::~OscilloscopeModuleGUI()
{
    leftOscilloscope->stopTimer();
    rightOscilloscope->stopTimer();
    leftOscilloscope = nullptr;
    rightOscilloscope = nullptr;
    freezeButton.setLookAndFeel(nullptr);
    bypassButton.setLookAndFeel(nullptr);
}

std::vector<juce::Component*> OscilloscopeModuleGUI::getAllComps()
{
    return {
        leftOscilloscope,
        rightOscilloscope,
        &title,
        &hZoomLabel,
        &vZoomLabel,
        &hZoomSlider,
        &vZoomSlider,
        &freezeButton,
        &bypassButton
    };
}

std::vector<juce::Component*> OscilloscopeModuleGUI::getParamComps()
{
    return {
        &hZoomSlider,
        &vZoomSlider,
        &freezeButton
    };
}

void OscilloscopeModuleGUI::paint(juce::Graphics& g)
{
    drawContainer(g);
}

void OscilloscopeModuleGUI::paintOverChildren(Graphics& g)
{
    auto leftOscArea = leftOscilloscope->getBounds();
    auto rightOscArea = rightOscilloscope->getBounds();

    g.setColour(juce::Colours::white);
    g.drawLine(juce::Line<float>(leftOscArea.getBottomLeft().toFloat(), leftOscArea.getBottomRight().toFloat()), 1.5f);

    g.setColour(juce::Colours::darkgrey);

    auto left = juce::Point<float>((float)leftOscArea.getTopLeft().getX(), leftOscArea.getCentreY());
    auto right = juce::Point<float>((float)leftOscArea.getTopRight().getX(), leftOscArea.getCentreY());
    g.drawLine(juce::Line<float>(left, right), 1.f);

    left = juce::Point<float>((float)rightOscArea.getTopLeft().getX(), rightOscArea.getCentreY());
    right = juce::Point<float>((float)rightOscArea.getTopRight().getX(), rightOscArea.getCentreY());
    g.drawLine(juce::Line<float>(left, right), 1.f);
}

void OscilloscopeModuleGUI::resized()
{
    auto oscilloscopeArea = getContentRenderArea();

    // bypass
    auto temp = oscilloscopeArea;
    auto bypassButtonArea = temp.removeFromTop(25);

    bypassButtonArea.setWidth(35.f);
    bypassButtonArea.setX(145.f);
    bypassButtonArea.setY(20.f);

    bypassButton.setBounds(bypassButtonArea);

    auto titleAndBypassArea = oscilloscopeArea.removeFromTop(30);
    titleAndBypassArea.translate(0, 4);

    auto graphArea = oscilloscopeArea.removeFromTop(oscilloscopeArea.getHeight() * (2.f / 3.f));
    graphArea.reduce(10, 10);

    auto freezeArea = oscilloscopeArea.removeFromLeft(oscilloscopeArea.getWidth() * (1.f / 3.f));
    juce::Rectangle<int> freezeCorrectArea = freezeArea;
    freezeCorrectArea.reduce(50.f, 34.f);
    freezeCorrectArea.setCentre(freezeArea.getCentre());

    auto hZoomArea = oscilloscopeArea.removeFromLeft(oscilloscopeArea.getWidth() * (1.f / 2.f));
    auto hZoomLabelArea = hZoomArea.removeFromTop(12);

    auto vZoomArea = oscilloscopeArea;
    auto vZoomLabelArea = vZoomArea.removeFromTop(12);

    title.setBounds(titleAndBypassArea);
    title.setJustificationType(juce::Justification::centredBottom);

    hZoomLabel.setBounds(hZoomLabelArea);
    hZoomLabel.setJustificationType(juce::Justification::centred);
    vZoomLabel.setBounds(vZoomLabelArea);
    vZoomLabel.setJustificationType(juce::Justification::centred);

    leftOscilloscope->setBounds(graphArea.removeFromTop(graphArea.getHeight() * (1.f / 2.f)));
    rightOscilloscope->setBounds(graphArea);
    hZoomSlider.setBounds(hZoomArea);
    vZoomSlider.setBounds(vZoomArea);
    freezeButton.setBounds(freezeCorrectArea);
}
