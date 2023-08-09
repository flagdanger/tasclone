/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include <math.h>

//==============================================================================
TascloneAudioProcessor::TascloneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , _treeState(*this, nullptr, "PARAMETERS", 
        { std::make_unique<juce::AudioParameterFloat>("inID","_input",juce::NormalisableRange<float>(0.0, 48.0,0.1),10.0),
          std::make_unique<juce::AudioParameterFloat>("outID","_output",juce::NormalisableRange<float>(-48.0,10,0.1),0.0),
          std::make_unique<juce::AudioParameterFloat>("toneID","_tone",juce::NormalisableRange<float>(20.0, 20000.0, 6.0),10000.0),
          std::make_unique<juce::AudioParameterFloat>("mixID","_mix",juce::NormalisableRange<float>(0.0, 100.0,0.1), 50.0)
        }
    ),
    _lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass((44100 * 4), 20000.0))
            
#endif
{
    _treeState.addParameterListener(inID, this);
    _treeState.addParameterListener(outID, this);
    _treeState.addParameterListener(toneID, this);
    _treeState.addParameterListener(mixID, this);

    oversampling.reset(new juce::dsp::Oversampling<float>(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false));

    _input = 1.0;
    _output = 1.0;
    _tone = 10000.0;
    _mix = 50.0;


    //KEEP WORKING HERE YOU HAVE MORE TO DO HERE, AND MOVE DISTORION CLASS INTO PLUGINPROCESSOR
}

TascloneAudioProcessor::~TascloneAudioProcessor()
{
    _treeState.removeParameterListener(inID, this);
    _treeState.removeParameterListener(outID, this);
    _treeState.removeParameterListener(toneID, this);
    _treeState.removeParameterListener(mixID, this);
    oversampling.reset();
}

/*
juce::AudioProcessorValueTreeState::ParameterLayout TascloneAudioProcessor::createParameterLayout() {

    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    auto inputParam = std::make_unique<juce::AudioParameterFloat>(inID, inName, juce::NormalisableRange<float>(0.0f, 48.0f, 0.1f), 10.0f);
    auto outputParam = std::make_unique<juce::AudioParameterFloat>(outID, outName, juce::NormalisableRange<float>(-48.0f, 10.0f, 0.1f), 0.0f);
    auto toneParam = std::make_unique<juce::AudioParameterFloat>(toneID, toneName, juce::NormalisableRange<float>(20.0f, 20000.0f, 6.0f), 10000.0f);
    auto mixParam = std::make_unique<juce::AudioParameterFloat>(mixID, mixName, juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f), 1.0f);

    params.push_back(std::move(inputParam));
    params.push_back(std::move(outputParam));
    params.push_back(std::move(toneParam));
    params.push_back(std::move(mixParam));

    return { params.begin(), params.end() };
}


void TascloneAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {

    updateParameters();
}

void TascloneAudioProcessor::updateParameters() {

    _distortionModule.setInput(_treeState.getRawParameterValue(inID)->load());
    _distortionModule.setOutput(_treeState.getRawParameterValue(outID)->load());
    _distortionModule.setTone(_treeState.getRawParameterValue(toneID)->load());
    _distortionModule.setMix(_treeState.getRawParameterValue(mixID)->load());
}
*/

//==============================================================================
const juce::String TascloneAudioProcessor::getName() const
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
    return 1;
}

int TascloneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TascloneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TascloneAudioProcessor::getProgramName (int index)
{
    return {};
}

void TascloneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TascloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling->reset();
    oversampling->initProcessing(static_cast<size_t> (samplesPerBlock));

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock*3;
    spec.sampleRate = sampleRate*4;
    spec.numChannels = getTotalNumOutputChannels();

    _lowPassFilter.prepare(spec);
    _lowPassFilter.reset();

}

void TascloneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TascloneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif







void TascloneAudioProcessor::parameterChanged(const juce::String& parameterID, float val) {
    if (parameterID == "inID") {
        _input = pow(10, val / 20);
    }
    else if (parameterID == "outID") {
        _output = pow(10, val / 20);
    }
    else if (parameterID == "toneID") {
        _tone = val;
    }
    else if (parameterID == "mixID") {
        _mix = val;
    }
}


void TascloneAudioProcessor::updateFilter() {
    float frequencyVal = 44100 * 4;
    *_lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(frequencyVal, _tone);
}


void TascloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    //juce::dsp::AudioBlock<float> block {buffer};
    juce::dsp::AudioBlock<float> blockInput(buffer);
    juce::dsp::AudioBlock<float> blockOutput = oversampling->processSamplesUp(blockInput);

    for (int channel = 0; channel < blockOutput.getNumChannels(); channel++) {
        for (int sample = 0; sample < blockOutput.getNumSamples(); sample++) {

            float signalIN = blockOutput.getSample(channel, sample);

            signalIN *= _input;

            float signalOUT;

            //soft clip 
            if (signalIN > 1.0f) {
                signalOUT = log(-0.2f * signalIN + 2.85f);
            }
            else if (signalIN < 1.0f) {
                signalOUT = -log(0.2f * signalIN + 2.85f);
            }
            else {
                signalOUT = signalIN * pow(2.05f - abs((3.5f * signalIN) / 2 - 0.8f), 0.8f);
            }

            signalOUT *= (1 / (log(_input + 1) + 1));

            outputSmoothed = outputSmoothed - 0.004 * (outputSmoothed - _output);
            signalOUT *= outputSmoothed;

            blockOutput.setSample(channel, sample, signalOUT);

        }
    }

    updateFilter(); 
    _lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(blockOutput));

    oversampling->processSamplesDown(blockInput);

    //_distortionModule.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool TascloneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TascloneAudioProcessor::createEditor()
{
    //return new TascloneAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void TascloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TascloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TascloneAudioProcessor();
}
