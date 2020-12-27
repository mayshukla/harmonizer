/*
  ==============================================================================

    PitchDetector.h
    Created: 23 Dec 2020 4:11:00pm
    Author:  ryan

  ==============================================================================
*/

#pragma once

#include <aubio/aubio.h>

class PitchDetector
{
public:
    PitchDetector(int sampleRate);
    ~PitchDetector();

    void doPitchDetection(const float *input, int size);

    float getCurrentPitch() { return currentPitch; };

private:
    // I've found this to be a good buffer size for pitch detection at a sample
    // rate around 48000 kHz.
    // TODO what if the sample rate is different?
    static constexpr int bufferSize = 2048;
    aubio_pitch_t *aubioPitch;
    // Array of length bufferSize
    fvec_t *aubioInputBuffer;
    // A single float representing pitch.
    fvec_t *aubioOutputBuffer;
    float currentPitch = 440;
};