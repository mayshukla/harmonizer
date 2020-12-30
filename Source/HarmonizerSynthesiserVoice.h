/*
  ==============================================================================

    HarmonizerSynthesiserVoice.h
    Created: 25 Dec 2020 6:11:08pm
    Author:  ryan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "HarmonizerSound.h"
#include "MultiArray.h"
#include "PluginProcessor.h"

class HarmonizerSynthesiserVoice : public juce::SynthesiserVoice {
public:
    HarmonizerSynthesiserVoice(HarmonizerjuceAudioProcessor &processor);
    ~HarmonizerSynthesiserVoice();

    virtual void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;
    virtual void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    virtual bool canPlaySound (juce::SynthesiserSound* sound) override {
        return dynamic_cast<HarmonizerSound*> (sound) != nullptr;
    }
    virtual void stopNote(float velocity, bool allowTailOff) override;
    virtual void pitchWheelMoved(int newPitchWheelValue) override {}
    virtual void controllerMoved(int controllerNumber, int newControllerValue) override {}

    // Must be called before renderNextBlock
    void prepareToPlay(int sampleRate, int bufferSize, int windowSize, int windowCount, int hopSize);

private:
    // Reference to the owning AudioProcessor.
    // This is where the voice gets the current pitch.
    HarmonizerjuceAudioProcessor &processor;

    int sampleRate = -1;
    int bufferSize = -1;
    int windowSize = -1;
    int windowCount = -1;
    int hopSize = -1;
    int overlapFactor = -1;
    float freqPerBin = -1; // Freq of bin 1

    // Arrays used for intermediate calculations
    MultiArray<float> *actualFreqs = nullptr;
    float *previousPhase = nullptr;
    MultiArray<float> *newMags = nullptr;
    MultiArray<float> *newFreqs = nullptr;
    float *phaseAccum = nullptr;

    int midiNoteNumber = 65;

    // Whether or not voice should be played
    bool voiceOn = false;
};