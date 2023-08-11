/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TascloneAudioProcessorEditor::TascloneAudioProcessorEditor(TascloneAudioProcessor& p, juce::AudioProcessorValueTreeState& ts)
    : AudioProcessorEditor(&p), audioProcessor(p), _treeState(ts)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);

    inputGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    inputGain.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    inputGain.addListener(this);
    inputGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(inputGain);
    sliderAttachmentInput.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(_treeState, "inID", inputGain));
    inputGain.setLookAndFeel(&knobLookAndFeel);
    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.setFont(juce::Font("Multicolore", 12, 1));
    toneLabel.setColour(juce::Label::textColourId, juce::Colour(242, 242, 242));
    addAndMakeVisible(toneLabel);
}

TascloneAudioProcessorEditor::~TascloneAudioProcessorEditor()
{
}

//==============================================================================
void TascloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void TascloneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
