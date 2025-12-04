#pragma once

#include <JuceHeader.h>

#include "Globals.h"

class AudioPlayerContext
{
public:
    AudioPlayerContext();
    ~AudioPlayerContext();

    void pushNextSampleIntoFifo(float sample) noexcept;

    juce::String songName;
    TransportState state;
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    int totalLengthSec;
    int currentLengthSec;

    juce::dsp::FFT forwardFFT = { 10 };
    std::array<float, 1 << 10> fifo;
    std::array<float, 1 << 11> fftData;
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
};