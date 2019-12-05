class DSPProcessor extends AudioWorkletProcessor {
  static get parameterDescriptors() {
    return [];
  }

  constructor() {
    super();
    this.port.onmessage = e => {
      if (e.data.type === "load-processor") {
        this._size = 128;
        console.log(this);
        this.v = _init_effect(44100);
        this._process = Module._process;
        // * sizeof float
        this._inPtr = getMemory(this._size * 4);
        this._outPtr = getMemory(this._size * 4);
        this._inBuf = new Float32Array(
          Module.HEAP32.buffer,
          this._inPtr,
          this._size
        );
        this._outBuf = new Float32Array(
          Module.HEAP32.buffer,
          this._outPtr,
          this._size
        );
      } else {
        throw "unexpected.";
      }
    };
  }

  process(inputs, outputs, parameters) {
    if (!this._process) {
      return true;
    }

    let output = outputs[0];
    let input = inputs[0];
    for (let channel = 0; channel < input.length; ++channel) {
      for (var i = 0; i < 128; i++) {
        this._inBuf[i] = 128 * channel + input[channel][i];
      }
    }

    for (let channel = 0; channel < output.length; channel++) {
      this._process(this.v, this._inPtr, this._outPtr, this._size);
      if (this._outBuf.buffer.byteLength == 0) {
        console.log(
          "heap growth detected, taking new windows on WASM heap, hold on"
        );
        this._inBuf = new Float32Array(
          Module.HEAP32.memory,
          this._inPtr,
          this._size
        );
        this._outBuf = new Float32Array(
          Module.HEAP32.memory,
          this._outPtr,
          this._size
        );
      }
      let outputChannel = output[channel];
      outputChannel.set(this._outBuf);
    }

    return true;
  }
}

registerProcessor("processor", DSPProcessor);
