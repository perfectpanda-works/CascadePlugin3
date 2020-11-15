/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CascadePluginAudioProcessorEditor::CascadePluginAudioProcessorEditor (CascadePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //★コンボボックスのslot1~3を初期化します。
    slot1.addItemList({ "Empty", "Oscillator", "Gain", "Filter" }, 1);
    slot1.setSelectedId(1);
    addAndMakeVisible(&slot1);//コンボボックスの可視化です。
    slot1.addListener(this);//リスナー関数に紐づけます。

    slot2.addItemList({ "Empty", "Oscillator", "Gain", "Filter" }, 1);
    slot2.setSelectedId(1);
    addAndMakeVisible(&slot2);
    slot2.addListener(this);

    slot3.addItemList({ "Empty", "Oscillator", "Gain", "Filter" }, 1);
    slot3.setSelectedId(1);
    addAndMakeVisible(&slot3);
    slot3.addListener(this);

    //★トグルボタンを初期化します
    addAndMakeVisible(&muteInputToggle);//ToggleButtonを可視化しています。
    muteInputToggle.addListener(this);
    addAndMakeVisible(&bypass1);
    bypass1.addListener(this);
    addAndMakeVisible(&bypass2);
    bypass2.addListener(this);
    addAndMakeVisible(&bypass3);
    bypass3.addListener(this);

    setSize(400, 300);
}

CascadePluginAudioProcessorEditor::~CascadePluginAudioProcessorEditor()
{
}

//==============================================================================
void CascadePluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //★以下をコメントアウトしました。
    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CascadePluginAudioProcessorEditor::resized()
{
    //★GUIの配置をします。
    muteInputToggle.setBounds(40, 10, 30, 30);
    slot1.setBounds(40, 50, 200, 30);
    slot2.setBounds(40, 90, 200, 30);
    slot3.setBounds(40, 130, 200, 30);
    bypass1.setBounds(40, 170, 30, 30);
    bypass2.setBounds(40, 210, 30, 30);
    bypass3.setBounds(40, 250, 30, 30);
}

//コンボボックスのリスナー関数です。★
void CascadePluginAudioProcessorEditor::comboBoxChanged(juce::ComboBox* combo)
{
    audioProcessor.setSlot1(slot1.getSelectedId() - 1);//processorSlot1の値を変更します。★
    audioProcessor.setSlot2(slot2.getSelectedId() - 1);
    audioProcessor.setSlot3(slot3.getSelectedId() - 1);
}

//ボタンのリスナー関数です。
void CascadePluginAudioProcessorEditor::buttonClicked(juce::Button* b)
{
    audioProcessor.setMute(muteInputToggle.getToggleState());
    audioProcessor.setBypass1(bypass1.getToggleState());
    audioProcessor.setBypass2(bypass2.getToggleState());
    audioProcessor.setBypass3(bypass3.getToggleState());
}