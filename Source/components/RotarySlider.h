#pragma once

#include <JuceHeader.h>

#include "../Globals.h"

class RotarySlider : public juce::Slider
{
public:
    struct LabelPos
    {
        float pos;
        juce::String label;
    };
    RotarySlider(juce::String title, juce::Array<RotarySlider::LabelPos>& labels);
    ~RotarySlider();

    // Override juce::Slider
    void paint(juce::Graphics& g) override;

private:
    juce::Rectangle<int> getSliderBounds() const;
    class LookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                float sliderPosProportional, float rotaryStartAngle,
                                float rotaryEndAngle, juce::Slider& slider) override;
    };

    LookAndFeel lnf;
    juce::Array<LabelPos> labels;
    juce::String title;
};