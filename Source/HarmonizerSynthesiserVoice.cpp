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

void HarmonizerSynthesiserVoice::prepareToPlay(int sampleRate, int bufferSize) {
    if (phaseVocoder == nullptr) {
        phaseVocoder = new PhaseVocoder(sampleRate, bufferSize);
    }

    // TODO handle these parameters changing dynamically
    if (this->sampleRate == -1) {
        this->sampleRate = sampleRate;
    }
    if (this->bufferSize == -1) {
        this->bufferSize = bufferSize;
    }
    if (this->windowSize == -1) {
        this->windowSize = phaseVocoder->getWindowSize();
    }
    if (this->windowCount == -1) {
        this->windowCount = phaseVocoder->getWindowCount();
    }
    if (this->hopSize == -1) {
        this->hopSize = phaseVocoder->getHopSize();
    }
    if (overlapFactor == -1) {
        overlapFactor = windowSize / hopSize;
    }
    if (freqPerBin == -1) {
        freqPerBin = ((float)sampleRate) / ((float)windowSize);
    }

    if (outputFftWindows == nullptr) {
        outputFftWindows = new cvec_t * [phaseVocoder->getWindowCount()];
        for (int i = 0; i < phaseVocoder->getWindowCount(); ++i) {
            outputFftWindows[i] = new_cvec(phaseVocoder->getWindowSize());
        }
    }

    if (actualFreqs == nullptr) {
        actualFreqs = new MultiArray<float>(windowCount, windowSize / 2 + 1);
    }
    if (previousPhase == nullptr) {
        previousPhase = new float[windowSize / 2 + 1];
    }
    if (newMags == nullptr) {
        newMags = new MultiArray<float>(windowCount, windowSize / 2 +1);
    }
    if (newFreqs == nullptr) {
        newFreqs = new MultiArray<float>(windowCount, windowSize / 2 + 1);
    }
    if (phaseAccum == nullptr) {
        phaseAccum = new float[windowSize / 2 + 1];
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
    // Clear output fft windows
    for (int window = 0; window < windowCount; ++window) {
        for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
            cvec_norm_set_sample(outputFftWindows[window], 0, bin);
            cvec_phas_set_sample(outputFftWindows[window], 0, bin);
        }
    }

    float targetFreq = aubio_miditofreq(midiNoteNumber);
    float pitchScaleFactor = targetFreq / processor.getCurrentPitch();
    if (pitchScaleFactor < 0.01 || std::isnan(pitchScaleFactor)) {
        // Weird pitchScaleFactor probably a glitch.
        // Set to 1 to be safe.
        pitchScaleFactor = 1;
    }

    cvec_t **inputFftWindows = processor.getInputFftWindows();

    for (int window = 0; window < windowCount; ++window) {
        // Estimate actual frequency of each bin based on phase difference
        // between each window.
        for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
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

        // Calculate new magnitudes by resampling and scale frequencies.
        newMags->clear();
        newFreqs->clear();
        for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
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

        // Calculate new phases based on scaled frequencies.
        for (int bin = 0; bin < windowSize / 2 + 1; ++bin) {
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

    if (voiceOn) {
        phaseVocoder->doReverse(outputFftWindows, outputBuffer.getWritePointer(0), numSamples);
    }
}