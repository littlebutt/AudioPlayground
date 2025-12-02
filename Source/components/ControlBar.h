#pragma once

#include <JuceHeader.h>

#include "../AudioPlayerContext.h"

class ControlBar : public juce::Component, public juce::Timer, juce::ChangeListener
{
public:
    ControlBar(AudioPlayerContext& ctx);
    ~ControlBar();

    // Override juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Override juce::Timer
    void timerCallback() override;

    // Override juce::ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    void onClickPlayAndPause();
    void onClickStop();
    void onClickLoad();

    juce::String convertSec2Min(float seconds);
    juce::Image drawButtonImage(int x, int y, int width, int height);

    struct ProgressSliderLnF : public juce::LookAndFeel_V4
    {
        void drawLinearSlider(juce::Graphics& g,
                                    int x, int y,
                                    int width, int height,
                                    float sliderPos,
                                    float minSliderPos,
                                    float maxSliderPos,
                                    const juce::Slider::SliderStyle style,
                                    juce::Slider& slider) override;
    };

    juce::ImageButton playAndPause, stop, load;
    juce::Slider progressSlider;
    juce::Label currentLengthLabel, totalLengthLabel, songNameLabel;
    juce::Image componentImage;

    std::unique_ptr<juce::FileChooser> chooser;
    AudioPlayerContext& ctx;
    std::atomic<bool> isSeeking = { false };

    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    ProgressSliderLnF progressSliderLnF;
};