#pragma once

#include <JuceHeader.h>

#include "../Globals.h"

class ToggleButton : public juce::ToggleButton
{
public:
    ToggleButton(juce::String title);
    ~ToggleButton();

    // Override juce::ToggleButton
    void paint(juce::Graphics& g) override;
private:
    class LookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawToggleButton(juce::Graphics &g,
                                juce::ToggleButton &toggleButton,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown) override;
    };
    LookAndFeel lnf;
    juce::String title;
};