#pragma once

#include <JuceHeader.h>

#include "globals.h"

class AudioPlayerContext
{
public:
    AudioPlayerContext();
    ~AudioPlayerContext();

    juce::String songName;
    TransportState state;
    int totalLengthSec;
    int currentLengthSec;
};