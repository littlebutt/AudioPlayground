#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
: ctx()
{
    addAndMakeVisible(controlBar);
    setSize(800, 600);
    ctx.audioFormatManager.registerBasicFormats();
    setAudioChannels(0, 2);
    ctx.state = STOPPED;
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    controlBar.setBounds(bounds.removeFromBottom(100));

}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    ctx.transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::releaseResources()
{
    ctx.transportSource.releaseResources();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (ctx.readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    ctx.transportSource.getNextAudioBlock(bufferToFill);
}