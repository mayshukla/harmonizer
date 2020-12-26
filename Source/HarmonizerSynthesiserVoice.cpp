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
    if (stretcher == nullptr) {
        RubberBand::RubberBandStretcher::Options stretcherOptions =
            RubberBand::RubberBandStretcher::OptionProcessRealTime
            | RubberBand::RubberBandStretcher::OptionPitchHighConsistency
            | RubberBand::RubberBandStretcher::OptionTransientsSmooth
            | RubberBand::RubberBandStretcher::OptionThreadingAlways;
        stretcher = new RubberBand::RubberBandStretcher(
            1, // channels
            stretcherOptions,
            1, // initialTimeRatio
            1  // initialPitchScale
        );
    }
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
}

void HarmonizerSynthesiserVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) {
    // We don't know the expected block size until the processor starts processing.
    // TODO what if block size changes?
    if (bufferSize == -1) {
        bufferSize = processor.getExpectedBufferSize();
        stretcherOutputBuffer = new float[bufferSize];
    }
    float targetFreq = aubio_miditofreq(midiNoteNumber);
    float pitchScaleFactor = targetFreq / processor.getCurrentPitch();
    stretcher->setPitchScale(pitchScaleFactor);
}

void HarmonizerSynthesiserVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    const float *inputBuffer = processor.getInputBuffer();
    stretcher->process(&inputBuffer, processor.getInputBufferSize(), false);
    stretcher->retrieve(&stretcherOutputBuffer, processor.getInputBufferSize());

    // TODO which way should I nest these loops?
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
        for (int i = startSample; i < startSample + numSamples; ++i) {
            outputBuffer.addSample(channel, i, stretcherOutputBuffer[i]);
        }
    }
}