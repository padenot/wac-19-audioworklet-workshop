extern "C" {
// base of the WASM heap
extern char __heap_base;
static float load = 1.0;

float *alloc(unsigned long size) {
  static char *bump_ptr = &__heap_base;
  void *previous = bump_ptr;
  bump_ptr += size;
  return (float *)previous;
}

void set_load(float new_load)
{
  load = new_load;
}

void process(float *input_buffer, float *output_buffer, unsigned long size) {
  for (unsigned long i = 0; i < size; i++) {
    output_buffer[i] = input_buffer[i];
  }
}
};
