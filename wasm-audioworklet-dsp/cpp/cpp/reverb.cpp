#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

#include "delay_line.h"
// #include "filter.h"
// #include "utils.h"
// #include "softclip.h"
// #include "allpass.h"
//
extern "C" {
EMSCRIPTEN_KEEPALIVE
DelayLine *init_effect(int sample_rate) {
  DelayLine *delay = new DelayLine(sample_rate);
  delay->set_duration(sample_rate / 10); // 100ms

  return delay;
}
EMSCRIPTEN_KEEPALIVE
void process(DelayLine *delay, float *input, float *output, size_t size) {
  float out;
  for (size_t i = 0; i < size; i++) {
    delay->process(input[i], &out);
    // half-gain on the feedback
    output[i] = input[i] + 0.5 * out;
  }
}

EMSCRIPTEN_KEEPALIVE
float *alloc(size_t size) { return new float[size]; }
};

#ifndef EMSCRIPTEN
int main() {
  DelayLine *delay = init_effect(100);
  delay->set_duration(50);
  std::vector<float> input(1000);
  std::vector<float> output(1000);

  for (size_t i = 0; i < input.size(); i++) {
    input[i] = i;
  }
  float out;
  for (size_t i = 0; i < 1000; i++) {
    delay->process(input[i], &out);
    output[i] = out;
  }
  for (size_t i = 0; i < 1000; i++) {
    printf("%zu %f\n", i, output[i]);
  }
}
#endif
