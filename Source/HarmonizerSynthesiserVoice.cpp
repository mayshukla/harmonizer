/*
  ==============================================================================

    HarmonizerSynthesiserVoice.cpp
    Created: 25 Dec 2020 6:11:08pm
    Author:  ryan

  ==============================================================================
*/

#include "HarmonizerSynthesiserVoice.h"

#include <aubio/musicutils.h>

HarmonizerSynthesiserVoice::HarmonizerSynthesiserVoice(HarmonizerjuceAudioProcessor &processor)
    : processor(processor) {
}

HarmonizerSynthesiserVoice::~HarmonizerSynthesiserVoice() {
}

void HarmonizerSynthesiserVoice::prepareToPlay(int sampleRate, int bufferSize, int windowSize, int windowCount, int hopSize) {
    // TODO handle these parameters changing dynamically
    if (this->sampleRate == -1) {
        this->sampleRate = sampleRate;
    }
    if (this->bufferSize == -1) {
        this->bufferSize = bufferSize;
    }
    if (this->windowSize == -1) {
        this->windowSize = windowSize;
    }
    if (this->windowCount == -1) {
        this->windowCount = windowCount;
    }
    if (this->hopSize == -1) {
        this->hopSize = hopSize;
    }
}

void HarmonizerSynthesiserVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) {
    this->midiNoteNumber = midiNoteNumber;
    voiceOn = true;
}

void HarmonizerSynthesiserVoice::stopNote(float velocity, bool allowTailOff) {
    voiceOn = false;
    clearCurrentNote();
}

void HarmonizerSynthesiserVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    float targetFreq = aubio_miditofreq(midiNoteNumber);
    float pitchScaleFactor = targetFreq / processor.getCurrentPitch();
    if (pitchScaleFactor < 0.01 || std::isnan(pitchScaleFactor)) {
        // Weird pitchScaleFactor probably a glitch.
        // Set to 1 to be safe.
        pitchScaleFactor = 1;
    }
    //stretcher->setPitchScale(pitchScaleFactor);
    //const float *inputBuffer = processor.getInputBuffer();
    //stretcher->process(&inputBuffer, processor.getInputBufferSize(), false);
    //stretcher->retrieve(&stretcherOutputBuffer, processor.getInputBufferSize());

    if (!voiceOn) return;
    // TODO which way should I nest these loops?
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
        for (int i = startSample; i < startSample + numSamples; ++i) {
            //outputBuffer.addSample(channel, i, stretcherOutputBuffer[i]);
        }
    }
}