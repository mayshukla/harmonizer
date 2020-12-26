/*
  ==============================================================================

    HarmonizerSound.h
    Created: 24 Dec 2020 5:05:46pm
    Author:  ryan

  ==============================================================================
*/

#pragma once

/**
 * Just a description of the harmonizer sound to pass to the Synthesizer class.
 */
struct HarmonizerSound : public juce::SynthesiserSound {
    HarmonizerSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};