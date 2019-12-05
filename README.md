# AudioWorklet load testing

Dependencies:

If working with Rust:
- `rustup`: <https://rustup.rs/>
- `rustup` with the wasm target: `rustup target add wasm32-unknown-unknown`
- A rust compiler (stable) and cargo to compile the WASM module

If working with C++:
- A recent LLVM toolchain **including the WASM target (> 8.0)**
  <https://releases.llvm.org/download.html#9.0.0> or `brew install llvm` or `apt`, etc. On OSX, mais sure to run `xcode-select --install` before that to avoid recompiling the world.
- Emscripten: <https://emscripten.org/docs/getting_started/downloads.html>
- `wabt`: <https://github.com/WebAssembly/wabt> (is in `brew` and Linux package
  managers, etc.)

In all cases:
- `node` to run a local server that knows the mime-type for a WASM module file
  (no dependencies)
- `make` (or just run the command by hand, they are not long/complex)

In each directory, run the node server (which is very standard except it serves `.wasm` files with the right mimetype)

> node server.js
