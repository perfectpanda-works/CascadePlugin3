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
    private juce::ComboBox::Listener,//�������炪ComboBox::Listener�N���X�̌p��
    private juce::Button::Listener//�������炪Button::Listener�N���X�̌p���ł��B
{
public:
    void comboBoxChanged(juce::ComboBox* combo1) override;//��ComboBox::Listener�N���X��comboBoxChanged�֐����I�[�o�[���C�h���܂��B
    void buttonClicked(juce::Button* mute) override;//��Button::Listener�N���X��buttonClicked�֐����I�[�o�[���C�h���܂��B

    CascadePluginAudioProcessorEditor (CascadePluginAudioProcessor&);
    ~CascadePluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CascadePluginAudioProcessor& audioProcessor;

    //���X���b�g��\������R���{�{�b�N�X�I�u�W�F�N�g��3��`���܂��B
    juce::ComboBox slot1;
    juce::ComboBox slot2;
    juce::ComboBox slot3;

    //�����͉����̃~���[�g�A�����X���b�g�̃o�C�p�X���I���I�t����g�O���{�^���I�u�W�F�N�g���v�S��`���܂��B
    juce::ToggleButton muteInputToggle;
    juce::ToggleButton bypass1;
    juce::ToggleButton bypass2;
    juce::ToggleButton bypass3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CascadePluginAudioProcessorEditor)
};
