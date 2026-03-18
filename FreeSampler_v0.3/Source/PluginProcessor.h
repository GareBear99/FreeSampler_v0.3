#pragma once

#include <JuceHeader.h>
#include "SamplerVoice.h"

class FreeSamplerAudioProcessor final : public juce::AudioProcessor
{
public:
    FreeSamplerAudioProcessor();
    ~FreeSamplerAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    bool loadSampleFromFile(const juce::File& file);
    juce::String getLoadedSampleName() const;
    bool hasLoadedSample() const noexcept { return loadedFileValid; }

    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::Synthesiser synth;
    juce::AudioFormatManager formatManager;
    juce::String loadedSampleName = "No sample loaded";
    juce::File loadedSampleFile;
    bool loadedFileValid = false;

    void rebuildSynthSound(juce::AudioBuffer<float> buffer, double sampleRate, int rootNote);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreeSamplerAudioProcessor)
};
