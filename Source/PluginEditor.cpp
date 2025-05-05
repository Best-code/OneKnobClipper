/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OneKnobClipAudioProcessorEditor::OneKnobClipAudioProcessorEditor (OneKnobClipAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    setResizable(true, true);
    setResizeLimits(200, 250, 200, 1080);
    setSize(250, 600);
    
    // Slider
    threshold.setSliderStyle(juce::Slider::LinearBarVertical);
    threshold.setRange(-18.0, 0.0, 0.01);
    threshold.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    threshold.setPopupDisplayEnabled(true, false, this);
    threshold.setTextValueSuffix(" dB");
    threshold.setTooltip("Clipper Threshold (dB)");
    threshold.setValue(0);
    threshold.setColour(juce::Slider::thumbColourId, juce::Colours::orange); // Custom thumb color
    threshold.setColour(juce::Slider::trackColourId, juce::Colours::lightblue); // Custom track color
    threshold.setColour(juce::Slider::backgroundColourId, juce::Colours::darkgrey); // Custom background color
    addAndMakeVisible(&threshold);

    // Buttons
    delta.setButtonText("Delta");
    analogBtn.setButtonText("Analog");
    hardBtn.setButtonText("Hard");
    bypass.setButtonText("Bypass");

    // Style buttons
    delta.setColour(juce::TextButton::buttonColourId, juce::Colours::cornflowerblue);
    analogBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::forestgreen);
    hardBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::crimson);
    bypass.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

    // Add buttons
    addAndMakeVisible(delta);
    addAndMakeVisible(analogBtn);
    addAndMakeVisible(hardBtn);
    addAndMakeVisible(bypass);

    // Set toggle state
    hardBtn.setToggleState(true, juce::NotificationType::dontSendNotification);

    // Attach listeners
    threshold.addListener(this);
    delta.addListener(this);
    hardBtn.addListener(this);
    analogBtn.addListener(this);
    bypass.addListener(this);
}

OneKnobClipAudioProcessorEditor::~OneKnobClipAudioProcessorEditor()
{
}

//==============================================================================
void OneKnobClipAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("EZ Clip", 80, getHeight()/2 - 40, 100, 100, juce::Justification::centred, 1);
}

void OneKnobClipAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    threshold.setBounds (20, 30, 60, getHeight() - 60);
    
    analogBtn.setBounds(100, 30, 100, 40);
    hardBtn.setBounds(100, 70, 100, 40);
    delta.setBounds(100, getHeight() - 40, 100, 40);
    bypass.setBounds(100, getHeight() - 80, 100, 40);

}
