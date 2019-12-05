use crate::utils::*;
use std::f32::consts::PI;

pub struct Biquad {
    // Coefficients
    b0: f32,
    b1: f32,
    b2: f32,
    a1: f32,
    a2: f32,

    // Memory
    x1: f32,
    x2: f32,
    y1: f32,
    y2: f32,
}

// Based on the web audio api implem: https://webaudio.github.io/web-audio-api/#biquadfilternode
impl Biquad {
    pub fn new() -> Biquad {
        Biquad {
            b0: 1.0,
            b1: 0.0,
            b2: 0.0,
            a1: 0.0,
            a2: 0.0,
            x1: 0.0,
            x2: 0.0,
            y1: 0.0,
            y2: 0.0,
        }
    }
    pub fn reset(&mut self) {
        self.x1 = 0.0;
        self.x2 = 0.0;
        self.y1 = 0.0;
        self.y2 = 0.0;
    }
    pub fn set_lowpass_params(&mut self, cutoff: f32, resonance: f32) {
        let clamped_cutoff = clamp(cutoff, 0., 1.);

        if clamped_cutoff == 1. {
            // When cutoff is 1, the z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        } else if cutoff > 0. {
            // Compute biquad coefficients for lowpass filter
            let clamped_resonance = max(0.0, resonance); // can't go negative
            let g = 10.0f32.powf(-0.05 * clamped_resonance);
            let w0 = PI * cutoff;
            let cos_w0 = w0.cos();
            let alpha = 0.5 * w0.sin() * g;

            let b1 = 1.0 - cos_w0;
            let b0 = 0.5 * b1;
            let b2 = b0;
            let a0 = 1.0 + alpha;
            let a1 = -2.0 * cos_w0;
            let a2 = 1.0 - alpha;

            self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
        } else {
            // When cutoff is zero, nothing gets through the filter, so set
            // coefficients up correctly.
            self.set_normalized_coefficients(0., 0., 0., 1., 0., 0.);
        }
    }
    pub fn set_highpass_params(&mut self, cutoff: f32, resonance: f32) {
        // Limit cutoff to 0 to 1.
        let clamped_cutoff = clamp(cutoff, 0., 1.0);

        if clamped_cutoff == 1. {
            // The z-transform is 0.
            self.set_normalized_coefficients(0., 0., 0., 1., 0., 0.);
        } else if clamped_cutoff > 0. {
            // Compute biquad coefficients for highpass filter
            let clamped_resonance = max(0.0, resonance); // can't go negative
            let g = 10.0f32.powf(-0.05 * clamped_resonance);
            let w0 = PI * cutoff;
            let cos_w0 = w0.cos();
            let alpha = 0.5 * w0.sin() * g;

            let b1 = -1.0 - cos_w0;
            let b0 = -0.5 * b1;
            let b2 = b0;
            let a0 = 1.0 + alpha;
            let a1 = -2.0 * cos_w0;
            let a2 = 1.0 - alpha;

            self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
        } else {
            // When cutoff is zero, we need to be careful because the above
            // gives a quadratic divided by the same quadratic, with poles
            // and zeros on the unit circle in the same place. When cutoff
            // is zero, the z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        }
    }

    pub fn set_lowshelf_params(&mut self, frequency: f32, db_gain: f32) {
        let clamped_frequency = clamp(frequency, 0., 1.);

        let a = 10.0f32.powf(db_gain / 40.);

        if clamped_frequency == 1. {
            // The z-transform is a constant gain.
            self.set_normalized_coefficients(a * a, 0., 0., 1., 0., 0.);
        } else if clamped_frequency > 0. {
            let w0 = PI * clamped_frequency;
            let s = 1.; // filter slope (1 is max value)
            let alpha = 0.5 * w0.sin() * ((a + 1. / a) * (1. / s - 1.) + 2.).sqrt();
            let k = w0.cos();
            let k2 = 2. * a.sqrt() * alpha;
            let a_plus_one = a + 1.;
            let a_minus_one = a - 1.;

            let b0 = a * (a_plus_one - a_minus_one * k + k2);
            let b1 = 2. * a * (a_minus_one - a_plus_one * k);
            let b2 = a * (a_plus_one - a_minus_one * k - k2);
            let a0 = a_plus_one + a_minus_one * k + k2;
            let a1 = -2. * (a_minus_one + a_plus_one * k);
            let a2 = a_plus_one + a_minus_one * k - k2;

            self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
        } else {
            // When frequency is 0, the z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        }
    }

    pub fn set_highshelf_params(&mut self, frequency: f32, db_gain: f32) {
        // Clip frequencies to between 0 and 1, inclusive.
        let clamped_frequency = clamp(frequency, 0.0, 1.0);

        let a = 10.0f32.powf(db_gain / 40.);

        if clamped_frequency == 1. {
            // The z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        } else if clamped_frequency > 0. {
            let w0 = PI * frequency;
            let s = 1.; // filter slope (1 is max value)
            let alpha = 0.5 * w0.sin() * ((a + 1. / a) * (1. / s - 1.) + 2.).sqrt();
            let k = w0.cos();
            let k2 = 2. * a.sqrt() * alpha;
            let a_plus_one = a + 1.;
            let a_minus_one = a - 1.;

            let b0 = a * (a_plus_one + a_minus_one * k + k2);
            let b1 = -2. * a * (a_minus_one + a_plus_one * k);
            let b2 = a * (a_plus_one + a_minus_one * k - k2);
            let a0 = a_plus_one - a_minus_one * k + k2;
            let a1 = 2. * (a_minus_one - a_plus_one * k);
            let a2 = a_plus_one - a_minus_one * k - k2;

            self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
        } else {
            // When frequency = 0, the filter is just a gain, a^2.
            self.set_normalized_coefficients(a * a, 0., 0., 1., 0., 0.);
        }
    }

    pub fn set_peaking_params(&mut self, frequency: f32, q: f32, db_gain: f32) {
        // Clip frequencies to between 0 and 1, inclusive.
        let clamped_frequency = clamp(frequency, 0.0, 1.0);

        // Don't let q go negative, which causes an unstable filter.
        let clamped_q = max(0.0, q);

        let a = 10.0f32.powf(db_gain / 40.);

        if clamped_frequency > 0. && clamped_frequency < 1. {
            if clamped_q > 0. {
                let w0 = PI * clamped_frequency;
                let alpha = w0.sin() / (2. * q);
                let k = w0.cos();

                let b0 = 1. + alpha * a;
                let b1 = -2. * k;
                let b2 = 1. - alpha * a;
                let a0 = 1. + alpha / a;
                let a1 = -2. * k;
                let a2 = 1. - alpha / a;

                self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
            } else {
                // When q = 0, the above formulas have problems. If we look at
                // the z-transform, we can see that the limit as q->0 is a^2, so
                // set the filter that way.
                self.set_normalized_coefficients(a * a, 0., 0., 1., 0., 0.);
            }
        } else {
            // When frequency is 0 or 1, the z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        }
    }

    pub fn set_allpass_params(&mut self, frequency: f32, q: f32) {
        let clamped_frequency = clamp(frequency, 0.0, 1.0);

        let clamped_q = max(0.0, q);

        if clamped_frequency > 0. && clamped_frequency < 1. {
            if clamped_q > 0. {
                let w0 = PI * clamped_frequency;
                let alpha = w0.sin() / (2. * clamped_q);
                let k = w0.cos();

                let b0 = 1. - alpha;
                let b1 = -2. * k;
                let b2 = 1. + alpha;
                let a0 = 1. + alpha;
                let a1 = -2. * k;
                let a2 = 1. - alpha;

                self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
            } else {
                // When q = 0, the above formulas have problems. If we look at
                // the z-transform, we can see that the limit as q->0 is -1, so
                // set the filter that way.
                self.set_normalized_coefficients(-1., 0., 0., 1., 0., 0.);
            }
        } else {
            // When frequency is 0 or 1, the z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        }
    }

    pub fn set_notch_params(&mut self, frequency: f32, q: f32) {
        let clamped_frequency = clamp(frequency, 0.0, 1.0);

        let clamped_q = max(0.0, q);

        if clamped_frequency > 0. && clamped_frequency < 1. {
            if clamped_q > 0. {
                let w0 = PI * clamped_frequency;
                let alpha = w0.sin() / (2. * clamped_q);
                let k = w0.cos();

                let b0 = 1.;
                let b1 = -2. * k;
                let b2 = 1.;
                let a0 = 1. + alpha;
                let a1 = -2. * k;
                let a2 = 1. - alpha;

                self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
            } else {
                // When q = 0, the above formulas have problems. If we look at
                // the z-transform, we can see that the limit as q->0 is 0, so
                // set the filter that way.
                self.set_normalized_coefficients(0., 0., 0., 1., 0., 0.);
            }
        } else {
            // When frequency is 0 or 1, the z-transform is 1.
            self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
        }
    }

    pub fn set_bandpass_params(&mut self, frequency: f32, q: f32) {
        let clamped_frequency = max(0.0, frequency);

        let clamped_q = max(0.0, q);

        if clamped_frequency > 0. && clamped_frequency < 1. {
            let w0 = PI * clamped_frequency;
            if clamped_q > 0. {
                let alpha = w0.sin() / (2. * clamped_q);
                let k = w0.cos();

                let b0 = alpha;
                let b1 = 0.;
                let b2 = -alpha;
                let a0 = 1. + alpha;
                let a1 = -2. * k;
                let a2 = 1. - alpha;

                self.set_normalized_coefficients(b0, b1, b2, a0, a1, a2);
            } else {
                // When q = 0, the above formulas have problems. If we look at
                // the z-transform, we can see that the limit as q->0 is 1, so
                // set the filter that way.
                self.set_normalized_coefficients(1., 0., 0., 1., 0., 0.);
            }
        } else {
            // When the cutoff is zero, the z-transform approaches 0, if q
            // > 0. When both q and cutoff are zero, the z-transform is
            // pretty much undefined. What should we do in this case?
            // For now, just make the filter 0. When the cutoff is 1, the
            // z-transform also approaches 0.
            self.set_normalized_coefficients(0., 0., 0., 1., 0., 0.);
        }
    }
    pub fn process(&mut self, input: f32, output: &mut f32) {
        *output = self.b0 * input + self.b1 * self.x1 + self.b2 * self.x2
            - self.a1 * self.y1
            - self.a2 * self.y2;
        self.x2 = self.x1;
        self.x1 = input;
        self.y2 = self.y1;
        self.y1 = *output;
    }

    fn set_normalized_coefficients(
        &mut self,
        b0: f32,
        b1: f32,
        b2: f32,
        a0: f32,
        a1: f32,
        a2: f32,
    ) {
        let a0_inverse = 1. / a0;

        self.b0 = b0 * a0_inverse;
        self.b1 = b1 * a0_inverse;
        self.b2 = b2 * a0_inverse;
        self.a1 = a1 * a0_inverse;
        self.a2 = a2 * a0_inverse;
    }
}
