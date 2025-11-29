#pragma once

#include <JuceHeader.h>

#include "../AudioPlayerContext.h"

class ControlBar : public juce::Component, public juce::ChangeListener
{
public:
    ControlBar(AudioPlayerContext& ctx);
    ~ControlBar();

    // Override juce::Component
    void paint(juce::Graphics&) override;
    void resized() override;

    // Override juce::ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source);

private:
    void onClickPlayAndPause();
    void onClickStop();
    void onClickLoad();

    juce::String convertSec2Min(float seconds);

    juce::TextButton playAndPause, stop, load;
    juce::Slider progressSlider;
    juce::Label currentLengthLabel, totalLengthLabel;
    std::unique_ptr<juce::FileChooser> chooser;
    AudioPlayerContext& ctx;
};