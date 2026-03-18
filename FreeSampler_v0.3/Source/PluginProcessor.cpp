#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SamplerSound.h"

FreeSamplerAudioProcessor::FreeSamplerAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    formatManager.registerBasicFormats();

    constexpr int numVoices = 16;
    for (int i = 0; i < numVoices; ++i)
        synth.addVoice(new SamplerVoice(apvts));
}

void FreeSamplerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void FreeSamplerAudioProcessor::releaseResources()
{
}

bool FreeSamplerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void FreeSamplerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* FreeSamplerAudioProcessor::createEditor()
{
    return new FreeSamplerAudioProcessorEditor(*this);
}

void FreeSamplerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    if (loadedFileValid)
        state.setProperty("loadedSamplePath", loadedSampleFile.getFullPathName(), nullptr);
    else
        state.removeProperty("loadedSamplePath", nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void FreeSamplerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (! xml->hasTagName(apvts.state.getType()))
            return;

        auto newState = juce::ValueTree::fromXml(*xml);
        apvts.replaceState(newState);

        const auto storedPath = newState.getProperty("loadedSamplePath").toString();
        if (storedPath.isNotEmpty())
        {
            const juce::File sampleFile(storedPath);
            if (sampleFile.existsAsFile())
                loadSampleFromFile(sampleFile);
        }
    }
}

bool FreeSamplerAudioProcessor::loadSampleFromFile(const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return false;

    juce::AudioBuffer<float> buffer(static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples));
    reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);

    const auto rootNote = static_cast<int>(*apvts.getRawParameterValue("rootNote"));
    rebuildSynthSound(std::move(buffer), reader->sampleRate, rootNote);
    loadedSampleName = file.getFileName();
    loadedSampleFile = file;
    loadedFileValid = true;
    return true;
}

juce::String FreeSamplerAudioProcessor::getLoadedSampleName() const
{
    return loadedSampleName;
}

void FreeSamplerAudioProcessor::rebuildSynthSound(juce::AudioBuffer<float> buffer, double sampleRate, int rootNote)
{
    synth.clearSounds();
    synth.addSound(new SamplerSound(std::move(buffer), sampleRate, rootNote));
}

juce::AudioProcessorValueTreeState::ParameterLayout FreeSamplerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.5f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.5f), 0.20f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.85f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(0.001f, 8.0f, 0.001f, 0.5f), 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", juce::NormalisableRange<float>(0.0f, 1.5f, 0.001f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("rootNote", "Root Note", 0, 127, 60));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("start", "Start", juce::NormalisableRange<float>(0.0f, 0.99f, 0.001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("end", "End", juce::NormalisableRange<float>(0.01f, 1.0f, 0.001f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("velocityAmount", "Velocity Amount", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("loopEnabled", "Loop Enabled", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("oneShot", "One Shot", false));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreeSamplerAudioProcessor();
}
