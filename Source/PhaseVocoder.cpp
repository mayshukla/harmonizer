/*
  ==============================================================================

    PhaseVocoder.cpp
    Created: 23 Dec 2020 10:55:00pm
    Author:  ryan

  ==============================================================================
*/

#include "PhaseVocoder.h"

#include <iostream> // TODO remove

#include <aubio/aubio.h>

PhaseVocoder::PhaseVocoder(int sampleRate, int bufferSize)
    : sampleRate(sampleRate), bufferSize(bufferSize) {
    pvoc = new_aubio_pvoc(windowSize, hopSize);
    aubioRealBuffer = new_fvec(hopSize);

    windowCount = bufferSize / hopSize;
}

PhaseVocoder::~PhaseVocoder() {
    del_aubio_pvoc(pvoc);
    del_fvec(aubioRealBuffer);
}

void PhaseVocoder::doForward(const float *input, cvec_t **output, int numSamples) {
    for (int window = 0; window < windowCount; ++window) {
        // Move input signal into buffer to supply to aubio
        int offset = window * hopSize;
        for (int i = 0; i < hopSize; ++i) {
          if ((offset + i) > numSamples) break;
          fvec_set_sample(aubioRealBuffer, input[offset + i], i);
        }

        aubio_pvoc_do(pvoc, aubioRealBuffer, output[window]);
    }
}

void PhaseVocoder::doForwardAndReverse(float *input, cvec_t **output, int numSamples) {
    for (int window = 0; window < windowCount; ++window) {
        // Move input signal into buffer to supply to aubio
        int offset = window * hopSize;
        for (int i = 0; i < hopSize; ++i) {
          if ((offset + i) > numSamples) break;
          fvec_set_sample(aubioRealBuffer, input[offset + i], i);
        }

        aubio_pvoc_do(pvoc, aubioRealBuffer, output[window]);
        aubio_pvoc_rdo(pvoc, output[window], aubioRealBuffer);

        for (int i = 0; i < hopSize; ++i) {
            if ((offset + i) > numSamples) break;
            input[offset + i] = fvec_get_sample(aubioRealBuffer, i);
        }
    }
}

void PhaseVocoder::doReverse(cvec_t **input, float *output, int numSamples) {
    for (int window = 0; window < windowCount; ++window) {
        aubio_pvoc_rdo(pvoc, input[window], aubioRealBuffer);

        int offset = window * hopSize;
        for (int i = 0; i < hopSize; ++i) {
            if ((offset + i) > numSamples) break;
            output[offset + i] += fvec_get_sample(aubioRealBuffer, i);
        }
    }
}
