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
class CascadePluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
    private juce::ComboBox::Listener,//★こちらがComboBox::Listenerクラスの継承
    private juce::Button::Listener//★こちらがButton::Listenerクラスの継承です。
{
public:
    void comboBoxChanged(juce::ComboBox* combo1) override;//★ComboBox::ListenerクラスのcomboBoxChanged関数をオーバーライドします。
    void buttonClicked(juce::Button* mute) override;//★Button::ListenerクラスのbuttonClicked関数をオーバーライドします。

    CascadePluginAudioProcessorEditor (CascadePluginAudioProcessor&);
    ~CascadePluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CascadePluginAudioProcessor& audioProcessor;

    //★スロットを表現するコンボボックスオブジェクトを3つ定義します。
    juce::ComboBox slot1;
    juce::ComboBox slot2;
    juce::ComboBox slot3;

    //★入力音声のミュート、書くスロットのバイパスをオンオフするトグルボタンオブジェクトを計４個定義します。
    juce::ToggleButton muteInputToggle;
    juce::ToggleButton bypass1;
    juce::ToggleButton bypass2;
    juce::ToggleButton bypass3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CascadePluginAudioProcessorEditor)
};
