/*
  ==============================================================================

    PhaseVocoder.h
    Created: 23 Dec 2020 10:55:00pm
    Author:  ryan

  ==============================================================================
*/

#pragma once

#include <aubio/aubio.h>

class PhaseVocoder {
public:
    PhaseVocoder(int sampleRate, int bufferSize);
    ~PhaseVocoder();

    /**
     * Do the forward transformation.
     * Use getData() to get the transformed data.
     */
    void doForward(float *input);

    /**
     * Do the reverse transformation on the data and store it in output.
     */
    void doReverse(float *output);

    /**
     * Note: the data array is owned by the PhaseVocoder.
     * DO NOT attempt to free it.
     */
    cvec_t *getData() { return data; };

private:
    // aubio phase vocoder object
    aubio_pvoc_t *pvoc;

    int sampleRate;
    // Size of buffer processed by Processor
    int bufferSize;
    // Size of window for phase vocoder
    int windowSize = 32;
    // Number of samples between start of one phase vocoder window and next
    int hopSize = windowSize / 4;

    // Input buffer for aubio
    fvec_t *aubioInputBuffer;
    // Array of size bufferSize x windowSize
    // [window0, window1, window2, ...]
    cvec_t *data;
    // Output buffer for aubio
    fvec_t *aubioOutputBuffer;
};