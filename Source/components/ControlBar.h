#pragma once

#include <JuceHeader.h>

#include "../AudioPlayerContext.h"

class ControlBar : public juce::Component
{
public:
    ControlBar(AudioPlayerContext& ctx);
    ~ControlBar();

    // Override juce::Component
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Path drawPlay() const;
    juce::Path drawPause() const;
    juce::Path drawStop() const;

    juce::ShapeButton playAndPause, stop;
    juce::Slider progressSlider;
    AudioPlayerContext& ctx;

    
};