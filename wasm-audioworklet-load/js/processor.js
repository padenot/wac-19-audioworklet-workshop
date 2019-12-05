class LoadProcessor extends AudioWorkletProcessor {
  static get parameterDescriptors() {
    return [];
  }

  constructor() {
    super();
    this.port.onmessage = e => {
      if (e.data.type === "load-processor") {
        this._size = 128;
        if (e.data.wasm) {
          var instance = new WebAssembly.Instance(
            new WebAssembly.Module(e.data.data),
            {}
          );
          this._wasm = instance;
          this._process = this._wasm.exports.process;
          this._set_load = this._wasm.exports.set_load;
          this._inPtr = this._wasm.exports.alloc(this._size);
          this._outPtr = this._wasm.exports.alloc(this._size);
          this._inBuf = new Float32Array(
            this._wasm.exports.memory.buffer,
            this._inPtr,
            this._size
          );
          this._outBuf = new Float32Array(
            this._wasm.exports.memory.buffer,
            this._outPtr,
            this._size
          );
        } else {
          var jsprocessor = eval(e.data.data);
          this._instance = new jsprocessor();
          this._process = this._instance.process;
          this._set_load = this._instance.set_load;
          this._inBuf = new Float32Array(this._size);
          this._outBuf = new Float32Array(this._size);
          this._inPtr = this._inBuf;
          this._outPtr = this._outBuf;
          this._set_load(1.0);
        }
      } else if (e.data.type === "set-load") {
        this._set_load(e.data.data);
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
      let outputChannel = output[channel];
      this._process(this._inPtr, this._outPtr, this._size);
      outputChannel.set(this._outBuf);
    }

    return true;
  }
}

registerProcessor("processor", LoadProcessor);
