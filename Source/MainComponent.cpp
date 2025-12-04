#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
: ctx()
{
    addAndMakeVisible(spectrum);
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
    g.fillAll(juce::Colour(245, 245, 245));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    spectrum.setBounds(bounds.removeFromTop(250));
    controlBar.setBounds(bounds.removeFromBottom(80));

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

    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
        for (auto i = 0; i < bufferToFill.numSamples; ++i)
        {
            ctx.pushNextSampleIntoFifo(channelData[i]);
        }
    }
}