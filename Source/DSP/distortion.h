/*
  ==============================================================================

    distortion.h
    Created: 20 Jun 2023 1:58:32pm
    Author:  Flynn, Tarun

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class Distortion {

public:

    Distortion();

    void prepare(juce::dsp::ProcessSpec &spec);

    void reset();

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept {

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        for (size_t channel = 0; channel < numChannels; ++channel) {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i) {
                outputSamples[i] = processSample(inputSamples[i]);
            }
        }
    }

    SampleType processSample(SampleType input) noexcept {

        switch (_model) {

            case DistortionModel::kHard: {

                return hardClip(input);
                break;
            }

            case DistortionModel::kSoftEx3: {

                return softClipEx3(input);
                break;
            }

            case DistortionModel::kSoftEx5: {

                return softClipEx5(input);
                break;
            }

            case DistortionModel::kSoftReciprocal: {

                return softClipReciprocal(input);
                break;
            }

            case DistortionModel::kSaturation: {

                return saturation(input);
                break;
            }
        }

    }

    SampleType hardClip(SampleType input) {

        auto wetSignal = input * _input.getNextValue();

        if (std::abs(wetSignal) >= 1.0) {
            wetSignal = 1.0 / wetSignal;
        }

        auto mix = (1.0 - _mix.getNextValue()) * input + wetSignal * _mix.getNextValue();

        return mix + _output.getNextValue();
    }

    SampleType softClipEx3(SampleType input) {

        return input;
    }

    SampleType softClipEx5(SampleType input) {

        return input;
    }

    SampleType softClipReciprocal(SampleType input) {

        return input;
    }

    SampleType saturation(SampleType input) {
           
        return input;
    }

    enum class DistortionModel {

        kHard,
        kSoftEx3,
        kSoftEx5,
        kSoftReciprocal,
        kSaturation
    };

    void setInput(SampleType newInput);
    void setOutput(SampleType newOutput);
    void setMix(SampleType newMix);

    void setModel(DistortionModel newModel);

private:
    juce::SmoothedValue<float> _input;
    juce::SmoothedValue<float> _output;
    juce::SmoothedValue<float> _mix;

    float _sampleRate = 44100.0f;

    DistortionModel _model = DistortionModel::kHard;
};