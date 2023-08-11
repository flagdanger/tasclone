

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
 */
class TascloneAudioProcessor : public juce::AudioProcessor,
							   public juce::AudioProcessorValueTreeState::Listener
{
public:
	//==============================================================================
	TascloneAudioProcessor();
	~TascloneAudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

	//==============================================================================
	juce::AudioProcessorEditor *createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String &newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock &destData) override;
	void setStateInformation(const void *data, int sizeInBytes) override;

	void parameterChanged(const juce::String &parameterID, float newValue) override;

	void updateFilter();

	juce::AudioProcessorValueTreeState audioTree;
	int distortionType, checkFilter;

	std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;

private:
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TascloneAudioProcessor)

	float outputGainSmoothed;
	float inputValue, outputValue, toneValue;
	std::size_t numChan = 2;
	std::size_t fact = 2;
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;
};
