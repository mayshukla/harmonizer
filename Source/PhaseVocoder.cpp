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

    data = new_cvec(bufferSize * windowSize);
}

PhaseVocoder::~PhaseVocoder() {
    del_aubio_pvoc(pvoc);
    del_cvec(data);
}
