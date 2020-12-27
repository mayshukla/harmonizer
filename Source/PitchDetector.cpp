/*
  ==============================================================================

    PitchDetector.cpp
    Created: 23 Dec 2020 4:11:00pm
    Author:  ryan

  ==============================================================================
*/

#include "PitchDetector.h"

PitchDetector::PitchDetector(int sampleRate) {
    aubioInputBuffer = new_fvec(bufferSize);
    aubioOutputBuffer = new_fvec(1);

    aubioPitch = new_aubio_pitch("yinfast",
                                 bufferSize,
                                 bufferSize,
                                 sampleRate);
    aubio_pitch_set_silence(aubioPitch, -50);
    aubio_pitch_set_tolerance(aubioPitch, 0.9);
    aubio_pitch_set_unit(aubioPitch, "Hz");
}

PitchDetector::~PitchDetector() {
    del_aubio_pitch(aubioPitch);
    del_fvec(aubioInputBuffer);
    del_fvec(aubioOutputBuffer);
}

void PitchDetector::doPitchDetection(const float *input, int size) {
    // Move over old cached values in aubio input buffer
    // TODO use a circular buffer
    for (int i = 0; (i + size) < bufferSize; ++i) {
        double sample = fvec_get_sample(aubioInputBuffer, i + size);
        fvec_set_sample(aubioInputBuffer, sample, i); 
    }

    int offset = bufferSize - size;
    for (int i = 0; i < size; ++i) {
        fvec_set_sample(aubioInputBuffer, input[i], i + offset);
    }
    aubio_pitch_do(aubioPitch, aubioInputBuffer, aubioOutputBuffer);
    double pitchCandidate = fvec_get_sample(aubioOutputBuffer, 0);
    if (pitchCandidate != 0) currentPitch = pitchCandidate;
}
