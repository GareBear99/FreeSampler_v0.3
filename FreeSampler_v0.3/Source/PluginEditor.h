#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class FreeSamplerAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                             private juce::Button::Listener,
                                             private juce::FileDragAndDropTarget,
                                             private juce::Timer
{
public:
    explicit FreeSamplerAudioProcessorEditor(FreeSamplerAudioProcessor&);
    ~FreeSamplerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void buttonClicked(juce::Button* button) override;
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int, int) override;
    void timerCallback() override;
    void configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);
    void configureToggle(juce::ToggleButton& button, const juce::String& text);
    bool isAudioFile(const juce::String& file) const;

    FreeSamplerAudioProcessor& audioProcessor;

    juce::TextButton loadButton { "Load Sample" };
    juce::Label titleLabel;
    juce::Label subTitleLabel;
    juce::Label sampleNameLabel;
    juce::Label dragDropLabel;
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboard { keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, gainSlider, rootNoteSlider;
    juce::Slider startSlider, endSlider, velocitySlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, gainLabel, rootNoteLabel;
    juce::Label startLabel, endLabel, velocityLabel;
    juce::ToggleButton loopToggle, oneShotToggle;

    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> rootNoteAttachment;
    std::unique_ptr<SliderAttachment> startAttachment;
    std::unique_ptr<SliderAttachment> endAttachment;
    std::unique_ptr<SliderAttachment> velocityAttachment;
    std::unique_ptr<ButtonAttachment> loopAttachment;
    std::unique_ptr<ButtonAttachment> oneShotAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreeSamplerAudioProcessorEditor)
};
