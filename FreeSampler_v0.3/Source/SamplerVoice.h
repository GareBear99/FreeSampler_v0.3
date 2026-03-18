#pragma once

#include <JuceHeader.h>
#include "SamplerSound.h"

class SamplerVoice final : public juce::SynthesiserVoice
{
public:
    explicit SamplerVoice(juce::AudioProcessorValueTreeState& stateToUse)
        : state(stateToUse)
    {
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SamplerSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber,
                   float velocity,
                   juce::SynthesiserSound* sound,
                   int /*currentPitchWheelPosition*/) override
    {
        auto* samplerSound = dynamic_cast<SamplerSound*>(sound);
        if (samplerSound == nullptr)
            return;

        currentSound = samplerSound;
        sourceSamplePosition = 0.0;

        const auto noteOffset = midiNoteNumber - samplerSound->getRootNote();
        const auto pitchRatio = std::pow(2.0, noteOffset / 12.0);
        const auto sourceRate = samplerSound->getSampleRate();
        const auto targetRate = getSampleRate() > 0.0 ? getSampleRate() : 44100.0;
        pitchDelta = (sourceRate / targetRate) * pitchRatio;

        const auto velocityAmount = *state.getRawParameterValue("velocityAmount");
        currentVelocityGain = juce::jmap(velocityAmount, 1.0f, juce::jlimit(0.0f, 1.0f, velocity));

        adsr.setSampleRate(targetRate);
        updateAdsrParameters();
        adsr.noteOn();
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (*state.getRawParameterValue("oneShot") > 0.5f)
            return;

        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            adsr.reset();
            currentSound = nullptr;
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample,
                         int numSamples) override
    {
        if (currentSound == nullptr)
            return;

        updateAdsrParameters();

        const auto& src = currentSound->getBuffer();
        const auto totalSamples = src.getNumSamples();
        if (totalSamples <= 1)
            return;

        const auto gain = *state.getRawParameterValue("gain");
        const auto loopEnabled = *state.getRawParameterValue("loopEnabled") > 0.5f;
        const auto oneShot = *state.getRawParameterValue("oneShot") > 0.5f;

        auto startNorm = *state.getRawParameterValue("start");
        auto endNorm = *state.getRawParameterValue("end");
        if (endNorm <= startNorm + 0.001f)
            endNorm = juce::jmin(1.0f, startNorm + 0.001f);

        const auto startPos = juce::jlimit(0, totalSamples - 1, static_cast<int>(std::floor(startNorm * static_cast<float>(totalSamples - 1))));
        const auto endPos = juce::jlimit(startPos + 1, totalSamples, static_cast<int>(std::floor(endNorm * static_cast<float>(totalSamples))));

        if (sourceSamplePosition < static_cast<double>(startPos))
            sourceSamplePosition = static_cast<double>(startPos);

        for (int i = 0; i < numSamples; ++i)
        {
            auto pos = static_cast<int>(sourceSamplePosition);
            auto alpha = static_cast<float>(sourceSamplePosition - static_cast<double>(pos));

            if (pos >= endPos - 1)
            {
                if (loopEnabled)
                {
                    sourceSamplePosition = static_cast<double>(startPos);
                    pos = startPos;
                    alpha = 0.0f;
                }
                else
                {
                    adsr.noteOff();

                    if (oneShot)
                    {
                        clearCurrentNote();
                        adsr.reset();
                        currentSound = nullptr;
                        break;
                    }
                }
            }

            const auto nextPos = juce::jmin(pos + 1, endPos - 1);
            auto leftA = src.getSample(0, juce::jlimit(0, totalSamples - 1, pos));
            auto leftB = src.getSample(0, juce::jlimit(0, totalSamples - 1, nextPos));
            auto sampleValue = juce::jmap(alpha, leftA, leftB);

            if (src.getNumChannels() > 1)
            {
                const auto rightA = src.getSample(1, juce::jlimit(0, totalSamples - 1, pos));
                const auto rightB = src.getSample(1, juce::jlimit(0, totalSamples - 1, nextPos));
                sampleValue = 0.5f * (sampleValue + juce::jmap(alpha, rightA, rightB));
            }

            sampleValue *= adsr.getNextSample() * currentVelocityGain * gain;

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                outputBuffer.addSample(channel, startSample + i, sampleValue);

            sourceSamplePosition += pitchDelta;

            if (! adsr.isActive())
            {
                clearCurrentNote();
                currentSound = nullptr;
                break;
            }
        }
    }

private:
    void updateAdsrParameters()
    {
        juce::ADSR::Parameters params;
        params.attack = *state.getRawParameterValue("attack");
        params.decay = *state.getRawParameterValue("decay");
        params.sustain = *state.getRawParameterValue("sustain");
        params.release = *state.getRawParameterValue("release");
        adsr.setParameters(params);
    }

    juce::AudioProcessorValueTreeState& state;
    SamplerSound* currentSound = nullptr;
    juce::ADSR adsr;
    double sourceSamplePosition = 0.0;
    double pitchDelta = 1.0;
    float currentVelocityGain = 1.0f;
};
