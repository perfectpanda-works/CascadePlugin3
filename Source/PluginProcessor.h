/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ProcessorBase : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProcessorBase() {}

    //==============================================================================
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override {}

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }

    //==============================================================================
    const juce::String getName() const override { return {}; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }

    //==============================================================================
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorBase)
};

//==============================================================================
class OscillatorProcessor : public ProcessorBase
{
public:
    OscillatorProcessor()
    {
        oscillator.setFrequency(440.0f);
        oscillator.initialise([](float x) { return std::sin(x); });
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock) };
        oscillator.prepare(spec);
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        oscillator.process(context);
    }

    void reset() override
    {
        oscillator.reset();
    }

    const juce::String getName() const override { return "Oscillator"; }

private:
    juce::dsp::Oscillator<float> oscillator;
};

class GainProcessor : public ProcessorBase
{
public:
    GainProcessor()
    {
        gain.setGainDecibels(-6.0f);
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        gain.prepare(spec);
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        gain.process(context);
    }

    void reset() override
    {
        gain.reset();
    }

    const juce::String getName() const override { return "Gain"; }

private:
    juce::dsp::Gain<float> gain;
};

//==============================================================================
class FilterProcessor : public ProcessorBase
{
public:
    FilterProcessor() {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 1000.0f);

        juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        filter.prepare(spec);
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }

    void reset() override
    {
        filter.reset();
    }

    const juce::String getName() const override { return "Filter"; }

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};

//==============================================================================
/**
*/
class CascadePluginAudioProcessor  : public juce::AudioProcessor
{
public:
    void setSlot1(int num)//š
    {
        processorSlot1->operator=(num);
    }
    void setSlot2(int num)
    {
        processorSlot2->operator=(num);
    }
    void setSlot3(int num)
    {
        processorSlot3->operator=(num);
    }
    void setMute(bool flg)
    {
        muteInput->operator=(flg);
    }
    void setBypass1(bool flg)
    {
        bypassSlot1->operator=(flg);
    }
    void setBypass2(bool flg)
    {
        bypassSlot2->operator=(flg);
    }
    void setBypass3(bool flg)
    {
        bypassSlot3->operator=(flg);
    }

    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;

    //==============================================================================
    CascadePluginAudioProcessor();
    ~CascadePluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //changed
    void initialiseGraph()
    {
        mainProcessor->clear();

        audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
        audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
        midiInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
        midiOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

        connectAudioNodes();
        connectMidiNodes();
    }

    void updateGraph()
    {
        bool hasChanged = false;

        juce::Array<juce::AudioParameterChoice*> choices{ processorSlot1,
                                                           processorSlot2,
                                                           processorSlot3 };

        juce::Array<juce::AudioParameterBool*> bypasses{ bypassSlot1,
                                                          bypassSlot2,
                                                          bypassSlot3 };

        juce::ReferenceCountedArray<Node> slots;
        slots.add(slot1Node);
        slots.add(slot2Node);
        slots.add(slot3Node);

        for (int i = 0; i < 3; ++i)
        {
            auto& choice = choices.getReference(i);
            auto  slot = slots.getUnchecked(i);

            if (choice->getIndex() == 0)            // [1]
            {
                if (slot != nullptr)
                {
                    mainProcessor->removeNode(slot.get());
                    slots.set(i, nullptr);
                    hasChanged = true;
                }
            }
            else if (choice->getIndex() == 1)       // [2]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Oscillator")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<OscillatorProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 2)       // [3]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Gain")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<GainProcessor>()));
                hasChanged = true;
            }
            else if (choice->getIndex() == 3)       // [4]
            {
                if (slot != nullptr)
                {
                    if (slot->getProcessor()->getName() == "Filter")
                        continue;

                    mainProcessor->removeNode(slot.get());
                }

                slots.set(i, mainProcessor->addNode(std::make_unique<FilterProcessor>()));
                hasChanged = true;
            }
        }

        if (hasChanged)
        {
            for (auto connection : mainProcessor->getConnections())     // [5]
                mainProcessor->removeConnection(connection);

            juce::ReferenceCountedArray<Node> activeSlots;

            for (auto slot : slots)
            {
                if (slot != nullptr)
                {
                    activeSlots.add(slot);                             // [6]

                    slot->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(),
                        getMainBusNumOutputChannels(),
                        getSampleRate(), getBlockSize());
                }
            }

            if (activeSlots.isEmpty())                                  // [7]
            {
                connectAudioNodes();
            }
            else
            {
                for (int i = 0; i < activeSlots.size() - 1; ++i)        // [8]
                {
                    for (int channel = 0; channel < 2; ++channel)
                        mainProcessor->addConnection({ { activeSlots.getUnchecked(i)->nodeID,      channel },
                                                        { activeSlots.getUnchecked(i + 1)->nodeID,  channel } });
                }

                for (int channel = 0; channel < 2; ++channel)           // [9]
                {
                    mainProcessor->addConnection({ { audioInputNode->nodeID,         channel },
                                                    { activeSlots.getFirst()->nodeID, channel } });
                    mainProcessor->addConnection({ { activeSlots.getLast()->nodeID,  channel },
                                                    { audioOutputNode->nodeID,        channel } });
                }
            }

            connectMidiNodes();

            for (auto node : mainProcessor->getNodes())                 // [10]
                node->getProcessor()->enableAllBuses();
        }

        for (int i = 0; i < 3; ++i)
        {
            auto  slot = slots.getUnchecked(i);
            auto& bypass = bypasses.getReference(i);

            if (slot != nullptr)
                slot->setBypassed(bypass->get());
        }

        audioInputNode->setBypassed(muteInput->get());

        slot1Node = slots.getUnchecked(0);
        slot2Node = slots.getUnchecked(1);
        slot3Node = slots.getUnchecked(2);
    }

    void connectAudioNodes()
    {
        for (int channel = 0; channel < 2; ++channel)
            mainProcessor->addConnection({ { audioInputNode->nodeID,  channel },
                                            { audioOutputNode->nodeID, channel } });
    }
    void connectMidiNodes()
    {
        mainProcessor->addConnection({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                        { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
    }

    
    //changed
    juce::StringArray processorChoices{ "Empty", "Oscillator", "Gain", "Filter" };

    std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

    juce::AudioParameterBool* muteInput;

    juce::AudioParameterChoice* processorSlot1;
    juce::AudioParameterChoice* processorSlot2;
    juce::AudioParameterChoice* processorSlot3;

    juce::AudioParameterBool* bypassSlot1;
    juce::AudioParameterBool* bypassSlot2;
    juce::AudioParameterBool* bypassSlot3;

    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;

    Node::Ptr slot1Node;
    Node::Ptr slot2Node;
    Node::Ptr slot3Node;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CascadePluginAudioProcessor)
};