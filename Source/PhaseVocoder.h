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

    int getWindowSize() { return windowSize; }

    int getWindowCount() { return windowCount; }

    int getHopSize() { return hopSize; }

    /**
     * TODO: allow caller to specify size of input in case it's smaller than
     * expected.
     * Do the forward transformation on input and store it in output.
     * output should have size [number of windows][bufferSize]
     * output is essential a 2D array stored in window-major order.
     *     [[window0], [window1], [window2], ..]
     */
    void doForward(const float *input, cvec_t **output, int numSamples);

    /**
     * Do the reverse transformation on input and ADD it to output.
     */
    void doReverse(cvec_t **input, float *output, int numSamples);

    void doForwardAndReverse(float *input, cvec_t **output, int numSamples);

private:
    // aubio phase vocoder object
    aubio_pvoc_t *pvoc;

    int sampleRate;
    // Size of buffer processed by Processor
    int bufferSize;
    // Size of window for phase vocoder
    int windowSize = 256;
    // Number of samples between start of one phase vocoder window and next
    int hopSize = windowSize / 16;
    // Numbfer of windows. Need to know bufferSize first.
    int windowCount;

    // Input buffer for forward transform and output buffer for reverse.
    // This only needs to be size hopSize. The aubio library will handle
    // overlapping and windowing, we just supply the new portion of the signal.
    fvec_t *aubioRealBuffer;
};