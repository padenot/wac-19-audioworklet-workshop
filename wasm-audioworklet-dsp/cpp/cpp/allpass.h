#ifndef ALLPASS_DELAY_H
#define ALLPASS_DELAY_H

#include "delay_line.h"

class AllPassDelay {
public:
  AllPassDelay(float delay_frames, float gain)
      : gain(gain), delay_input(DelayLine(5 * delay_frames)),
        delay_output(DelayLine(5 * delay_frames)) {
    delay_input.set_duration(delay_frames);
    delay_output.set_duration(delay_frames);
  }

  void set_gain(float gain) { this->gain = gain; }

  void set_delay(size_t delay_frames) {
    delay_input.set_duration(delay_frames);
    delay_output.set_duration(delay_frames);
  }

  void process(float input, float *output) {
    float delayed_out = 0.0;
    float delayed_in = 0.0;
    delay_input.process(input, &delayed_in);
    delay_output.read(&delayed_out);
    *output = (-gain * input) + delayed_in + (gain * delayed_out);
    delay_output.write(*output);
  }

private:
  float gain;
  DelayLine delay_input;
  DelayLine delay_output;
};

#endif // ALLPASS_DELAY_H
