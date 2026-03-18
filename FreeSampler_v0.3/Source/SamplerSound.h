#pragma once

#include <JuceHeader.h>

class SamplerSound final : public juce::SynthesiserSound
{
public:
    SamplerSound(juce::AudioBuffer<float> loadedBuffer,
                 double sourceSampleRate,
                 int midiRootNote)
        : buffer(std::move(loadedBuffer)),
          sampleRate(sourceSampleRate),
          rootNote(juce::jlimit(0, 127, midiRootNote))
    {
    }

    bool appliesToNote(int) override    { return true; }
    bool appliesToChannel(int) override { return true; }

    const juce::AudioBuffer<float>& getBuffer() const noexcept { return buffer; }
    double getSampleRate() const noexcept { return sampleRate; }
    int getRootNote() const noexcept { return rootNote; }
    int getLength() const noexcept { return buffer.getNumSamples(); }

private:
    juce::AudioBuffer<float> buffer;
    double sampleRate = 44100.0;
    int rootNote = 60;
};
