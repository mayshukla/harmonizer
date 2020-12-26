/*
  ==============================================================================

    PhaseVocoder.cpp
    Created: 23 Dec 2020 10:55:00pm
    Author:  ryan

  ==============================================================================
*/

#include "PhaseVocoder.h"

#include <aubio/aubio.h>

PhaseVocoder::PhaseVocoder(int bufferSize, int sampleRate)
    : bufferSize(bufferSize), sampleRate(sampleRate) {
    pvoc = new_aubio_pvoc(windowSize, hopSize);
    aubioRealBuffer = new_fvec(bufferSize);

    windowCount = bufferSize / hopSize;
}

PhaseVocoder::~PhaseVocoder() {
    del_aubio_pvoc(pvoc);
    del_fvec(aubioRealBuffer);
}

void PhaseVocoder::doForward(const float *input, cvec_t **output) {
    // Move input signal into buffer to supply to aubio
    for (int i = 0; i < bufferSize; ++i) {
      fvec_set_sample(aubioRealBuffer, input[i], i);
    }

    for (int i = 0; i < windowCount; ++i) {
      aubio_pvoc_do(pvoc, aubioRealBuffer, output[i]);
    }
}
