/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TascloneAudioProcessor::TascloneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
						 .withInput("Input", AudioChannelSet::stereo(), true)
#endif
						 .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
						 ),
	  audioTree(*this, nullptr, Identifier("PARAMETERS"),
				{std::make_unique<AudioParameterFloat>("InputGain_ID", "InputGain", NormalisableRange<float>(0.0, 48.0, 0.1), 10.0),
				 std::make_unique<AudioParameterFloat>("OutputGain_ID", "OutputGain", NormalisableRange<float>(-48.0, 10.0, 0.1), 0.0),
				 std::make_unique<AudioParameterFloat>("ToneControlle_ID", "ToneControlle", NormalisableRange<float>(20.0, 20000.0, 6.0), 10000)}),
	  lowPassFilter(dsp::IIR::Coefficients<float>::makeLowPass((44100 * 4), 20000.0))

#endif
{

	audioTree.addParameterListener("InputGain_ID", this);
	audioTree.addParameterListener("OutputGain_ID", this);
	audioTree.addParameterListener("ToneControlle_ID", this);
	oversampling.reset(new dsp::Oversampling<float>(2, 2, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false));

	inputValue = 1.0;
	outputValue = 1.0;
	toneValue = 10000;

	distortionType = 1;
	checkFilter = 1;
}

TascloneAudioProcessor::~TascloneAudioProcessor()
{
	oversampling.reset();
}

//==============================================================================
const String TascloneAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool TascloneAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool TascloneAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool TascloneAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double TascloneAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int TascloneAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
			  // so this should be at least 1, even if you're not really implementing programs.
}

int TascloneAudioProcessor::getCurrentProgram()
{
	return 0;
}

void TascloneAudioProcessor::setCurrentProgram(int index)
{
}

const String TascloneAudioProcessor::getProgramName(int index)
{
	return {};
}

void TascloneAudioProcessor::changeProgramName(int index, const String &newName)
{
}

//==============================================================================
void TascloneAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	oversampling->reset();
	oversampling->initProcessing(static_cast<size_t>(samplesPerBlock));

	dsp::ProcessSpec spec;
	
	spec.sampleRate = sampleRate * 4;
	spec.maximumBlockSize = samplesPerBlock * 3;
	spec.numChannels = getTotalNumOutputChannels();

	lowPassFilter.prepare(spec);
	lowPassFilter.reset();
}

void TascloneAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TascloneAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

		// This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void TascloneAudioProcessor::parameterChanged(const String &parameterID, float newValue)
{
	if (parameterID == "InputGain_ID")
	{
		inputValue = pow(10, newValue / 20);
	}
	else if (parameterID == "OutputGain_ID")
	{
		outputValue = pow(10, newValue / 20);
	}
	else if (parameterID == "ToneControlle_ID")
	{
		toneValue = newValue;
	}
}

void TascloneAudioProcessor::updateFilter()
{
	float frequency = 44100 * 4;
	*lowPassFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(frequency, toneValue);
}

void TascloneAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	juce::dsp::AudioBlock<float> blockInput(buffer);
	juce::dsp::AudioBlock<float> blockOutput = oversampling->processSamplesUp(blockInput);

	for (int channel = 0; channel < blockOutput.getNumChannels(); channel++) {
		for (int sample = 0; sample < blockOutput.getNumSamples(); sample++) {

			float signalIN = blockOutput.getSample(channel, sample);

			signalIN *= inputValue;

			float signalOUT;

			//soft clip 
			/*
			if (signalIN > 0.5f) {
				signalOUT = std::log(-0.2f * signalIN + 2.85f);
			}
			else if (signalIN < 0.5f) {
				signalOUT = -std::log(0.2f * signalIN + 2.85f);
			}
			else {
				signalOUT = signalIN * std::pow(2.05f - std::abs((3.5f * signalIN) / 2 - 0.8f), 0.8f);
			}
			*/

			//sigmoid function
			if (signalIN > 0)
				signalOUT = 1.0f - expf(-signalIN);
			else
				signalOUT = -1.0f + expf(signalIN);

			//signalOUT *= (1 / (std::log(inputValue + 1) + 1));

			outputGainSmoothed = outputGainSmoothed - 0.004 * (outputGainSmoothed - outputValue);
			signalOUT *= outputGainSmoothed;

			blockOutput.setSample(channel, sample, signalOUT);

		}
	}

	updateFilter();
	lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(blockOutput));

	oversampling->processSamplesDown(blockInput);
}

//==============================================================================
bool TascloneAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *TascloneAudioProcessor::createEditor()
{
	return new TascloneAudioProcessorEditor(*this, audioTree);
}

//==============================================================================
void TascloneAudioProcessor::getStateInformation(MemoryBlock &destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void TascloneAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
	return new TascloneAudioProcessor();
}
