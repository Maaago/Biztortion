/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ResponseCurveComponent.h"
#include "FFTAnalyzerComponent.h"
#include "TransferFunctionGraphComponent.h"

struct CustomRotatorySlider : juce::Slider {
    CustomRotatorySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

//==============================================================================
/** EDITOR
*/
class BiztortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BiztortionAudioProcessorEditor (BiztortionAudioProcessor&);
    ~BiztortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiztortionAudioProcessor& audioProcessor;

    // filterModule
    CustomRotatorySlider peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider;
    Attachment peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment;
    ResponseCurveComponent responseCurveComponent;
    FFTAnalyzerComponent filterFftAnalyzerComponent;

    // waveshaperModule
    TransferFunctionGraphComponent transferFunctionGraph;
    // juce::LookAndFeel_V4 lookAndFeel1, lookAndFeel2, lookAndFeel3;
    juce::Label waveshaperDriveLabel,
        waveshaperMixLabel,
        tanhAmpLabel,
        tanhSlopeLabel,
        sineAmpLabel,
        sineFreqLabel;
    CustomRotatorySlider waveshaperDriveSlider,
        waveshaperMixSlider,
        tanhAmpSlider,
        tanhSlopeSlider,
        sineAmpSlider,
        sineFreqSlider;
    Attachment waveshaperDriveSliderAttachment,
        waveshaperMixSliderAttachment,
        tanhAmpSliderAttachment,
        tanhSlopeSliderAttachment,
        sineAmpSliderAttachment,
        sineFreqSliderAttachment;

    // fft analyzer
    FFTAnalyzerComponent analyzerComponent;

    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiztortionAudioProcessorEditor)
};