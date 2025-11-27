#include "ControlBar.h"

ControlBar::ControlBar(AudioPlayerContext& ctx)
: ctx(ctx),
playAndPause("playAndPause", juce::Colours::grey, juce::Colours::red, juce::Colours::grey),
stop("stop", juce::Colours::grey, juce::Colours::red, juce::Colours::grey)
{
    if (ctx.state == PLAYING)
    {
        playAndPause.setShape(drawPause(), true, true, false);
        playAndPause.setButtonText("Pause");
    }
    else
    {
        playAndPause.setShape(drawPlay(), true, true, false);
        playAndPause.setButtonText("Play");
    }
    addAndMakeVisible(playAndPause);
    
    stop.setShape(drawStop(), true, true, false);
    stop.setButtonText("Stop");
    addAndMakeVisible(stop);

    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.setRange(0.0, 1, 0.0);
    addAndMakeVisible(progressSlider);
}

ControlBar::~ControlBar()
{
}

void ControlBar::paint(juce::Graphics&)
{
}

void ControlBar::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromLeft(10);
    bounds.removeFromRight(10);

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.items.add(juce::FlexItem(playAndPause).withFlex(15));
    flexBox.items.add(juce::FlexItem(stop).withFlex(15));
    flexBox.items.add(juce::FlexItem(progressSlider).withFlex(70));
    flexBox.performLayout(bounds);
}

juce::Path ControlBar::drawPlay() const
{
    juce::Path p;
    p.startNewSubPath(20.0f, 10.0f);
    p.lineTo(80.0f, 50.0f);
    p.lineTo(20.0f, 90.0f);
    p.closeSubPath();
    return p;
}

juce::Path ControlBar::drawPause() const
{
    juce::Path p;

    float barWidth = 20.0f;
    float gap = 10.0f;

    p.addRectangle(20.0f, 10.0f, barWidth, 80.0f);
    p.addRectangle(20.0f + barWidth + gap, 10.0f, barWidth, 80.0f);

    return p;
}

juce::Path ControlBar::drawStop() const
{
    juce::Path p;
    p.addRectangle(20.0f, 20.0f, 60.0f, 60.0f);
    return p;
}