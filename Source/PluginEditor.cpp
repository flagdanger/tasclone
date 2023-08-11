/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TascloneAudioProcessorEditor::TascloneAudioProcessorEditor(TascloneAudioProcessor &p, AudioProcessorValueTreeState &vts)
	: AudioProcessorEditor(&p), processor(p), audioTree(vts)
{
	setSize(500, 200);

	// Input Gain
	inputGain.setSliderStyle(Slider::SliderStyle::Rotary);
	inputGain.addListener(this);
	inputGain.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	addAndMakeVisible(inputGain);
	sliderAttachInputGain.reset(new AudioProcessorValueTreeState::SliderAttachment(audioTree, "InputGain_ID", inputGain));
	// inputGainLabel.setText("Input Gain", dontSendNotification);
	// addAndMakeVisible(inputGainLabel);

	// Output Gain
	outputGain.setSliderStyle(Slider::SliderStyle::Rotary);
	outputGain.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	outputGain.addListener(this);
	addAndMakeVisible(outputGain);
	sliderAttachOutputGain.reset(new AudioProcessorValueTreeState::SliderAttachment(audioTree, "OutputGain_ID", outputGain));
	// outputGainLabel.setText("Volume", dontSendNotification);
	// addAndMakeVisible(outputGainLabel);

	// Tone Controlle
	toneControlle.setSliderStyle(Slider::SliderStyle::Rotary);
	toneControlle.addListener(this);
	toneControlle.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
	addAndMakeVisible(toneControlle);
	sliderAttachToneControlle.reset(new AudioProcessorValueTreeState::SliderAttachment(audioTree, "ToneControlle_ID", toneControlle));
	// toneLabel.setText("Tone Control", dontSendNotification);
	// addAndMakeVisible(toneLabel);
}

TascloneAudioProcessorEditor::~TascloneAudioProcessorEditor()
{
	sliderAttachInputGain.reset();
	sliderAttachOutputGain.reset();
	sliderAttachToneControlle.reset();
}

//==============================================================================
void TascloneAudioProcessorEditor::paint(Graphics &g)
{
}

void TascloneAudioProcessorEditor::resized()
{
	const auto heightFromTop = 25;

	inputGain.setBounds(25, heightFromTop, 150, 150);
	outputGain.setBounds(200, heightFromTop, 150, 150);
	toneControlle.setBounds(375, heightFromTop, 150, 150);

	// also set bounds of labels if added back
}

void TascloneAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
	std::cout << "Slider " << slider->getName() << " value changed to " << slider->getValue() << '\n';
}
