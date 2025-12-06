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

    void produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, const float negativeInfinity);

    void generatePath(const std::vector<float>& renderData, juce::Rectangle<float> fftBounds, int fftSize, float binWidth, float negativeInfinity);

    juce::String songName;
    TransportState state;
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    int totalLengthSec;
    int currentLengthSec;
    double sampleRate;

    juce::AudioBuffer<float> leftChannelBuffer, rightChannelBuffer;
    int leftChannelFifoIndex = 0, rightChannelFifoIndex = 0;
    Fifo<juce::AudioBuffer<float>> leftChannelfftDataFifo, rightChannelfftDataFifo;

    juce::AudioBuffer<float> leftMonoBuffer, rightMonoBuffer;

    std::vector<float> leftChannelFFTData, rightChannelFFTData;
    Fifo<std::vector<float>> leftFFTDataFifo, rightFFTDataFifo;
    std::unique_ptr<juce::dsp::FFT> leftForwardFFT, rightForwardFFT;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> leftWindow, rightWindow;

    Fifo<juce::Path> leftPathFifo, rightPath;
    juce::Path leftChannelFFTPath, rightChannelFFTPath;
};