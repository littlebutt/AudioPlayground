#include "VolumeUnitMeter.h"

VolumeUnitMeter::VolumeUnitMeter(Channel ch, AudioPlayerContext& ctx)
: ch(ch), ctx(ctx)
{
    startTimerHz(60);
}

VolumeUnitMeter::~VolumeUnitMeter()
{
}

void VolumeUnitMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);
    auto unitHeight = bounds.getHeight() * 1.f / rectNum;
    auto peak = ctx.channelContexts[ch].peak.load();
    float norm = peak * 3.0f;
    norm = juce::jlimit(0.0f, 1.0f, norm);
    auto colouredRect = int(rectNum * norm);
    for (int i = 0; i < rectNum; i++)
    {
        if (i < colouredRect)
        {
            g.setColour(COLOR_RED);
            if (i >= rectNum - 3)
            {
                g.setColour(COLOR_DARK_RED);
            }
            if (i >= rectNum - 1)
            {
                g.setColour(COLOR_GREY);
            }
        }
        else
        {
            g.setColour(COLOR_WHITE);
        }
        g.fillRoundedRectangle(bounds.getX(), bounds.getY() + bounds.getHeight() + 5 - int((i + 1) * unitHeight), bounds.getWidth(), unitHeight * 0.8, 3.f);
    }
    g.setColour(COLOR_GREY);
    for (int i = 0; i < rectNum; i++)
    {
        g.drawRoundedRectangle(bounds.getX(), bounds.getY() + bounds.getHeight() + 5 - int((i + 1) * unitHeight), bounds.getWidth(), unitHeight * 0.8, 3.f, 1.f);
    }
}

void VolumeUnitMeter::resized()
{
}

void VolumeUnitMeter::timerCallback()
{
    repaint();
}