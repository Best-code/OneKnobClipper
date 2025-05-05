/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OneKnobClipAudioProcessorEditor  : public juce::AudioProcessorEditor
                                        ,private juce::Slider::Listener
                                        ,private juce::Button::Listener
{
public:
    OneKnobClipAudioProcessorEditor (OneKnobClipAudioProcessor&);
    ~OneKnobClipAudioProcessorEditor() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void paintSlider(juce::Graphics& g, juce::Slider& slider);
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OneKnobClipAudioProcessor& audioProcessor;
    juce::Slider threshold;
    juce::ToggleButton delta;
    juce::ToggleButton hardBtn;
    juce::ToggleButton analogBtn;
    juce::ToggleButton bypass;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobClipAudioProcessorEditor)
};
