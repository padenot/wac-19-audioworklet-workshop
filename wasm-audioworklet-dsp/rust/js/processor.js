class DSPProcessor extends AudioWorkletProcessor {
  static get parameterDescriptors() {
    return []
  }

  constructor() {
    super()
    this.port.onmessage = e => {
      if (e.data.type === 'load-processor') {
        this._size = 128;
        var memory = new WebAssembly.Memory({initial: 512});
        var instance = new WebAssembly.Instance(new WebAssembly.Module(e.data.data), {env: { memory: memory}});
        this._wasm = instance;
        this._process = this._wasm.exports.process;
        this._inPtr = this._wasm.exports.alloc(this._size);
        this._outPtr = this._wasm.exports.alloc(this._size);
        this._inBuf = new Float32Array(this._wasm.exports.memory.buffer, this._inPtr, this._size);
        this._outBuf = new Float32Array(this._wasm.exports.memory.buffer, this._outPtr, this._size);
      } else {
        throw "unexpected.";
      }
    }
  }

  process(inputs, outputs, parameters) {
    if (!this._process) {
      return true
    }

    let output = outputs[0]
    let input = inputs[0]
    for (let channel = 0; channel < input.length; ++channel) {
      for (var i = 0; i < 128; i++) {
        this._inBuf[i] = 128 * channel + input[channel][i];
      }
    }

    for (let channel = 0; channel < output.length; channel++) {
      this._process(this._inPtr, this._outPtr, this._size)
      if (this._outBuf.buffer.byteLength == 0) {
        console.log("heap growth detected, taking new windows on WASM heap, hold on");
        this._inBuf = new Float32Array(this._wasm.exports.memory.buffer, this._inPtr, this._size);
        this._outBuf = new Float32Array(this._wasm.exports.memory.buffer, this._outPtr, this._size);
      }
      let outputChannel = output[channel]
      outputChannel.set(this._outBuf)
    }

    return true;
  }
}

registerProcessor('processor', DSPProcessor)
