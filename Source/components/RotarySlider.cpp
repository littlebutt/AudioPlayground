#include "RotarySlider.h"

RotarySlider::RotarySlider(juce::String title, juce::Array<RotarySlider::LabelPos>& labels)
: title(title),
labels(labels),
juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
{
    setLookAndFeel(&lnf);
}

RotarySlider::~RotarySlider()
{
    setLookAndFeel(nullptr);
}

void RotarySlider::paint(juce::Graphics& g)
{
    using namespace juce;
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();
    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
                                    sliderBounds.getX(),
                                    sliderBounds.getY(),
                                    sliderBounds.getWidth(),
                                    sliderBounds.getHeight(),
                                    jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                    startAng,
                                    endAng,
                                    *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    g.setColour(Colour(0u, 172u, 1u));
    g.setFont(14);

    auto numChoices = labels.size();
    for( int i = 0; i < numChoices; ++i )
    {
        auto pos = labels[i].pos;
        
        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
        
        auto c = center.getPointOnCircumference(radius + 14 * 0.5f + 1, ang);
        
        Rectangle<float> r;
        auto str = labels[i].label;
        GlyphArrangement arr;
        arr.addLineOfText(g.getCurrentFont(), str, 0, 0);
        auto strWidth = arr.getBoundingBox(0, -1, true).getWidth();
        r.setSize(strWidth, 14);
        r.setCentre(c);
        r.setY(r.getY() + 14);
        
        g.drawFittedText(str, r.toNearestInt(), Justification::centred, 1);
    }
    g.drawText(title, 0, 0, sliderBounds.getWidth(), 14, juce::Justification::bottomLeft);
}

void RotarySlider::LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                                    float sliderPosProportional, float rotaryStartAngle,
                                                    float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);
    auto enabled = slider.isEnabled();

    g.setColour(enabled ? COLOR_RED : Colours::darkgrey);
    g.fillEllipse(bounds);

    g.setColour(enabled ? COLOR_GREY : Colours::grey);
    g.drawEllipse(bounds, 1.f);

    if (auto* rs = dynamic_cast<RotarySlider*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p;
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - 14 * 1.5);
        p.addRoundedRectangle(r, 2.f);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

        g.fillPath(p);

        g.setFont(14);
        auto text = juce::String(rs->getValue());
        GlyphArrangement arr;
        arr.addLineOfText(g.getCurrentFont(), text, 0, 0);
        auto strWidth = arr.getBoundingBox(0, -1, true).getWidth();
        
        r.setSize(strWidth + 4, 14 + 2);
        r.setCentre(bounds.getCentre());
        
        g.setColour(enabled ? Colours::black : Colours::darkgrey);
        g.fillRect(r);
        
        g.setColour(enabled ? Colours::white : Colours::lightgrey);
        g.drawFittedText(text, r.toNearestInt(), Justification::centred, 1);
    }
}

juce::Rectangle<int> RotarySlider::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= 14 * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(2);
    
    return r;
}