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
          this.o = { index: 0, value: 0 };
        }
      } else if (e.data.type === "recv-message-queue") {
        var d = e.data.data;
        eval(
          "" +
            e.data.code +
            "; this.rb = RingBuffer; this.pr = ParameterReader;"
        );
        this._param_reader = new this.pr(new this.rb(d));
      } else {
        throw "unexpected.";
      }
    };
  }

  process(inputs, outputs, parameters) {
    if (!this._process) {
      return true;
    }

    let index, value;
    // Only set load if we could get a value, but use the latest value.
    if (this._param_reader.dequeue_change(this.o)) {
      this._set_load(this.o.value);
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
