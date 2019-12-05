#!/bin/sh -e

all: ../js/processor.js

../js/processor.js: $(wildcard *.cpp) $(wildcard *.h) Makefile ../js/processor-end.js
	em++ -g4 -Os  -fno-exceptions --std=c++11 reverb.cpp -s BINARYEN_ASYNC_COMPILATION=0 -s SINGLE_FILE=1 -s STANDALONE_WASM -s EXPORT_ALL=1   -o wasm_audioworklet.js
	cat wasm_audioworklet.js ../js/processor-end.js > ../js/processor.js

clean:
	rm wasm_audioworklet.wasm

