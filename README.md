## Implementation Plan
- PhaseVocoder: wrapper around aubio pvoc
  - doForward: float[] -> cvec_t[][]
  - doReverse: cvec_t[][] -> float[]
- HarmonizerAudioProcessor: holds a Synthesizer, does setup for Synthesizer (e.g.
  add voices), does forward phase vocoder transform (since this is shared
  between voices)
  - Has a method to supply it with input signal
- HarmonizerSynthesizerVoice: responsible for pitch shifting one note and
  adding that note to the output buffer.
  - Needs a reference to HarmonizerAudioProcessor to get forward phase vocoder
    signal and current pitch
- HarmonizerSound: a simple description of a sound. Required by the Synthesizer
  class.

## Depedencies
- [aubio](https://aubio.org/)
