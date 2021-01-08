# Harmonizer

A polyphonic pitch-shifting plugin built using open source libraries.

Pitches are controlled by midi.

My goal is to create something similar to Antares Harmony Engine or the
["Messina" device that Bon Iver uses to create vocal harmonies](https://youtu.be/CaYgMdq6NDg).

## How It Works

- The pitch of the input signal is estimated using the pitch detection module
  from the aubio library.
- Target pitches are identified based on which midi notes are active.
- For each target pitch:
  - The input signal is duplicated and shifted to the target frequency.
  - Pitch shifting is done using the phase vocoder implementation from the
    aubio library.
- The pitch-shifted signals are summed together and written to the output buffer

## TODO
- fix all NaN issues
- improve pitch detection (lpf input signal?)
- experiment with different window sizes and overlap
- fix stereo routing

## Implementation Plan
- voices do pitch shifting and add their own shifted signal to the frequency
  domain
- this way there is only one forward and one reverse pvoc operation

- PhaseVocoder: wrapper around aubio pvoc
  - doForward: float[] -> cvec_t[][]
  - doReverse: cvec_t[][] -> float[]
- HarmonizerAudioProcessor: holds a Synthesizer, does setup for Synthesizer (e.g.
  add voices), does forward AND reverse phase vocoder transform
- HarmonizerSynthesizerVoice: responsible for pitch shifting one note and
  adding that note to the shared cvec_t
  - Needs a reference to HarmonizerAudioProcessor to get forward phase vocoder
    signal and current pitch
- HarmonizerSound: a simple description of a sound. Required by the Synthesizer
  class.

## Depedencies
- [aubio](https://aubio.org/)

## Resources
- (http://blogs.zynaptiq.com/bernsee/pitch-shifting-using-the-ft/)
