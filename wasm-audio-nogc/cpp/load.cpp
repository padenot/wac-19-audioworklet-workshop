struct processor {
  float load = 1.0;
  void process(float *input_buffer, float *output_buffer, unsigned long size) {
    double compensation_gain = 1. / (load * 1000.);
    unsigned long iterations = load * 1000;

    for (unsigned long j = 0; j < iterations; j++) {
      for (unsigned long i = 0; i < size; i++) {
        output_buffer[i] += compensation_gain * input_buffer[i];
      }
    }
  }
};

extern "C" {
// base of the WASM heap
extern char __heap_base;

float *alloc(unsigned long size) {
  static char *bump_ptr = &__heap_base;
  void *previous = bump_ptr;
  bump_ptr += size;
  return (float *)previous;
}

void process(float *input_buffer, float *output_buffer, unsigned long size) {
  static processor p;
  p.process(input_buffer, output_buffer, size);
}
};
