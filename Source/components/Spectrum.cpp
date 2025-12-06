#include "Spectrum.h"

Spectrum::Spectrum(AudioPlayerContext& ctx)
: ctx(ctx)
{
    auto fftSize = 1 << 11;
    ctx.leftMonoBuffer.setSize(1, fftSize);
    ctx.rightMonoBuffer.setSize(1, fftSize);
    ctx.leftForwardFFT = std::make_unique<juce::dsp::FFT>(11);
    ctx.leftWindow = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);
    ctx.leftChannelFFTData.clear();
    ctx.leftChannelFFTData.resize(fftSize * 2, 0);

    ctx.leftFFTDataFifo.prepare(ctx.leftChannelFFTData.size());

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

    auto leftChannelFFTPath = ctx.leftChannelFFTPath;
    leftChannelFFTPath.applyTransform(AffineTransform().translation(left, top));
    
    g.setColour(Colour(97u, 18u, 167u));
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

}

void Spectrum::resized()
{
}

void Spectrum::timerCallback()
{
    process(getLocalBounds().toFloat(), ctx.sampleRate);
    repaint();
}

void Spectrum::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    while (ctx.leftChannelfftDataFifo.getNumAvailableForReading() > 0)
    {
        if (ctx.leftChannelfftDataFifo.pull(tempIncomingBuffer))
        {
            auto size = tempIncomingBuffer.getNumSamples();

            juce::FloatVectorOperations::copy(ctx.leftMonoBuffer.getWritePointer(0, 0),
                                              ctx.leftMonoBuffer.getReadPointer(0, size),
                                              ctx.leftMonoBuffer.getNumSamples() - size);

            juce::FloatVectorOperations::copy(ctx.leftMonoBuffer.getWritePointer(0, ctx.leftMonoBuffer.getNumSamples() - size),
                                              tempIncomingBuffer.getReadPointer(0, 0),
                                              size);
            ctx.produceFFTDataForRendering(ctx.leftMonoBuffer, -48.f);
        }
    }
    
    const auto fftSize = 1 << 11;
    const auto binWidth = sampleRate / double(fftSize);

    while (ctx.leftFFTDataFifo.getNumAvailableForReading() > 0)
    {
        std::vector<float> fftData;
        if (ctx.leftFFTDataFifo.pull(fftData))
        {
            ctx.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }
    
    while (ctx.leftPathFifo.getNumAvailableForReading() > 0)
    {
        ctx.leftPathFifo.pull(ctx.leftChannelFFTPath);
    }
    // for right
}