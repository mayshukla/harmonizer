/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PitchDetector.h"
#include "PhaseVocoder.h"

//==============================================================================
/**
*/
class HarmonizerjuceAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    HarmonizerjuceAudioProcessor();
    ~HarmonizerjuceAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // HarmonizerSynthesiserVoice needs this information.
    float getCurrentPitch() { return currentPitch; }
    const float *getInputBuffer() { return inputBuffer; }
    int getInputBufferSize() { return inputBufferSize; }
    int getExpectedBufferSize() { return expectedBufferSize; }

private:
    PitchDetector *pitchDetector = nullptr;
    float currentPitch = 440;
    const float *inputBuffer;
    int inputBufferSize;
    int expectedBufferSize;

    static constexpr int numVoices = 4;
    Synthesiser synthesiser;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonizerjuceAudioProcessor)
};
