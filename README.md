# Harmonizer

A polyphonic pitch-shifting plugin built using open source libraries.

My goal is to create something similar to Antares Harmony Engine which is often
used by Bon Iver to create vocal harmonies.

## TODO
- fix polyphony bug
- improve pitch detection (lpf input signal?)
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
