#include "AudioPlayerContext.h"

AudioPlayerContext::AudioPlayerContext()
{
}

AudioPlayerContext::~AudioPlayerContext()
{
}

void AudioPlayerContext::pushNextSampleIntoFifo(float sample) noexcept
{
    if (fifoIndex == 1 << 11)
    {
        if (!nextFFTBlockReady)
        {
            std::fill(fftData.begin(), fftData.end(), 0.0f);
            std::copy(fifo.begin(), fifo.end(), fftData.begin());
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[(size_t) fifoIndex++] = sample;
}