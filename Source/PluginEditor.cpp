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
    
    threshold.setSliderStyle(juce::Slider::LinearBarVertical);
    threshold.setRange(-18.0, 0, 0.01);
    threshold.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    threshold.setPopupDisplayEnabled (true, false, this);
    threshold.setTextValueSuffix (" dB");
    threshold.setTooltip("Clipper Threshold (dB)");
    threshold.setValue (0);
    
    addAndMakeVisible(&threshold);
    
    delta.setButtonText("Delta");
    analogBtn.setButtonText("Analog");
    hardBtn.setButtonText("Hard");
    bypass.setButtonText("Bypass");
    hardBtn.setToggleState(true, juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(delta);
    addAndMakeVisible(analogBtn);
    addAndMakeVisible(hardBtn);
    addAndMakeVisible(bypass);

    threshold.addListener(this);
    delta.addListener(this);
    hardBtn.addListener(this);
    analogBtn.addListener(this);
    bypass.addListener(this);

    delta.setBounds(    100, 40 , 100, 40);
    analogBtn.setBounds(100, 80 , 100, 40);
    hardBtn.setBounds(  100, 120, 100, 40);
    bypass.setBounds(  100, 460, 100, 40);

    setSize (200, 600);
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
    g.drawFittedText ("EZ Clipr", 80, 250, 100, 100, juce::Justification::centred, 1);
}

void OneKnobClipAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    threshold.setBounds (20, 30, 60, getHeight() - 60);

}
