/*
  ==============================================================================

    PluginEditor.h

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

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Component/FFTAnalyzerComponent.h"
#include "Component/TransferFunctionGraphComponent.h"
#include "Shared/GUIStuff.h"
#include "Module/FilterModule.h"
#include "Module/WaveshaperModule.h"
#include "Module/NewModule.h"
#include "Module/WelcomeModule.h"

//==============================================================================
/** EDITOR
*/
class BiztortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BiztortionAudioProcessorEditor (BiztortionAudioProcessor&);
    ~BiztortionAudioProcessorEditor() override;

    void editorSetup();

    void paint (juce::Graphics&) override;
    void resized() override;

    std::unique_ptr<GUIModule> inputMeter;
    std::unique_ptr<GUIModule> outputMeter;
    std::unique_ptr<GUIModule> currentGUIModule;
    std::vector<std::unique_ptr<NewModuleGUI>> newModules;

private:

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiztortionAudioProcessor& audioProcessor;

    juce::HyperlinkButton githubLink;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiztortionAudioProcessorEditor)
};