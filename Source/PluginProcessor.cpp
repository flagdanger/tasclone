/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIds.h"

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
				{std::make_unique<AudioParameterFloat>(inputGainId, "InputGain", NormalisableRange<float>(0.0, 48.0, 0.1), 10.0),
				 std::make_unique<AudioParameterFloat>(outputGainId, "OutputGain", NormalisableRange<float>(-48.0, 10, 0.1), 0.0),
				 std::make_unique<AudioParameterFloat>(toneControllerId, "ToneController", NormalisableRange<float>(20.0, 20000.0, 6.0), 10000)}),
	  lowPassFilter(dsp::IIR::Coefficients<float>::makeLowPass((44100 * 4), 20000.0))

#endif
{

	audioTree.addParameterListener(inputGainId, this);
	audioTree.addParameterListener(outputGainId, this);
	audioTree.addParameterListener(toneControllerId, this);
	oversampling.reset(new dsp::Oversampling<float>(2, 2, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false));

	inputGainValue = 1.0;
	outputGainValue = 1.0;
	toneControlleValue = 10000;

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
	// Sample Rate of the filter must be 4 times because the Oversampling
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
	// Parameters update  when sliders moved
	if (parameterID == inputGainId)
	{
		// in db
		inputGainValue = pow(10, newValue / 20);
		// inputGainValue = newValue;
	}
	else if (parameterID == outputGainId)
	{
		// in db
		outputGainValue = pow(10, newValue / 20);
	}
	else if (parameterID == toneControllerId)
	{
		toneControlleValue = newValue;
	}
}

void TascloneAudioProcessor::updateFilter()
{
	float frequency = 44100 * 4;
	*lowPassFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(frequency, toneControlleValue);
}

void TascloneAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	dsp::AudioBlock<float> blockInput(buffer);
	dsp::AudioBlock<float> blockOutput = oversampling->processSamplesUp(blockInput);

	// lowPassFilter (condition used to check if apply the filter before or after the distortion
	if (checkFilter == 0)
	{
		updateFilter();
		lowPassFilter.process(dsp::ProcessContextReplacing<float>(blockOutput));
	}

	for (int channel = 0; channel < blockOutput.getNumChannels(); channel++)
	{
		for (int sample = 0; sample < blockOutput.getNumSamples(); sample++)
		{
			// Take the sample from the Audio Block
			float in = blockOutput.getSample(channel, sample);

			// Input Gain (Not for Full wave and Half wave rectifier)
			if (distortionType == 1 || distortionType == 2 || distortionType == 3)
			{
				in *= inputGainValue;
			}

			// Distortion Type
			float out;
			if (distortionType == 1)
			{
				// Simple hard clipping
				float threshold = 1.0f;
				if (in > threshold)
					out = threshold;
				else if (in < -threshold)
					out = -threshold;
				else
					out = in;
			}
			else if (distortionType == 2)
			{
				// Soft clipping based on quadratic function
				float threshold1 = 1.0f / 3.0f;
				float threshold2 = 2.0f / 3.0f;
				if (in > threshold2)
					out = 1.0f;
				else if (in > threshold1)
					out = (3.0f - (2.0f - 3.0f * in) *
									  (2.0f - 3.0f * in)) /
						  3.0f;
				else if (in < -threshold2)
					out = -1.0f;
				else if (in < -threshold1)
					out = -(3.0f - (2.0f + 3.0f * in) *
									   (2.0f + 3.0f * in)) /
						  3.0f;
				else
					out = 2.0f * in;
			}
			else if (distortionType == 3)
			{
				// Soft clipping based on exponential function
				if (in > 0)
					out = 1.0f - expf(-in);
				else
					out = -1.0f + expf(in);
			}
			else if (distortionType == 4)
			{
				// Full-wave rectifier (absolute value)
				out = fabsf(in);
			}
			else if (distortionType == 5)
			{
				// Half-wave rectifier
				if (in > 0)
					out = in;
				else
					out = 0;
			}

			// match output with input
			if (distortionType == 1 || distortionType == 2 || distortionType == 3)
			{
				out *= (1 / (log(inputGainValue + 1) + 1));
			}

			// Allow the user to modify the output level (smoothing)
			parameterOutputGainSmoothed = parameterOutputGainSmoothed - 0.004 * (parameterOutputGainSmoothed - outputGainValue);
			out *= parameterOutputGainSmoothed;

			// Set the new sample in the audio block
			blockOutput.setSample(channel, sample, out);
		}
	}

	// lowPassFilter (condition used to check if apply the filter before or after the distortion
	if (checkFilter == 1)
	{
		updateFilter();
		lowPassFilter.process(dsp::ProcessContextReplacing<float>(blockOutput));
	}

	// DownSampling
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
