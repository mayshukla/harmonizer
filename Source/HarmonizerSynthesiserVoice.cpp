/*
  ==============================================================================

    HarmonizerSynthesiserVoice.cpp
    Created: 25 Dec 2020 6:11:08pm
    Author:  ryan

  ==============================================================================
*/

#include "HarmonizerSynthesiserVoice.h"

#include <cmath>

#include <aubio/musicutils.h>

HarmonizerSynthesiserVoice::HarmonizerSynthesiserVoice(HarmonizerjuceAudioProcessor &processor)
    : processor(processor) {
}

HarmonizerSynthesiserVoice::~HarmonizerSynthesiserVoice() {
    delete actualFreqs;
    delete[] previousPhase;
    delete newMags;
    delete newFreqs;
    delete phaseAccum;
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
    if (overlapFactor == -1) {
        overlapFactor = windowSize / hopSize;
    }
    if (freqPerBin == -1) {
        freqPerBin = ((float)sampleRate) / ((float)windowSize);
    }

    if (actualFreqs == nullptr) {
        actualFreqs = new MultiArray<float>(windowCount, windowSize);
    }
    if (previousPhase == nullptr) {
        previousPhase = new float[windowSize];
    }
    if (newMags == nullptr) {
        newMags = new MultiArray<float>(windowCount, windowSize);
    }
    if (newFreqs == nullptr) {
        newFreqs = new MultiArray<float>(windowCount, windowSize);
    }
    if (phaseAccum == nullptr) {
        phaseAccum = new float[windowSize];
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

/**
 * Maps an angle in radians to the interval [-pi, pi]
 */
static inline float constrainAngle(float angle) {
  angle = fmod(angle, 2 * M_PI);
  if (angle > M_PI) {
    angle -= 2 * M_PI;
  }
  if (angle < - M_PI) {
    angle += 2 * M_PI;
  }
  return angle;
}

void HarmonizerSynthesiserVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    if (!voiceOn) return;

    float targetFreq = aubio_miditofreq(midiNoteNumber);
    float pitchScaleFactor = targetFreq / processor.getCurrentPitch();
    if (pitchScaleFactor < 0.01 || std::isnan(pitchScaleFactor)) {
        // Weird pitchScaleFactor probably a glitch.
        // Set to 1 to be safe.
        pitchScaleFactor = 1;
    }

    cvec_t **inputFftWindows = processor.getInputFftWindows();

    // Estimate actual frequency of each bin based on phase difference between
    // each window.
    for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
        for (int window = 0; window < windowCount; ++window) {
            float phase = cvec_phas_get_sample(inputFftWindows[window], bin);
            float phaseDiff = phase - previousPhase[bin];
            previousPhase[bin] = phase;

            // Calculate expected phase difference based on hop size
            float expectedDiff = ((float)bin) * 2 * M_PI * ((float)hopSize) / ((float)windowSize);

            // Correct phaseDiff by accounting for expectedDiff
            phaseDiff -= expectedDiff;

            phaseDiff = constrainAngle(phaseDiff);

            float actualFreq = ((float)bin) * freqPerBin
                + phaseDiff * ((float)overlapFactor) * freqPerBin / (2 * M_PI);

            actualFreqs->set(window, bin, actualFreq);
        }
    }

    // Calculate new magnitudes by resampling and scale frequencies.
    newMags->clear();
    newFreqs->clear();
    for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
        for (int window = 0; window < windowCount; ++window) {
            int newBin = bin * pitchScaleFactor;
            if (newBin < windowSize / 2 + 1) {
                // Resample magnitude
                float oldMag = cvec_norm_get_sample(inputFftWindows[window], bin);
                newMags->set(window, newBin, oldMag);
                // Resample AND scale frequency
                float oldFreq = actualFreqs->get(window, bin);
                newFreqs->set(window, newBin, oldFreq * pitchScaleFactor);
            }
        }
    }

    cvec_t **outputFftWindows = processor.getOutputFftWindows();

    // Calculate new phases based on scaled frequencies.
    for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
        for (int window = 0; window < windowCount; ++window) {
            float deltaFreq = newFreqs->get(window, bin)
                - ((float)bin) * freqPerBin;

            float deltaPhase = deltaFreq * 2 * M_PI;
            deltaPhase /= (float)overlapFactor;
            deltaPhase /= freqPerBin;

            deltaPhase += ((float)bin) * 2 * M_PI
                * ((float)hopSize) / ((float)windowSize);

            phaseAccum[bin] += deltaPhase;
            double phase = phaseAccum[bin];
            double mag = newMags->get(window, bin);

            // Add mag and phase to existing data
            mag +=  cvec_norm_get_sample(outputFftWindows[window], bin);
            cvec_norm_set_sample(outputFftWindows[window], mag, bin);
            phase +=  cvec_phas_get_sample(outputFftWindows[window], bin);
            cvec_phas_set_sample(outputFftWindows[window], phase, bin);
        }
    }
}