/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

//==============================================================================
/**
 */
class TascloneAudioProcessorEditor : public AudioProcessorEditor,
									 public Slider::Listener
{
public:
	TascloneAudioProcessorEditor(TascloneAudioProcessor &, AudioProcessorValueTreeState &);
	~TascloneAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics &) override;
	void resized() override;

	void sliderValueChanged(Slider *slider) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	TascloneAudioProcessor &processor;
	AudioProcessorValueTreeState &audioTree;

	Slider inputGain;
	// Label inputGainLabel;

	Slider outputGain;
	// Label outputGainLabel;

	Slider toneControlle;
	// Label toneLabel;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttachInputGain;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttachOutputGain;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttachToneControlle;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TascloneAudioProcessorEditor)
};
