#include "ControlBar.h"

ControlBar::ControlBar(AudioPlayerContext& ctx)
: ctx(ctx)
{
    
    playAndPause.setButtonText("Play");
    playAndPause.onClick = [this]{ onClickPlayAndPause(); };
    addAndMakeVisible(playAndPause);

    stop.setButtonText("Stop");
    stop.onClick = [this]{ onClickStop(); };
    addAndMakeVisible(stop);

    load.setButtonText("Load");
    load.onClick = [this]{ onClickLoad(); };
    addAndMakeVisible(load);

    currentLengthLabel.setText("00:00", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(currentLengthLabel);

    totalLengthLabel.setText("00:00", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(totalLengthLabel);

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

    juce::FlexBox labelFlexBox;
    labelFlexBox.flexDirection = juce::FlexBox::Direction::row;
    labelFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    labelFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    labelFlexBox.items.add(juce::FlexItem(currentLengthLabel).withWidth(50));
    labelFlexBox.items.add(juce::FlexItem(totalLengthLabel).withWidth(50));

    juce::FlexBox progressFlexBox;
    progressFlexBox.flexDirection = juce::FlexBox::Direction::column;
    progressFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    progressFlexBox.items.add(juce::FlexItem(labelFlexBox).withFlex(50));
    progressFlexBox.items.add(juce::FlexItem(progressSlider).withFlex(50));

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.items.add(juce::FlexItem(playAndPause).withFlex(10));
    flexBox.items.add(juce::FlexItem(stop).withFlex(10));
    flexBox.items.add(juce::FlexItem(progressFlexBox).withFlex(70));
    flexBox.items.add(juce::FlexItem(load).withFlex(10));
    flexBox.performLayout(bounds);
}

void ControlBar::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    
}


void ControlBar::onClickLoad()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
                                                juce::File{},
                                                "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
                        | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file != juce::File{})
        {
            auto* reader = ctx.audioFormatManager.createReaderFor(file);
            if (reader != nullptr)
            {
                auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                ctx.transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                ctx.readerSource.reset(newSource.release());
                ctx.state = STOPPED;
                ctx.currentLengthSec = 0;
                ctx.totalLengthSec = ctx.transportSource.getLengthInSeconds();
                ctx.songName = file.getFileName();
                progressSlider.setRange(0.f, ctx.totalLengthSec, 0.f);
                totalLengthLabel.setText(convertSec2Min(ctx.totalLengthSec), juce::NotificationType::dontSendNotification);
            }
        }
    });

}

void ControlBar::onClickStop()
{
    ctx.state = STOPPED;
    playAndPause.setButtonText("Play");
    ctx.transportSource.setPosition(0.f);
    ctx.transportSource.stop();
}

void ControlBar::onClickPlayAndPause()
{
    if (ctx.state == STOPPED || ctx.state == PAUSED)
    {
        playAndPause.setButtonText("Pause");
        ctx.state = PLAYING;
        ctx.transportSource.start();
    }
    else
    {
        playAndPause.setButtonText("Play");
        ctx.state = PAUSED;
        ctx.transportSource.stop();
    }

}

juce::String ControlBar::convertSec2Min(float seconds)
{
    int m = (int)seconds / 60;
    int s = (int)seconds % 60;
    return juce::String::formatted("%02d:%02d", m, s);
}