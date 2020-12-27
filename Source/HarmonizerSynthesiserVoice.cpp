/*
  ==============================================================================

    HarmonizerSynthesiserVoice.cpp
    Created: 25 Dec 2020 6:11:08pm
    Author:  ryan

  ==============================================================================
*/

#include "HarmonizerSynthesiserVoice.h"

#include <aubio/musicutils.h>
#include <rubberband/RubberBandStretcher.h>

HarmonizerSynthesiserVoice::HarmonizerSynthesiserVoice(HarmonizerjuceAudioProcessor &processor)
    : processor(processor) {
}

HarmonizerSynthesiserVoice::~HarmonizerSynthesiserVoice() {
    delete stretcher;
    if (stretcherOutputBuffer != nullptr) delete stretcherOutputBuffer;
}

void HarmonizerSynthesiserVoice::setCurrentPlaybackSampleRate(double newRate) {
    // TODO also create new stretcher if sample rate changes
    if (newRate == 0) return; // May be called with 0 before real rate is known.
    if (stretcher == nullptr) {
        RubberBand::RubberBandStretcher::Options stretcherOptions =
            RubberBand::RubberBandStretcher::OptionProcessRealTime
            | RubberBand::RubberBandStretcher::OptionPitchHighConsistency
            | RubberBand::RubberBandStretcher::OptionTransientsSmooth
            | RubberBand::RubberBandStretcher::OptionThreadingAlways;
        stretcher = new RubberBand::RubberBandStretcher(
            newRate,
            1, // channels
            stretcherOptions,
            1, // initialTimeRatio
            1  // initialPitchScale
        );
    }
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
}

void HarmonizerSynthesiserVoice::setExpectedBufferSize(int size) {
    // TODO what if block size changes?
    if (bufferSize == -1) {
        bufferSize = size;
        stretcherOutputBuffer = new float[bufferSize];
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
    stretcher->setPitchScale(pitchScaleFactor);
    const float *inputBuffer = processor.getInputBuffer();
    stretcher->process(&inputBuffer, processor.getInputBufferSize(), false);
    stretcher->retrieve(&stretcherOutputBuffer, processor.getInputBufferSize());

    if (!voiceOn) return;
    // TODO which way should I nest these loops?
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
        for (int i = startSample; i < startSample + numSamples; ++i) {
            outputBuffer.addSample(channel, i, stretcherOutputBuffer[i]);
        }
    }
}