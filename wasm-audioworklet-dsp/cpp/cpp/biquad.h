#ifndef BIQUAD_H
#define BIQUAD_H

/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Web Audio API Biquad filter originally from WebKit, forked for Gecko
// https://searchfox.org/mozilla-central/source/dom/media/webaudio/blink/Biquad.cpp

#include <algorithm>
#include <complex>
#include <float.h>
#include <math.h>

class Biquad {
  typedef std::complex<double> Complex;

public:
  Biquad() {
    // Initialize as pass-thru (straight-wire, no filter effect)
    set_normalized_coefficients(1, 0, 0, 1, 0, 0);

    reset(); // clear filter memory
  }

  ~Biquad() {}

  void process(float input, float *output) {
    // Create local copies of member variables
    double x1 = m_x1;
    double x2 = m_x2;
    double y1 = m_y1;
    double y2 = m_y2;

    double b0 = m_b0;
    double b1 = m_b1;
    double b2 = m_b2;
    double a1 = m_a1;
    double a2 = m_a2;

    double out;

    out = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

    // Update state variables
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = out;

    *output = out;

    // Avoid introducing a stream of subnormals when input is silent and the
    // tail approaches zero.
    if (x1 == 0.0 && x2 == 0.0 && (y1 != 0.0 || y2 != 0.0) &&
        fabs(y1) < FLT_MIN && fabs(y2) < FLT_MIN) {
      // Flush future values to zero (until there is new input).
      y1 = y2 = 0.0;
      // Flush calculated values.
    }
    // Local variables back to member.
    m_x1 = x1;
    m_x2 = x2;
    m_y1 = y1;
    m_y2 = y2;
  }

  void reset() { m_x1 = m_x2 = m_y1 = m_y2 = 0; }

  void set_lowpass_params(double cutoff, double resonance) {
    // Limit cutoff to 0 to 1.
    cutoff = std::max(0.0, std::min(cutoff, 1.0));

    if (cutoff == 1) {
      // When cutoff is 1, the z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    } else if (cutoff > 0) {
      // Compute biquad coefficients for lowpass filter
      double g = pow(10.0, -0.05 * resonance);
      double w0 = M_PI * cutoff;
      double cos_w0 = cos(w0);
      double alpha = 0.5 * sin(w0) * g;

      double b1 = 1.0 - cos_w0;
      double b0 = 0.5 * b1;
      double b2 = b0;
      double a0 = 1.0 + alpha;
      double a1 = -2.0 * cos_w0;
      double a2 = 1.0 - alpha;

      set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
    } else {
      // When cutoff is zero, nothing gets through the filter, so set
      // coefficients up correctly.
      set_normalized_coefficients(0, 0, 0, 1, 0, 0);
    }
  }

  void set_highpass_params(double cutoff, double resonance) {
    // Limit cutoff to 0 to 1.
    cutoff = std::max(0.0, std::min(cutoff, 1.0));

    if (cutoff == 1) {
      // The z-transform is 0.
      set_normalized_coefficients(0, 0, 0, 1, 0, 0);
    } else if (cutoff > 0) {
      // Compute biquad coefficients for highpass filter
      double g = pow(10.0, -0.05 * resonance);
      double w0 = M_PI * cutoff;
      double cos_w0 = cos(w0);
      double alpha = 0.5 * sin(w0) * g;

      double b1 = -1.0 - cos_w0;
      double b0 = -0.5 * b1;
      double b2 = b0;
      double a0 = 1.0 + alpha;
      double a1 = -2.0 * cos_w0;
      double a2 = 1.0 - alpha;

      set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
    } else {
      // When cutoff is zero, we need to be careful because the above
      // gives a quadratic divided by the same quadratic, with poles
      // and zeros on the unit circle in the same place. When cutoff
      // is zero, the z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    }
  }

  void set_lowshelf_params(double frequency, double dbGain) {
    // Clip frequencies to between 0 and 1, inclusive.
    frequency = std::max(0.0, std::min(frequency, 1.0));

    double A = pow(10.0, dbGain / 40);

    if (frequency == 1) {
      // The z-transform is a constant gain.
      set_normalized_coefficients(A * A, 0, 0, 1, 0, 0);
    } else if (frequency > 0) {
      double w0 = M_PI * frequency;
      double S = 1; // filter slope (1 is max value)
      double alpha = 0.5 * sin(w0) * sqrt((A + 1 / A) * (1 / S - 1) + 2);
      double k = cos(w0);
      double k2 = 2 * sqrt(A) * alpha;
      double aPlusOne = A + 1;
      double aMinusOne = A - 1;

      double b0 = A * (aPlusOne - aMinusOne * k + k2);
      double b1 = 2 * A * (aMinusOne - aPlusOne * k);
      double b2 = A * (aPlusOne - aMinusOne * k - k2);
      double a0 = aPlusOne + aMinusOne * k + k2;
      double a1 = -2 * (aMinusOne + aPlusOne * k);
      double a2 = aPlusOne + aMinusOne * k - k2;

      set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
    } else {
      // When frequency is 0, the z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    }
  }

  void set_highshelf_params(double frequency, double dbGain) {
    // Clip frequencies to between 0 and 1, inclusive.
    frequency = std::max(0.0, std::min(frequency, 1.0));

    double A = pow(10.0, dbGain / 40);

    if (frequency == 1) {
      // The z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    } else if (frequency > 0) {
      double w0 = M_PI * frequency;
      double S = 1; // filter slope (1 is max value)
      double alpha = 0.5 * sin(w0) * sqrt((A + 1 / A) * (1 / S - 1) + 2);
      double k = cos(w0);
      double k2 = 2 * sqrt(A) * alpha;
      double aPlusOne = A + 1;
      double aMinusOne = A - 1;

      double b0 = A * (aPlusOne + aMinusOne * k + k2);
      double b1 = -2 * A * (aMinusOne + aPlusOne * k);
      double b2 = A * (aPlusOne + aMinusOne * k - k2);
      double a0 = aPlusOne - aMinusOne * k + k2;
      double a1 = 2 * (aMinusOne - aPlusOne * k);
      double a2 = aPlusOne - aMinusOne * k - k2;

      set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
    } else {
      // When frequency = 0, the filter is just a gain, A^2.
      set_normalized_coefficients(A * A, 0, 0, 1, 0, 0);
    }
  }

  void set_peaking_params(double frequency, double Q, double dbGain) {
    // Clip frequencies to between 0 and 1, inclusive.
    frequency = std::max(0.0, std::min(frequency, 1.0));

    // Don't let Q go negative, which causes an unstable filter.
    Q = std::max(0.0, Q);

    double A = pow(10.0, dbGain / 40);

    if (frequency > 0 && frequency < 1) {
      if (Q > 0) {
        double w0 = M_PI * frequency;
        double alpha = sin(w0) / (2 * Q);
        double k = cos(w0);

        double b0 = 1 + alpha * A;
        double b1 = -2 * k;
        double b2 = 1 - alpha * A;
        double a0 = 1 + alpha / A;
        double a1 = -2 * k;
        double a2 = 1 - alpha / A;

        set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
      } else {
        // When Q = 0, the above formulas have problems. If we look at
        // the z-transform, we can see that the limit as Q->0 is A^2, so
        // set the filter that way.
        set_normalized_coefficients(A * A, 0, 0, 1, 0, 0);
      }
    } else {
      // When frequency is 0 or 1, the z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    }
  }

  void set_allpass_params(double frequency, double Q) {
    // Clip frequencies to between 0 and 1, inclusive.
    frequency = std::max(0.0, std::min(frequency, 1.0));

    // Don't let Q go negative, which causes an unstable filter.
    Q = std::max(0.0, Q);

    if (frequency > 0 && frequency < 1) {
      if (Q > 0) {
        double w0 = M_PI * frequency;
        double alpha = sin(w0) / (2 * Q);
        double k = cos(w0);

        double b0 = 1 - alpha;
        double b1 = -2 * k;
        double b2 = 1 + alpha;
        double a0 = 1 + alpha;
        double a1 = -2 * k;
        double a2 = 1 - alpha;

        set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
      } else {
        // When Q = 0, the above formulas have problems. If we look at
        // the z-transform, we can see that the limit as Q->0 is -1, so
        // set the filter that way.
        set_normalized_coefficients(-1, 0, 0, 1, 0, 0);
      }
    } else {
      // When frequency is 0 or 1, the z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    }
  }

  void set_notch_params(double frequency, double Q) {
    // Clip frequencies to between 0 and 1, inclusive.
    frequency = std::max(0.0, std::min(frequency, 1.0));

    // Don't let Q go negative, which causes an unstable filter.
    Q = std::max(0.0, Q);

    if (frequency > 0 && frequency < 1) {
      if (Q > 0) {
        double w0 = M_PI * frequency;
        double alpha = sin(w0) / (2 * Q);
        double k = cos(w0);

        double b0 = 1;
        double b1 = -2 * k;
        double b2 = 1;
        double a0 = 1 + alpha;
        double a1 = -2 * k;
        double a2 = 1 - alpha;

        set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
      } else {
        // When Q = 0, the above formulas have problems. If we look at
        // the z-transform, we can see that the limit as Q->0 is 0, so
        // set the filter that way.
        set_normalized_coefficients(0, 0, 0, 1, 0, 0);
      }
    } else {
      // When frequency is 0 or 1, the z-transform is 1.
      set_normalized_coefficients(1, 0, 0, 1, 0, 0);
    }
  }

  void set_bandpass_params(double frequency, double Q) {
    // No negative frequencies allowed.
    frequency = std::max(0.0, frequency);

    // Don't let Q go negative, which causes an unstable filter.
    Q = std::max(0.0, Q);

    if (frequency > 0 && frequency < 1) {
      double w0 = M_PI * frequency;
      if (Q > 0) {
        double alpha = sin(w0) / (2 * Q);
        double k = cos(w0);

        double b0 = alpha;
        double b1 = 0;
        double b2 = -alpha;
        double a0 = 1 + alpha;
        double a1 = -2 * k;
        double a2 = 1 - alpha;

        set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
      } else {
        // When Q = 0, the above formulas have problems. If we look at
        // the z-transform, we can see that the limit as Q->0 is 1, so
        // set the filter that way.
        set_normalized_coefficients(1, 0, 0, 1, 0, 0);
      }
    } else {
      // When the cutoff is zero, the z-transform approaches 0, if Q
      // > 0. When both Q and cutoff are zero, the z-transform is
      // pretty much undefined. What should we do in this case?
      // For now, just make the filter 0. When the cutoff is 1, the
      // z-transform also approaches 0.
      set_normalized_coefficients(0, 0, 0, 1, 0, 0);
    }
  }

  void setZeroPolePairs(const Complex &zero, const Complex &pole) {
    double b0 = 1;
    double b1 = -2 * zero.real();

    double zeroMag = abs(zero);
    double b2 = zeroMag * zeroMag;

    double a1 = -2 * pole.real();

    double poleMag = abs(pole);
    double a2 = poleMag * poleMag;
    set_normalized_coefficients(b0, b1, b2, 1, a1, a2);
  }

  void setAllpassPole(const Complex &pole) {
    Complex zero = Complex(1, 0) / pole;
    setZeroPolePairs(zero, pole);
  }

  void getFrequencyResponse(int nFrequencies, const float *frequency,
                            float *magResponse, float *phaseResponse) {
    // Evaluate the Z-transform of the filter at given normalized
    // frequency from 0 to 1.  (1 corresponds to the Nyquist
    // frequency.)
    //
    // The z-transform of the filter is
    //
    // H(z) = (b0 + b1*z^(-1) + b2*z^(-2))/(1 + a1*z^(-1) + a2*z^(-2))
    //
    // Evaluate as
    //
    // b0 + (b1 + b2*z1)*z1
    // --------------------
    // 1 + (a1 + a2*z1)*z1
    //
    // with z1 = 1/z and z = exp(j*pi*frequency). Hence z1 =
    // exp(-j*pi*frequency)

    // Make local copies of the coefficients as a micro-optimization.
    double b0 = m_b0;
    double b1 = m_b1;
    double b2 = m_b2;
    double a1 = m_a1;
    double a2 = m_a2;

    for (int k = 0; k < nFrequencies; ++k) {
      double omega = -M_PI * frequency[k];
      Complex z = Complex(cos(omega), sin(omega));
      Complex numerator = b0 + (b1 + b2 * z) * z;
      Complex denominator = Complex(1, 0) + (a1 + a2 * z) * z;
      // Strangely enough, using complex division:
      // e.g. Complex response = numerator / denominator;
      // fails on our test machines, yielding infinities and NaNs, so we do
      // things the long way here.
      double n = norm(denominator);
      double r = (real(numerator) * real(denominator) +
                  imag(numerator) * imag(denominator)) /
                 n;
      double i = (imag(numerator) * real(denominator) -
                  real(numerator) * imag(denominator)) /
                 n;
      std::complex<double> response = std::complex<double>(r, i);

      magResponse[k] = static_cast<float>(abs(response));
      phaseResponse[k] =
          static_cast<float>(atan2(imag(response), real(response)));
    }
  }
  bool hasTail() const { return m_y1 || m_y2 || m_x1 || m_x2; }

private:
  void set_normalized_coefficients(double b0, double b1, double b2, double a0,
                                   double a1, double a2) {
    double a0Inverse = 1 / a0;

    m_b0 = b0 * a0Inverse;
    m_b1 = b1 * a0Inverse;
    m_b2 = b2 * a0Inverse;
    m_a1 = a1 * a0Inverse;
    m_a2 = a2 * a0Inverse;
  }

  // Filter coefficients. The filter is defined as
  //
  // y[n] + m_a1*y[n-1] + m_a2*y[n-2] = m_b0*x[n] + m_b1*x[n-1] + m_b2*x[n-2].
  double m_b0;
  double m_b1;
  double m_b2;
  double m_a1;
  double m_a2;

  // Filter memory
  //
  // Double precision for the output values is valuable because errors can
  // accumulate.  Input values are also stored as double so they need not be
  // converted again for computation.
  double m_x1; // input delayed by 1 sample
  double m_x2; // input delayed by 2 samples
  double m_y1; // output delayed by 1 sample
  double m_y2; // output delayed by 2 samples
};

#endif // BIQUAD_H
