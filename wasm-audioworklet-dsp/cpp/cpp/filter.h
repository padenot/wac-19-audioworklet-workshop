#ifndef FILTER_H
#define FILTER_H

#include "biquad.h"

enum FilterType {
  LowPass,
  HighPass,
  BandPass,
  LowShelf,
  HighShelf,
  Peaking,
  AllPass,
  Notch,
};

class Filter {
public:
  Filter(FilterType type, float frequency, float Q, float gain, float sample_rate)
    : type(type)
      , frequency(frequency)
      , q(Q)
      , gain(gain)
      , sample_rate(sample_rate)
  {
    float nyquist = sample_rate / 2.0;
    set_params_on_biquad();
  }
  void set_frequency(float frequency) {
    this->frequency = frequency;
    set_params_on_biquad();
  }
  void set_q(float q) {
    this->q = q;
    set_params_on_biquad();
  }
  void set_gain(float gain) {
    this->gain = gain;
    set_params_on_biquad();
  }
  void process(float input, float* output) {
    biquad.process(input, output);
  }
  private:
  void set_params_on_biquad()
  {
    float nyquist = sample_rate / 2.0;
    float frequency_normalized = frequency / nyquist;

    switch(type) {
      case FilterType::LowPass:
        biquad.set_lowpass_params(frequency_normalized, q);
        break;
      case FilterType::HighPass:
        biquad.set_highpass_params(frequency_normalized, q);
        break;
      case FilterType::BandPass:
        biquad.set_bandpass_params(frequency_normalized, q);
        break;
      case FilterType::LowShelf:
        biquad.set_lowshelf_params(frequency_normalized, gain);
        break;
      case FilterType::HighShelf:
        biquad.set_highshelf_params(frequency_normalized, gain);
        break;
      case FilterType::Peaking:
        biquad.set_peaking_params(frequency_normalized, q, gain);
        break;
      case FilterType::AllPass:
        biquad.set_allpass_params(frequency_normalized, q);
        break;
      case FilterType::Notch:
        biquad.set_notch_params(frequency_normalized, q);
        break;
    }
  }
  Biquad biquad;
  FilterType type;
  float frequency;
  float q;
  float gain;
  float sample_rate;
};


#endif // FILTER_H
