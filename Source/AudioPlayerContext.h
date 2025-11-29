#pragma once

#include <JuceHeader.h>

#include "Globals.h"

class AudioPlayerContext
{
public:
    AudioPlayerContext();
    ~AudioPlayerContext();

    juce::String songName;
    TransportState state;
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    int totalLengthSec;
    int currentLengthSec;
};