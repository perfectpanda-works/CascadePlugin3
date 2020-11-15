/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CascadePluginAudioProcessor::CascadePluginAudioProcessor()
     //changed
     : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
             .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
         mainProcessor(new juce::AudioProcessorGraph()),
    muteInput(new juce::AudioParameterBool("mute", "Mute Input", true)),
    processorSlot1(new juce::AudioParameterChoice("slot1", "Slot 1", processorChoices, 0)),
    processorSlot2(new juce::AudioParameterChoice("slot2", "Slot 2", processorChoices, 0)),
    processorSlot3(new juce::AudioParameterChoice("slot3", "Slot 3", processorChoices, 0)),
    bypassSlot1(new juce::AudioParameterBool("bypass1", "Bypass 1", false)),
    bypassSlot2(new juce::AudioParameterBool("bypass2", "Bypass 2", false)),
    bypassSlot3(new juce::AudioParameterBool("bypass3", "Bypass 3", false))
{
    addParameter(muteInput);

    addParameter(processorSlot1);
    addParameter(processorSlot2);
    addParameter(processorSlot3);

    addParameter(bypassSlot1);
    addParameter(bypassSlot2);
    addParameter(bypassSlot3);
}

CascadePluginAudioProcessor::~CascadePluginAudioProcessor()
{
}

//==============================================================================
const juce::String CascadePluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CascadePluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CascadePluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CascadePluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CascadePluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CascadePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CascadePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CascadePluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CascadePluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void CascadePluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
//changed
void CascadePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate, samplesPerBlock);

    mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

    initialiseGraph();
}

//changed
void CascadePluginAudioProcessor::releaseResources()
{
    mainProcessor->releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
//changed
bool CascadePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}
#endif

//changed
void CascadePluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateGraph();

    mainProcessor->processBlock(buffer, midiMessages);
}

//==============================================================================
bool CascadePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CascadePluginAudioProcessor::createEditor()
{
    return new CascadePluginAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void CascadePluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CascadePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CascadePluginAudioProcessor();
}
