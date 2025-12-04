#pragma once

#include <JuceHeader.h>

#include "./AudioPlayerContext.h"
#include "components/ControlBar.h"
#include "components/Spectrum.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    // Override juce::Component
    void paint(juce::Graphics&) override;
    void resized() override;

    // Override juce::AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

private:
    //==============================================================================
    // Your private member variables go here...
    AudioPlayerContext ctx;
    ControlBar controlBar = { ctx };
    Spectrum spectrum = { ctx };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
