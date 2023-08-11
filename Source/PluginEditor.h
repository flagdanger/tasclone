/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
 */
class TascloneAudioProcessorEditor : public juce::AudioProcessorEditor,
									 public juce::Slider::Listener
{
public:
	TascloneAudioProcessorEditor(TascloneAudioProcessor &, juce::AudioProcessorValueTreeState &);
	~TascloneAudioProcessorEditor();

	//==============================================================================
	void paint(juce::Graphics &) override;
	void resized() override;

	void sliderValueChanged(juce::Slider *slider) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	TascloneAudioProcessor &processor;
	juce::AudioProcessorValueTreeState &audioTree;

	juce::Slider inputGain;
	juce::Label inputGainLabel;

	juce::Slider outputGain;
	juce::Label outputGainLabel;

	juce::Slider toneController;
	juce::Label toneLabel;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachInputGain;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachOutputGain;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachToneControlle;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TascloneAudioProcessorEditor)
};
