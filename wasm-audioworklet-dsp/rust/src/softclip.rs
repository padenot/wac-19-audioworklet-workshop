pub struct Softclip {
    hardness: f32,
}

impl Softclip {
    pub fn new(hardness: f32) -> Softclip {
        Softclip { hardness }
    }
    pub fn set_hardness(&mut self, hardness: f32) {
        self.hardness = hardness;
    }
    pub fn process(&mut self, input: f32, output: &mut f32) {
        fn fast_tanh(x: f32) -> f32 {
            let x2 = x * x;
            let numerator = x * (135135. + x2 * (17325. + x2 * (378. + x2)));
            let denominator = 135135. + x2 * (62370. + x2 * (3150. + 28. * x2));
            return numerator / denominator;
        }
        *output = fast_tanh(self.hardness * input) / self.hardness;
        // alternative clipper
        // *output = input / (5. + (input*input)).sqrt();
    }
}
