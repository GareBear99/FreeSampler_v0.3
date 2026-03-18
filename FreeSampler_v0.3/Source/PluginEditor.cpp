#include "PluginProcessor.h"
#include "PluginEditor.h"

FreeSamplerAudioProcessorEditor::FreeSamplerAudioProcessorEditor(FreeSamplerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(900, 600);

    titleLabel.setText("FreeSampler", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    subTitleLabel.setText("Minimal sample instrument | MIDI + polyphony + ADSR + loop/start/end", juce::dontSendNotification);
    subTitleLabel.setJustificationType(juce::Justification::centredLeft);
    subTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.65f));
    addAndMakeVisible(subTitleLabel);

    sampleNameLabel.setText(audioProcessor.getLoadedSampleName(), juce::dontSendNotification);
    sampleNameLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(sampleNameLabel);

    dragDropLabel.setText("Drop WAV / AIFF anywhere", juce::dontSendNotification);
    dragDropLabel.setJustificationType(juce::Justification::centredRight);
    dragDropLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.55f));
    addAndMakeVisible(dragDropLabel);

    loadButton.addListener(this);
    addAndMakeVisible(loadButton);

    configureSlider(attackSlider, attackLabel, "Attack");
    configureSlider(decaySlider, decayLabel, "Decay");
    configureSlider(sustainSlider, sustainLabel, "Sustain");
    configureSlider(releaseSlider, releaseLabel, "Release");
    configureSlider(gainSlider, gainLabel, "Gain");
    configureSlider(rootNoteSlider, rootNoteLabel, "Root");
    rootNoteSlider.setRange(0, 127, 1);

    configureSlider(startSlider, startLabel, "Start");
    configureSlider(endSlider, endLabel, "End");
    configureSlider(velocitySlider, velocityLabel, "Velocity");

    configureToggle(loopToggle, "Loop");
    configureToggle(oneShotToggle, "One Shot");

    attackAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "attack", attackSlider);
    decayAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "decay", decaySlider);
    sustainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "sustain", sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "release", releaseSlider);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    rootNoteAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "rootNote", rootNoteSlider);
    startAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "start", startSlider);
    endAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "end", endSlider);
    velocityAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "velocityAmount", velocitySlider);
    loopAttachment = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "loopEnabled", loopToggle);
    oneShotAttachment = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "oneShot", oneShotToggle);

    keyboard.setAvailableRange(24, 96);
    addAndMakeVisible(keyboard);

    startTimerHz(12);
}

FreeSamplerAudioProcessorEditor::~FreeSamplerAudioProcessorEditor()
{
    loadButton.removeListener(this);
}

void FreeSamplerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0f1217));

    auto bounds = getLocalBounds().reduced(12).toFloat();
    g.setColour(juce::Colour(0xff171c24));
    g.fillRoundedRectangle(bounds, 18.0f);

    g.setColour(juce::Colour(0xff7d8cff).withAlpha(0.14f));
    g.drawRoundedRectangle(bounds, 18.0f, 1.3f);

    auto topGlow = getLocalBounds().removeFromTop(110).reduced(16).toFloat();
    g.setColour(juce::Colour(0xff8e9cff).withAlpha(0.08f));
    g.fillRoundedRectangle(topGlow, 16.0f);
}

void FreeSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);

    auto header = area.removeFromTop(72);
    auto titleArea = header.removeFromLeft(420);
    titleLabel.setBounds(titleArea.removeFromTop(34));
    subTitleLabel.setBounds(titleArea);

    auto rightHeader = header;
    loadButton.setBounds(rightHeader.removeFromLeft(140).reduced(6, 8));
    dragDropLabel.setBounds(rightHeader);

    sampleNameLabel.setBounds(area.removeFromTop(26));
    area.removeFromTop(14);

    auto controlPanel = area.removeFromTop(360);
    auto leftPanel = controlPanel.removeFromLeft(controlPanel.getWidth() / 2 - 8);
    controlPanel.removeFromLeft(16);
    auto rightPanel = controlPanel;

    const int gap = 10;

    auto placeGrid = [gap] (juce::Rectangle<int> rect, std::vector<juce::Slider*> sliders)
    {
        const int cols = 3;
        const int rows = 2;
        const int cellW = (rect.getWidth() - gap * (cols - 1)) / cols;
        const int cellH = (rect.getHeight() - gap * (rows - 1)) / rows;

        int index = 0;
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                if (index >= (int) sliders.size())
                    return;

                sliders[(size_t) index++]->setBounds(rect.getX() + col * (cellW + gap),
                                                     rect.getY() + row * (cellH + gap),
                                                     cellW,
                                                     cellH);
            }
        }
    };

    placeGrid(leftPanel, { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider, &gainSlider, &rootNoteSlider });

    auto topRight = rightPanel.removeFromTop(170);
    const int sliderW = (topRight.getWidth() - gap * 2) / 3;
    startSlider.setBounds(topRight.removeFromLeft(sliderW));
    topRight.removeFromLeft(gap);
    endSlider.setBounds(topRight.removeFromLeft(sliderW));
    topRight.removeFromLeft(gap);
    velocitySlider.setBounds(topRight);

    rightPanel.removeFromTop(16);
    auto toggleRow = rightPanel.removeFromTop(30);
    loopToggle.setBounds(toggleRow.removeFromLeft(120));
    toggleRow.removeFromLeft(20);
    oneShotToggle.setBounds(toggleRow.removeFromLeft(140));

    area.removeFromTop(14);
    keyboard.setBounds(area.removeFromTop(110));
}

void FreeSamplerAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button != &loadButton)
        return;

    juce::FileChooser chooser("Load a WAV or AIFF sample", {}, "*.wav;*.aif;*.aiff");
    if (chooser.browseForFileToOpen())
        audioProcessor.loadSampleFromFile(chooser.getResult());
}

bool FreeSamplerAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (isAudioFile(file))
            return true;
    }

    return false;
}

void FreeSamplerAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.isEmpty() || ! isAudioFile(files[0]))
        return;

    audioProcessor.loadSampleFromFile(juce::File(files[0]));
}

void FreeSamplerAudioProcessorEditor::timerCallback()
{
    const auto hasSample = audioProcessor.hasLoadedSample();
    sampleNameLabel.setText(hasSample ? "Loaded Sample: " + audioProcessor.getLoadedSampleName()
                                      : "No sample loaded yet — drag/drop or press Load Sample",
                            juce::dontSendNotification);
}

void FreeSamplerAudioProcessorEditor::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 22);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.88f));
    addAndMakeVisible(label);
}

void FreeSamplerAudioProcessorEditor::configureToggle(juce::ToggleButton& button, const juce::String& text)
{
    button.setButtonText(text);
    button.setColour(juce::ToggleButton::textColourId, juce::Colours::white.withAlpha(0.9f));
    addAndMakeVisible(button);
}

bool FreeSamplerAudioProcessorEditor::isAudioFile(const juce::String& file) const
{
    return file.endsWithIgnoreCase(".wav")
        || file.endsWithIgnoreCase(".aif")
        || file.endsWithIgnoreCase(".aiff");
}
