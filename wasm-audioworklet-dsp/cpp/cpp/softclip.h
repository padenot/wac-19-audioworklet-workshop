#include <cmath>

class SoftClip {
public:
  SoftClip(float hardness) : hardness(hardness) {}
  void set_hardness(float hardness) { this->hardness = hardness; }
  void process(float input, float *output) {
    *output = fast_tanh(hardness * input) / hardness;
    // alternative clipper
    // *output = input / sqrt(5. + (input*input));
  }

private:
  float fast_tanh(float x) {
    float x2 = x * x;
    float numerator = x * (135135. + x2 * (17325. + x2 * (378. + x2)));
    float denominator = 135135. + x2 * (62370. + x2 * (3150. + 28. * x2));
    return numerator / denominator;
  }
  float hardness;
};
