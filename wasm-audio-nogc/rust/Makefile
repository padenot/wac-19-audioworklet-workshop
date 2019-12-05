#!/bin/sh -e

all: ../wasm/wasm_audioworklet_load_rust.wasm

target/wasm32-unknown-unknown/release/wasm_audioworklet_load.wasm:
	@cargo build --target wasm32-unknown-unknown --release

../wasm/wasm_audioworklet_load_rust.wasm: target/wasm32-unknown-unknown/release/wasm_audioworklet_load.wasm
	@cp target/wasm32-unknown-unknown/release/wasm_audioworklet_load.wasm ../wasm/wasm_audioworklet_load_rust.wasm

clean:
	rm -r target
	rm -r wasm/wasm_audioworklet_load_rust.wasm

