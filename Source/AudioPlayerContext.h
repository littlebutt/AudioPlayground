#pragma once

#include <JuceHeader.h>

#include "Globals.h"
#include "Fifo.h"

class AudioPlayerContext
{
public:
    AudioPlayerContext();
    ~AudioPlayerContext();

    void pushNextSampleIntoFifo(float leftChannelSample, float rightChannelSample);

    void produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, Channel ch, const float negativeInfinity);

    void generatePath(const std::vector<float>& renderData, Channel ch, juce::Rectangle<float> fftBounds, int fftSize, float binWidth, float negativeInfinity);

    juce::String songName;
    TransportState state;
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    int totalLengthSec;
    int currentLengthSec;
    double sampleRate;

    struct ChannelContext
    {
        juce::AudioBuffer<float> channelBuffer;
        int channelFifoIndex = 0;
        Fifo<juce::AudioBuffer<float>> channelFFTDataFifo;

        juce::AudioBuffer<float> monoBuffer;

        std::vector<float> FFTData;
        Fifo<std::vector<float>> FFTDataFifo;
        std::unique_ptr<juce::dsp::FFT> forwardFFT;
        std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

        Fifo<juce::Path> FFTPathFifo;
        juce::Path FFTPath;
    };

    ChannelContext channelContexts[2];

};