#include "ControlBar.h"

ControlBar::ControlBar(AudioPlayerContext& ctx)
: ctx(ctx),
thumbnailCache(1),
thumbnail(512, ctx.audioFormatManager, thumbnailCache)
{

    componentImage = juce::ImageCache::getFromMemory(BinaryData::components_png, BinaryData::components_pngSize);
    
    playAndPause.setTitle("Play");
    playAndPause.setImages(false, true, true,
        drawButtonImage(0, 204, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 204, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 204, 36, 36), 1.0f, juce::Colours::transparentBlack);
    playAndPause.onClick = [this]{ onClickPlayAndPause(); };
    addAndMakeVisible(playAndPause);

    stop.setTitle("Stop");
    stop.setImages(false, true, true,
        drawButtonImage(0, 127, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 127, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 127, 36, 36), 1.0f, juce::Colours::transparentBlack);
    stop.onClick = [this]{ onClickStop(); };
    addAndMakeVisible(stop);

    load.setButtonText("Load");
    load.setImages(false, true, true,
        drawButtonImage(88, 163, 25, 25), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(88, 189, 25, 25), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(88, 189, 25, 25), 1.0f, juce::Colours::transparentBlack);
    load.onClick = [this]{ onClickLoad(); };
    addAndMakeVisible(load);

    songNameLabel.setText("", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(songNameLabel);

    currentLengthLabel.setText("00:00", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(currentLengthLabel);

    totalLengthLabel.setText("00:00", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(totalLengthLabel);

    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.setRange(0.0, 1, 0.0);
    progressSlider.setLookAndFeel(&progressSliderLnF);
    progressSlider.onDragStart = [this] { isSeeking.store(true); };
    progressSlider.onDragEnd = [this] { isSeeking.store(false); };
    progressSlider.onValueChange = [this] {
        this->ctx.transportSource.setPosition(progressSlider.getValue());
    };
    addAndMakeVisible(progressSlider);

    startTimerHz (30);
}

ControlBar::~ControlBar()
{
    progressSlider.setLookAndFeel(nullptr);
}

void ControlBar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(36, 36, 36));

    auto track = progressSlider.getLookAndFeel()
            .getSliderLayout(progressSlider).sliderBounds + progressSlider.getPosition();
    auto thumbnailBound = track.withHeight(progressSlider.getHeight());
    g.setColour(juce::Colour(51, 51, 51));
    thumbnail.drawChannel(g,
        thumbnailBound,
        0.0,
        thumbnail.getTotalLength(),
        1,
        1.0f); 
    thumbnail.drawChannel(g,
        thumbnailBound,
        0.0,
        thumbnail.getTotalLength(),
        0,
        1.0f); 
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
    progressFlexBox.items.add(juce::FlexItem(songNameLabel).withFlex(20));
    progressFlexBox.items.add(juce::FlexItem(progressSlider).withFlex(40));
    progressFlexBox.items.add(juce::FlexItem(labelFlexBox).withFlex(20));

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.items.add(juce::FlexItem(playAndPause).withFlex(10));
    flexBox.items.add(juce::FlexItem(stop).withFlex(10));
    flexBox.items.add(juce::FlexItem(progressFlexBox).withFlex(70));
    flexBox.items.add(juce::FlexItem(load).withFlex(10));
    flexBox.performLayout(bounds);
}

void ControlBar::timerCallback()
{
    if (ctx.state == PLAYING && isSeeking == false)
    {
        progressSlider.setValue(ctx.transportSource.getCurrentPosition(), juce::NotificationType::dontSendNotification);
    }
    currentLengthLabel.setText(convertSec2Min(progressSlider.getValue()), juce::NotificationType::dontSendNotification);
    repaint();
}

void ControlBar::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail)
    {
        repaint();
    }
}

void ControlBar::ProgressSliderLnF::drawLinearSlider(juce::Graphics& g,
                                                        int x, int y,
                                                        int width, int height,
                                                        float sliderPos,
                                                        float minSliderPos,
                                                        float maxSliderPos,
                                                        const juce::Slider::SliderStyle style,
                                                        juce::Slider& slider)
{
    auto lineY = y + height / 2;

    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawLine((float)x, (float)lineY,
                (float)(x + width), (float)lineY,
                2.0f);
    g.setColour(juce::Colour(194, 12, 12));
    const int lineWidth = 2;
    int lineX = (int)sliderPos - lineWidth / 2;
    g.fillRect(lineX, 0, lineWidth, slider.getHeight());
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
                songNameLabel.setText(ctx.songName, juce::NotificationType::dontSendNotification);
                progressSlider.setRange(0.f, ctx.totalLengthSec, 0.f);
                totalLengthLabel.setText(convertSec2Min(ctx.totalLengthSec), juce::NotificationType::dontSendNotification);
                thumbnail.setSource(new juce::FileInputSource(file));
            }
        }
    });

}

void ControlBar::onClickStop()
{
    ctx.state = STOPPED;
    playAndPause.setTitle("Play");
    playAndPause.setImages(false, true, true,
        drawButtonImage(0, 204, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 204, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 204, 36, 36), 1.0f, juce::Colours::transparentBlack);
    ctx.transportSource.setPosition(0.f);
    ctx.transportSource.stop();
    progressSlider.setValue(0.f, juce::NotificationType::dontSendNotification);
    currentLengthLabel.setText("00:00", juce::NotificationType::dontSendNotification);
    load.setEnabled(true);
}

void ControlBar::onClickPlayAndPause()
{
    if (ctx.state == STOPPED || ctx.state == PAUSED)
    {
        playAndPause.setTitle("Pause");
        playAndPause.setImages(false, true, true,
        drawButtonImage(0, 165, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 165, 36, 36), 1.0f, juce::Colours::transparentBlack,
        drawButtonImage(40, 165, 36, 36), 1.0f, juce::Colours::transparentBlack);
        ctx.state = PLAYING;
        ctx.transportSource.start();
    }
    else
    {
        playAndPause.setTitle("Play");
        playAndPause.setImages(false, true, true,
            drawButtonImage(0, 204, 36, 36), 1.0f, juce::Colours::transparentBlack,
            drawButtonImage(40, 204, 36, 36), 1.0f, juce::Colours::transparentBlack,
            drawButtonImage(40, 204, 36, 36), 1.0f, juce::Colours::transparentBlack);
        ctx.state = PAUSED;
        ctx.transportSource.stop();
    }
    load.setEnabled(false);

}

juce::String ControlBar::convertSec2Min(float seconds)
{
    int m = (int)seconds / 60;
    int s = (int)seconds % 60;
    return juce::String::formatted("%02d:%02d", m, s);
}

juce::Image ControlBar::drawButtonImage(int x, int y, int width, int height)
{
    juce::Rectangle<int> subArea(x, y, width, height);
    return componentImage.getClippedImage(subArea);
}