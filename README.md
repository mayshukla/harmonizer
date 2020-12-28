## TODO
- figure out if i'm using pvoc correctly (should i do forward/reverse within one window?)
  - update: I don't think this is the problem. I tried it that way and it was
    still bad

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

## New Implementation Plan
- Just use one RubberBandStretcher per voice
- If this is too inefficient, they I made need to implement my own phase
  vocoder.

- HarmonizerSynthesizerVoice: responsible for pitch shifting one note and
  adding that note to the output buffer.
  - Needs a reference to HarmonizerAudioProcessor to get current pitch and
    pointer to buffer with input signal (since Synthesiser class usually
    doesn't expect input signal)
- HarmonizerSound: a simple description of a sound. Required by the Synthesizer
  class.

## New New Implementation Plan
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
