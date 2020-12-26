/*
  ==============================================================================

    HarmonizerSynthesiserVoice.h
    Created: 25 Dec 2020 6:11:08pm
    Author:  ryan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <rubberband/RubberBandStretcher.h>

#include "HarmonizerSound.h"
#include "PluginProcessor.h"

class HarmonizerSynthesiserVoice : public juce::SynthesiserVoice {
public:
    HarmonizerSynthesiserVoice(HarmonizerjuceAudioProcessor &processor);
    ~HarmonizerSynthesiserVoice();

    // Must be called before renderNextBlock
    virtual void setCurrentPlaybackSampleRate(double newRate) override;

    virtual void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;
    virtual void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    virtual bool canPlaySound (juce::SynthesiserSound* sound) override {
        return dynamic_cast<HarmonizerSound*> (sound) != nullptr;
    }
    virtual void stopNote(float velocity, bool allowTailOff) override {}
    virtual void pitchWheelMoved(int newPitchWheelValue) override {}
    virtual void controllerMoved(int controllerNumber, int newControllerValue) override {}

    // Must be called before renderNextBlock
    void setExpectedBufferSize(int size);

private:
    // Reference to the owning AudioProcessor.
    // This is where the voice gets the current pitch.
    HarmonizerjuceAudioProcessor &processor;
    RubberBand::RubberBandStretcher *stretcher = nullptr;

    float *stretcherOutputBuffer = nullptr;
    int bufferSize = -1;

    int midiNoteNumber = 65;
};