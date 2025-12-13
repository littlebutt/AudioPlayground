#include "ToggleButton.h"

ToggleButton::ToggleButton(juce::String title)
: title(title)
{
    setLookAndFeel(&lnf);
}

ToggleButton::~ToggleButton()
{
    setLookAndFeel(nullptr);
}

void ToggleButton::paint(juce::Graphics& g)
{
    auto toggleBounds = getLocalBounds();
    getLookAndFeel().drawToggleButton(g, *this, false, false);
    g.setColour(COLOR_GREY);
    g.drawText(title, 0, 0, toggleBounds.getWidth(), 14, juce::Justification::bottomLeft);
}

void ToggleButton::LookAndFeel::drawToggleButton(juce::Graphics &g,
                                juce::ToggleButton &toggleButton,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown)
{
    using namespace juce;
    auto enabled = toggleButton.isEnabled();

    auto bounds = toggleButton.getLocalBounds().reduced(15, 15);

    auto toggleState = toggleButton.getToggleState();

    g.setColour(enabled ? (toggleState ? COLOR_RED : COLOR_DARK_RED) : Colours::darkgrey);
    g.fillRoundedRectangle(bounds.toFloat(), 2.f);

    g.setColour(enabled ? COLOR_GREY : Colours::grey);
    g.drawRoundedRectangle(bounds.toFloat(), 2.f, 1.f);
}