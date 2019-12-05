# AudioWorklet load testing

Dependencies:

- `rustup` with the wasm target: `rustup target add wasm32-unknown-unknown`
- A rust compiler (stable) and cargo to compile the WASM module
- A recent LLVM toolchain **including the WASM target**
- node to run a local server that knows the mime-type for a WASM module file

Open `localhost:8888` in a browser. There are four things one can do:

- Start the `AudioContext` (because it is probably blocked by the auto-play policy)
- A slider to artificially change the load of the "DSP"
- A button to toggle generating GC pressure **on the main thread** (there is no gc on the audio thread, it's all WASM without allocations)
- A drop-down to choose the language in which the DSP code is written (Rust, C++, JavaScript). This needs a page refresh to then switch what the DSP does.

"DSP" is between quote, because it's not a real DSP, it's just something
that takes an input signal and adds it to the output buffer multiple times,
adjusting the gain so that it's roughly constant, but generating lots of load to
simulate a real app, but allows to reproduce a scenario.

This demo only requires the patches in [bug
1565956](https://bugzilla.mozilla.org/show_bug.cgi?id=1565956).
