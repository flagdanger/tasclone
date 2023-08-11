/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TascloneAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::Slider::Listener
    //public juce::Combobox:Listener
{
public:
    TascloneAudioProcessorEditor (TascloneAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~TascloneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider *slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TascloneAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& _treeState;

    juce::Slider inputGain;
    juce::Label inputLabel;

    juce::Slider outputGain;
    juce::Label outputLabel;

    juce::Slider toneKnob;
    juce::Label toneLabel;

    juce::Slider mixKnob;
    juce::Label mixLabel;

    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachmentInput;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachmentOutput;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachmentTone;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachmentMix;

    KnobLookAndFeel knobLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TascloneAudioProcessorEditor)
};
