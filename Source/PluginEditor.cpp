#include "PluginProcessor.h"
#include "PluginEditor.h"

const char *const inputGainId = "InputGain_ID",
		   *const outputGainId = "OutputGain_ID",
		   *const toneControllerId = "ToneController_ID";

//==============================================================================
TascloneAudioProcessorEditor::TascloneAudioProcessorEditor(TascloneAudioProcessor &p, AudioProcessorValueTreeState &vts)
	: AudioProcessorEditor(&p), processor(p), audioTree(vts)
{
	setResizable(false, false);
	setSize(375, 150);

	// Input Gain
	inputGain.setSliderStyle(Slider::SliderStyle::Rotary);
	inputGain.addListener(this);
	inputGain.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 50, 25);
	addAndMakeVisible(inputGain);
	sliderAttachInputGain.reset(new AudioProcessorValueTreeState::SliderAttachment(audioTree, inputGainId, inputGain));

	// input gain label
	inputGainLabel.setText("Input", dontSendNotification);
	inputGainLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(inputGainLabel);

	// Output Gain
	outputGain.setSliderStyle(Slider::SliderStyle::Rotary);
	outputGain.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 50, 25);
	outputGain.addListener(this);
	addAndMakeVisible(outputGain);
	sliderAttachOutputGain.reset(new AudioProcessorValueTreeState::SliderAttachment(audioTree, outputGainId, outputGain));

	// outpit gain label
	outputGainLabel.setText("Output", dontSendNotification);
	outputGainLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(outputGainLabel);

	// Tone Controller
	toneController.setSliderStyle(Slider::SliderStyle::Rotary);
	toneController.addListener(this);
	toneController.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 50, 25);
	addAndMakeVisible(toneController);
	sliderAttachToneControlle.reset(new AudioProcessorValueTreeState::SliderAttachment(audioTree, toneControllerId, toneController));

	// tone controller label
	toneLabel.setText("Tone", dontSendNotification);
	toneLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(toneLabel);
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

// layout constants
const auto sliderSideLength = 100;
const auto firstSliderX = 10;
const auto distanceBetweenSliders = 25;
const auto sliderY = firstSliderX;
const auto labelDistanceBelowSlider = 10;
const auto labelY = sliderSideLength + labelDistanceBelowSlider;

int sliderX(int i)
{
	return firstSliderX + ((sliderSideLength + distanceBetweenSliders) * i);
}

void TascloneAudioProcessorEditor::resized()
{
	inputGain.setBounds(sliderX(0), sliderY, sliderSideLength, sliderSideLength);
	outputGain.setBounds(sliderX(1), sliderY, sliderSideLength, sliderSideLength);
	toneController.setBounds(sliderX(2), sliderY, sliderSideLength, sliderSideLength);

	// also set bounds of labels if added back
	inputGainLabel.setBounds(sliderX(0), labelY, sliderSideLength, 25);
	outputGainLabel.setBounds(sliderX(1), labelY, sliderSideLength, 25);
	toneLabel.setBounds(sliderX(2), labelY, sliderSideLength, 25);
}

void TascloneAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
	std::cout << "Slider " << slider->getName() << " value changed to " << slider->getValue() << '\n';
}
