#pragma once

#include <JuceHeader.h>

enum TransportState
{
    STOPPED,
    PLAYING,
    PAUSED
};

enum Channel
{
    LEFT,
    RIGHT
};

const auto COLOR_RED = juce::Colour(194u, 12u, 12u);
const auto COLOR_DARK_RED = juce::Colour(164u, 0u, 17u);
const auto COLOR_GREY = juce::Colour(51u, 51u, 51u);
const auto COLOR_WHITE = juce::Colour(245u, 245u, 245u);