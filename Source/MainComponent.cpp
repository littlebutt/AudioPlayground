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
    ctx.sampleRate = sampleRate;
    ctx.transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    for (int i = 0; i < 2; i++)
    {
        ctx.channelContexts[i].channelBuffer.setSize(1, samplesPerBlockExpected, false, true, true);
        ctx.channelContexts[i].channelFFTDataFifo.prepare(1, samplesPerBlockExpected);
    }
}

void MainComponent::releaseResources()
{
    ctx.transportSource.releaseResources();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* buffer = bufferToFill.buffer;
    if (ctx.readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    ctx.transportSource.getNextAudioBlock(bufferToFill);
    for (int i = 0; i < buffer->getNumSamples(); ++i)
    {
        ctx.pushNextSampleIntoFifo(buffer->getReadPointer(0)[i], buffer->getReadPointer(1)[i]);
    }
}