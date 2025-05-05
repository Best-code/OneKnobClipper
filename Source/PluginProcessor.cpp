/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OneKnobClipAudioProcessor::OneKnobClipAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
}

void OneKnobClipAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.proThreshold = threshold.getValue();
}

/*
    BUTTON STUFF HERE
*/


void OneKnobClipAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    if(button == &delta)
        audioProcessor.delta = !audioProcessor.delta;
    else if(button == &bypass)
        audioProcessor.bypass = !audioProcessor.bypass;
    else if (button == &hardBtn){
        hardBtn.setToggleState(true, juce::NotificationType::dontSendNotification);
        analogBtn.setToggleState(false, juce::NotificationType::dontSendNotification);
        audioProcessor.hardClipMode = true;
//        audioProcessor.proThreshold -= audioProcessor.softDistance;
    }
    else if (button == &analogBtn){
        hardBtn.setToggleState(false, juce::NotificationType::dontSendNotification);
        analogBtn.setToggleState(true, juce::NotificationType::dontSendNotification);
        audioProcessor.hardClipMode = false;
//        audioProcessor.proThreshold += audioProcessor.softDistance;
    }
}


OneKnobClipAudioProcessor::~OneKnobClipAudioProcessor()
{
}

//==============================================================================
const juce::String OneKnobClipAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobClipAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool OneKnobClipAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool OneKnobClipAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double OneKnobClipAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobClipAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobClipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobClipAudioProcessor::setCurrentProgram (int index){}

const juce::String OneKnobClipAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobClipAudioProcessor::changeProgramName (int index, const juce::String& newName){}

//==============================================================================
void OneKnobClipAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void OneKnobClipAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobClipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

float OneKnobClipAudioProcessor::hardClip(float sampleIn){
    return juce::jlimit(-1.f, 1.f, sampleIn);
}

float OneKnobClipAudioProcessor::analogClip(float sampleIn) {
    const float clipThreshold = juce::Decibels::decibelsToGain(0.f);              // 0 dB or user-defined
    const float softThreshold = juce::Decibels::decibelsToGain(0.f - softDistance); // e.g. -6 dB

    const float absIn = std::abs(sampleIn);
    float shaped = sampleIn;

    if (absIn < softThreshold) {
        // Clean region – no clipping
        shaped = sampleIn;
    }
    else if (absIn < clipThreshold) {
        // Soft clip region
        const float normalized = (absIn - softThreshold) / (clipThreshold - softThreshold);
        const float curved = std::tanh(normalized * 2.5f); // shaping curve
        shaped = (sampleIn / absIn) * juce::jmap(curved, 0.0f, 1.0f, softThreshold, clipThreshold);
    }
    else {
        // Hard clip region
        shaped = juce::jlimit(-clipThreshold, clipThreshold, sampleIn);
    }

    // Ensure output isn't louder than input
    shaped = juce::jlimit(-std::abs(sampleIn), std::abs(sampleIn), shaped);
    shaped = juce::jlimit(-1.0f, 1.0f, shaped); // safety clip

    return shaped;
}

void OneKnobClipAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if(bypass) return;
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    
    float inputGain =  juce::Decibels::decibelsToGain(-proThreshold);
    float outputGain = 1.f/inputGain;
    float deltaSample;
    
    // Calculate gain steps
    float inputGainStep = (inputGain - lastInputGain) / buffer.getNumSamples();
    float outputGainStep = (outputGain - (1.0f / lastInputGain)) / buffer.getNumSamples();

    // === Get clipping threshold from dB slider ===
    // Assuming you have a Slider or parameter called clipperSlider or clipThresholdDbParam
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        int numSamples = buffer.getNumSamples();
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Gain Ramp - No Clicks when moving
            inputGain = lastInputGain + inputGainStep * sample;
            outputGain = (1.0f / lastInputGain) + outputGainStep * sample;

            
            float processedSample = channelData[sample] * inputGain;
            if(hardClipMode)
                processedSample = hardClip(processedSample); // Hard Clip
            else
                processedSample = analogClip(processedSample); // Analog Clip
            processedSample *= outputGain;
            
            
            deltaSample = channelData[sample] - processedSample;
            if(delta)
                channelData[sample] = deltaSample;
            else
                channelData[sample] = processedSample;
        }
    }
    lastInputGain = inputGain;
}

//==============================================================================
bool OneKnobClipAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OneKnobClipAudioProcessor::createEditor()
{
    return new OneKnobClipAudioProcessorEditor (*this);
}

//==============================================================================
void OneKnobClipAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OneKnobClipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobClipAudioProcessor();
}
