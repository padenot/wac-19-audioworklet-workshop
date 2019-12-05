#ifndef DELAY_LINE_H
#define DELAY_LINE_H
#include <stdio.h>

#include <stdint.h>
#include <vector>

class DelayLine {
public:
    DelayLine(size_t max_duration)
    {
      memory.resize(max_duration, 0.0);
      write_index = max_duration - 1;
      read_index = 0;
      duration = max_duration;

      set_duration(max_duration);
    }

    void set_duration(size_t duration)
    {
      if (duration > memory.size()) {
        duration = memory.size();
      }
      this->duration = duration;
      write_index = write_index % memory.size();
      if (write_index > this->duration) {
        read_index = write_index - duration;
      } else {
        read_index = memory.size() - (duration - write_index);
      };
    }

    void write(float input) {
      memory[write_index] = input;
      write_index = (write_index + 1) % memory.size();
    }

    void read(float* output) {
      *output = memory[read_index];
      read_index = (read_index + 1) % memory.size();
    }

    void process(float input, float* output) {
        write(input);
        read(output);
    }
  private:
    std::vector<float> memory;
    size_t duration{0};
    size_t read_index{0};
    size_t write_index{0};
};

#endif // DELAY_LINE_H
