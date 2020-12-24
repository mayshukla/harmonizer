/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <string>

//==============================================================================
HarmonizerjuceAudioProcessorEditor::HarmonizerjuceAudioProcessorEditor (HarmonizerjuceAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(pitch);
    pitch.setColour(Label::ColourIds::textColourId, Colours::white);
    pitch.setFont(32.0f);
    pitch.setJustificationType(Justification::centred);
    pitch.setText("pitch", NotificationType::dontSendNotification);
}

HarmonizerjuceAudioProcessorEditor::~HarmonizerjuceAudioProcessorEditor()
{
}

//==============================================================================
void HarmonizerjuceAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

    std::string text = "pitch : ";
    text += std::to_string(processor.getCurrentPitch());
    pitch.setText(text, NotificationType::dontSendNotification);
}

void HarmonizerjuceAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    pitch.setBounds (40, 30, 300, getHeight() - 60);
}
