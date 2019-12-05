pub struct DelayLine {
    memory: Vec<f32>,
    duration: usize,
    read_index: usize,
    write_index: usize,
}

impl DelayLine {
    pub fn new(max_duration: usize) -> DelayLine {
        let mut v = Vec::<f32>::with_capacity(max_duration);
        v.resize(max_duration, 0.0);
        let mut d = DelayLine {
            memory: v,
            duration: 0,
            read_index: 0,
            write_index: max_duration - 1,
        };
        d.set_duration(max_duration / 2);
        return d;
    }
    pub fn set_duration(&mut self, duration: usize) {
        let d = if duration > self.memory.len() {
            println!("clipping duration in delay: {} >= {}", duration, self.memory.len());
            self.memory.len()
        } else {
            duration
        };
        self.duration = d;
        self.write_index = self.write_index % self.memory.len();
        self.read_index = if self.write_index > self.duration {
            self.write_index - self.duration
        } else {
            self.memory.len() - (self.duration - self.write_index)
        };

        println!("rd {} wr {} len {} duration{}", self.read_index, self.write_index, self.memory.len(), self.duration);
        //panic!("Ok");
    }
    pub fn write(&mut self, input: f32) {
        self.memory[self.write_index] = input;
        self.write_index = (self.write_index + 1) % self.memory.len()
    }
    pub fn read(&mut self, output: &mut f32) {
        *output = self.memory[self.read_index];
        self.read_index = (self.read_index + 1) % self.memory.len();
    }
    pub fn process(&mut self, input: f32, output: &mut f32) {
        self.write(input);
        self.read(output);
    }
}
