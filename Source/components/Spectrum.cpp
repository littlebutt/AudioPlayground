#include "Spectrum.h"

Spectrum::Spectrum(AudioPlayerContext& ctx)
: ctx(ctx)
{
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

}

void Spectrum::resized()
{
}