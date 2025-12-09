#pragma once

#include <JuceHeader.h>

#include "../Globals.h"
#include "../AudioPlayerContext.h"

class VolumeUnitMeter : public juce::Component, public juce::Timer
{
public:
    VolumeUnitMeter(Channel ch, AudioPlayerContext& ctx);
    ~VolumeUnitMeter();

    // Override juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Override juce::Timer
    void timerCallback() override;

private:
    Channel ch;
    AudioPlayerContext& ctx;
    std::atomic<float>* level = nullptr;
    int rectNum = 9;
};