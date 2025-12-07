#include "AudioPlayerContext.h"

AudioPlayerContext::AudioPlayerContext()
{
}

AudioPlayerContext::~AudioPlayerContext()
{
}

void AudioPlayerContext::pushNextSampleIntoFifo(float leftChannelSample, float rightChannelSample)
{
    for (int i = 0; i < 2; i++)
    {
        auto& channelContext = channelContexts[i];
        if (channelContext.channelFifoIndex == channelContext.channelBuffer.getNumSamples())
        {
            auto ok = channelContext.channelFFTDataFifo.push(channelContext.channelBuffer);
            juce::ignoreUnused(ok);
            channelContext.channelFifoIndex = 0;
        }
        if (i == 0)
        {
           channelContext.channelBuffer.setSample(0, channelContext.channelFifoIndex, leftChannelSample); 
        }
        else
        {
            channelContext.channelBuffer.setSample(0, channelContext.channelFifoIndex, rightChannelSample); 
        }
        
        ++ channelContext.channelFifoIndex;
    }
}

void AudioPlayerContext::produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, Channel ch, const float negativeInfinity)
{
    const auto fftSize = 1 << 11;

    channelContexts[ch].FFTData.assign(channelContexts[ch].FFTData.size(), 0);
    auto* readIndex = audioData.getReadPointer(0);
    std::copy(readIndex, readIndex + fftSize, channelContexts[ch].FFTData.begin());
        
    // first apply a windowing function to our data
    channelContexts[ch].window->multiplyWithWindowingTable(channelContexts[ch].FFTData.data(), fftSize);
        
    // then render our FFT data..
    channelContexts[ch].forwardFFT->performFrequencyOnlyForwardTransform(channelContexts[ch].FFTData.data());
        
    int numBins = (int)fftSize / 2;
        
    //normalize the fft values.
    for( int i = 0; i < numBins; ++i )
    {
        auto v = channelContexts[ch].FFTData[i];
        if( !std::isinf(v) && !std::isnan(v) )
        {
            v /= float(numBins);
        }
        else
        {
            v = 0.f;
        }
        channelContexts[ch].FFTData[i] = v;
    }
        
    //convert them to decibels
    for( int i = 0; i < numBins; ++i )
    {
        channelContexts[ch].FFTData[i] = juce::Decibels::gainToDecibels(channelContexts[ch].FFTData[i], negativeInfinity);
    }
        
    channelContexts[ch].FFTDataFifo.push(channelContexts[ch].FFTData);
}

void AudioPlayerContext::generatePath(const std::vector<float>& renderData, Channel ch, juce::Rectangle<float> fftBounds, int fftSize, float binWidth, float negativeInfinity)
{
    auto top = fftBounds.getY();
    auto bottom = fftBounds.getHeight();
    auto width = fftBounds.getWidth();
    int numBins = (int)fftSize / 2;

    juce::Path p;
    p.preallocateSpace(3 * (int)fftBounds.getWidth());

    auto map = [bottom, top, negativeInfinity](float v)
    {
        return juce::jmap(v,
                        negativeInfinity, 0.f,
                        float(bottom+10),   top);
    };

    auto y = map(renderData[0]);

    if (std::isnan(y) || std::isinf(y))
        y = bottom;
        
    p.startNewSubPath(0, y);

    const int pathResolution = 2; //you can draw line-to's every 'pathResolution' pixels.

    for (int binNum = 1; binNum < numBins; binNum += pathResolution)
    {
        y = map(renderData[binNum]);

        if (!std::isnan(y) && !std::isinf(y))
        {
            auto binFreq = binNum * binWidth;
            auto normalizedBinX = juce::mapFromLog10(binFreq, 20.f, 20000.f);
            int binX = std::floor(normalizedBinX * width);
            p.lineTo(binX, y);
        }
    }

    channelContexts[ch].FFTPathFifo.push(p);
}