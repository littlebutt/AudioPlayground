#include "Spectrum.h"

Spectrum::Spectrum(AudioPlayerContext& ctx)
: ctx(ctx)
{
    auto fftSize = 1 << 11;
    for (auto& channelContext: ctx.channelContexts)
    {
        channelContext.monoBuffer.setSize(1, fftSize);
        channelContext.forwardFFT = std::make_unique<juce::dsp::FFT>(11);
        channelContext.window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);
        channelContext.FFTData.clear();
        channelContext.FFTData.resize(fftSize * 2, 0);
        channelContext.FFTDataFifo.prepare(channelContext.FFTData.size());
    }

    startTimerHz(60);
}

Spectrum::~Spectrum()
{
}

void Spectrum::paint(juce::Graphics& g)
{
    using namespace juce;
    auto bounds = getLocalBounds().reduced(5);
    auto left = bounds.getX();
    auto right = bounds.getRight();
    auto width = bounds.getWidth();
    auto top = bounds.getY();
    auto bottom = bounds.getBottom();
    g.setColour(Colour(51, 51, 51));
    g.drawRoundedRectangle(bounds.toFloat(), 3, 1);

    for (auto freq : freqs)
    {
        auto normX = mapFromLog10(freq, 20.f, 20000.f);
        g.drawVerticalLine(left + width * normX, top, bottom);
    }

    for (auto dB : dBs)
    {
        auto y = jmap(dB, -100.f, 0.f, float(bottom), float(top));
        g.drawHorizontalLine(y, left, right);
    }

    auto leftChannelFFTPath = ctx.channelContexts[0].FFTPath;
    leftChannelFFTPath.applyTransform(AffineTransform().translation(left, top));
    
    g.setColour(COLOR_RED);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

    auto rightChannelFFTPath = ctx.channelContexts[1].FFTPath;
    rightChannelFFTPath.applyTransform(AffineTransform().translation(left, top));
    
    g.setColour(COLOR_DARK_RED);
    g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));

}

void Spectrum::resized()
{
}

void Spectrum::timerCallback()
{
    process(getLocalBounds().reduced(10).toFloat(), ctx.sampleRate);
    repaint();
}

void Spectrum::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    for (int i = 0; i < 2; i++)
    {
        juce::AudioBuffer<float> tempIncomingBuffer;
        while (ctx.channelContexts[i].channelFFTDataFifo.getNumAvailableForReading() > 0)
        {
            if (ctx.channelContexts[i].channelFFTDataFifo.pull(tempIncomingBuffer))
            {
                auto size = tempIncomingBuffer.getNumSamples();

                juce::FloatVectorOperations::copy(ctx.channelContexts[i].monoBuffer.getWritePointer(0, 0),
                                                ctx.channelContexts[i].monoBuffer.getReadPointer(0, size),
                                                ctx.channelContexts[i].monoBuffer.getNumSamples() - size);

                juce::FloatVectorOperations::copy(ctx.channelContexts[i].monoBuffer.getWritePointer(0, ctx.channelContexts[i].monoBuffer.getNumSamples() - size),
                                              tempIncomingBuffer.getReadPointer(0, 0),
                                              size);
            ctx.produceFFTDataForRendering(ctx.channelContexts[i].monoBuffer, (Channel)i, -100.f);
            }
        }
        const auto fftSize = 1 << 11;
        const auto binWidth = sampleRate / double(fftSize);

        while (ctx.channelContexts[i].FFTDataFifo.getNumAvailableForReading() > 0)
        {
            std::vector<float> FFTData;
            if (ctx.channelContexts[i].FFTDataFifo.pull(FFTData))
            {
                ctx.generatePath(FFTData, (Channel)i, fftBounds, fftSize, binWidth, -100.f);
            }
        }
        while (ctx.channelContexts[i].FFTPathFifo.getNumAvailableForReading() > 0)
        {
            ctx.channelContexts[i].FFTPathFifo.pull(ctx.channelContexts[i].FFTPath);
        }
    }
}