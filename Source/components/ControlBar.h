#pragma once

#include <JuceHeader.h>

#include "../AudioPlayerContext.h"

class ControlBar : public juce::Component, public juce::Timer
{
public:
    ControlBar(AudioPlayerContext& ctx);
    ~ControlBar();

    // Override juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Override juce::Timer
    void timerCallback() override;

private:
    void onClickPlayAndPause();
    void onClickStop();
    void onClickLoad();

    juce::String convertSec2Min(float seconds);
    juce::Image drawButtonImage(int x, int y, int width, int height);

    juce::ImageButton playAndPause, stop, load;
    juce::Slider progressSlider;
    juce::Label currentLengthLabel, totalLengthLabel, songNameLabel;
    juce::Image componentImage;

    std::unique_ptr<juce::FileChooser> chooser;
    AudioPlayerContext& ctx;
    std::atomic<bool> isSeeking = { false };
};