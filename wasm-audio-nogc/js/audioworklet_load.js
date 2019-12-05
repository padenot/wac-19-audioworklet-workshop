this.JSProcessor = function jsp() {
  this.load = 1.0,
  this.set_load = function s(load) {
    this.load = load;
  },
  this.process = function p(input_buffer, output_buffer, size) {
    let iterations = this.load * 1000.;
    let gain_compensation = 1 / iterations;
    for (var j = 0; j < iterations; j++) {
      for (var i = 0; i < size; i++) {
        output_buffer[i] += input_buffer[i] * gain_compensation;
      }
    }
  }
}
