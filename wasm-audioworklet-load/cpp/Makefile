#!/bin/sh -e

all: ../wasm/wasm_audioworklet_load_cpp.wasm

wasm_audioworklet_load.wasm: load.cpp
	clang++ -cc1 -Ofast -emit-llvm-bc -triple=wasm32-unknown-unknown-wasm -std=c++11 -fvisibility hidden load.cpp
	llvm-link -o load.bc load.bc
	opt -O3 load.bc -o load.bc
	llc -O3 -filetype=obj load.bc -o wasm.o
	wasm-ld --export-dynamic --no-entry --export-all wasm.o -o wasm_audioworklet_load.wasm

../wasm/wasm_audioworklet_load_cpp.wasm: wasm_audioworklet_load.wasm
	cp wasm_audioworklet_load.wasm ../wasm/wasm_audioworklet_load_cpp.wasm

clean:
	rm ../wasm/wasm_audioworklet_load_cpp.wasm
	rm load.bc
	rm wasm_audioworklet_load.wasm
	rm wasm.o

