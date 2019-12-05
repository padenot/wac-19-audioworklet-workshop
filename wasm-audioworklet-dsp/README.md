# Instruction

In either the `cpp` or the `rust` directory, there is a web app, a build system,
and DSP primitives in C++ or Rust.

The web app is simply a skeleton that loads:
- an audio loop (a drum loop and a synth line are available, just change the URL
  in `js/app.js`, feel free to swap it out for another source)
- a file to serves as the core of an `AudioWorkletProcessor`
- a javascript files that handles the sample copying to/from the WASM heap

The goal is to combine those DSP primitives to build an effect.

Running `make` in either directory compiles the code to wasm and puts it in the
right directory. Reloading the page allows testing the changes.
