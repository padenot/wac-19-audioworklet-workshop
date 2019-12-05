# AudioWorklet load testing

Four things to do:

- Start the `AudioContext` (because it is probably blocked by the auto-play
  policy)
- A slider to artificially change the load of the "DSP"
- A button to toggle generating GC pressure **on the main thread** (there is no
  gc on the audio thread, it's all WASM without allocations)
- A drop-down to choose the language in which the DSP code is written (Rust,
  C++, JavaScript). This needs a page refresh to then switch what the DSP does.

"DSP" is between quote, because it's not a real DSP, it's just something
that takes an input signal and adds it to the output buffer multiple times,
adjusting the gain so that it's roughly constant, but generating lots of load to
simulate a real app, but allows to reproduce a scenario.
