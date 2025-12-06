#include "AudioPlayerContext.h"

AudioPlayerContext::AudioPlayerContext()
{
}

AudioPlayerContext::~AudioPlayerContext()
{
}

void AudioPlayerContext::pushNextSampleIntoFifo(float leftChannelSample, float rightChannelSample)
{
    if (leftChannelFifoIndex == leftChannelBuffer.getNumSamples())
    {
        auto ok = leftChannelfftDataFifo.push(leftChannelBuffer);
        juce::ignoreUnused(ok);
        leftChannelFifoIndex = 0;
    }
    leftChannelBuffer.setSample(0, leftChannelFifoIndex, leftChannelSample);
    ++ leftChannelFifoIndex;

    if (rightChannelFifoIndex == rightChannelBuffer.getNumSamples())
    {
        auto ok = rightChannelfftDataFifo.push(rightChannelBuffer);
        juce::ignoreUnused(ok);
        rightChannelFifoIndex = 0;
    }
    rightChannelBuffer.setSample(0, rightChannelFifoIndex, rightChannelSample);
    ++ rightChannelFifoIndex;
}

void AudioPlayerContext::produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, const float negativeInfinity)
{
    const auto fftSize = 1 << 11;
        
    leftChannelFFTData.assign(leftChannelFFTData.size(), 0);
    auto* readIndex = audioData.getReadPointer(0);
    std::copy(readIndex, readIndex + fftSize, leftChannelFFTData.begin());
        
    // first apply a windowing function to our data
    leftWindow->multiplyWithWindowingTable(leftChannelFFTData.data(), fftSize);
        
    // then render our FFT data..
    leftForwardFFT->performFrequencyOnlyForwardTransform(leftChannelFFTData.data());
        
    int numBins = (int)fftSize / 2;
        
    //normalize the fft values.
    for( int i = 0; i < numBins; ++i )
    {
        auto v = leftChannelFFTData[i];
        if( !std::isinf(v) && !std::isnan(v) )
        {
            v /= float(numBins);
        }
        else
        {
            v = 0.f;
        }
        leftChannelFFTData[i] = v;
    }
        
    //convert them to decibels
    for( int i = 0; i < numBins; ++i )
    {
        leftChannelFFTData[i] = juce::Decibels::gainToDecibels(leftChannelFFTData[i], negativeInfinity);
    }
        
    leftFFTDataFifo.push(leftChannelFFTData);

    // for right
}

void AudioPlayerContext::generatePath(const std::vector<float>& renderData, juce::Rectangle<float> fftBounds, int fftSize, float binWidth, float negativeInfinity)
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

    leftPathFifo.push(p);

    // for right
}