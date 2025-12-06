#pragma once

#include <JuceHeader.h>

#include "../AudioPlayerContext.h"

class Spectrum : public juce::Component, public juce::Timer
{
public:
    Spectrum(AudioPlayerContext& ctx);
    ~Spectrum();

    // Override juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Override juce::Timer
    void timerCallback() override;

private:
    void process(juce::Rectangle<float> fftBounds, double sampleRate);

    AudioPlayerContext& ctx;
    juce::Array<float> freqs = {
        20, 50, 100,
        200, 500, 1000,
        2000, 5000, 10000,
        20000
    };
    juce::Array<float> dBs = {
        -100.f, -80.f, -60.f, -40.f, -20.f, 0.f
    };
};