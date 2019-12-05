// Send audio from a producer to a consumer, lock free, no gc. (after an initial
// setup), between a UI thread (browser main thread or worker) and a real-time
// thread (in an AudioWorkletProcessor). Write and Reader cannot change role
// after setup, unless externally synchronized.
//
// Audio can be multi-channel is always float32, is stored interleaved: for a
// stereo audio stream, the samples are stored like so:
//
// Array index | 0 1 2 3 4 5 6 7 8 9
// Sample      | L R L R L R L R L R
//
// Static methods allow converting from interleaved to planar and the opposite,
// provided two arrays.

class AudioStreamProducer {
  // From a RingBuffer and a channel count, build a object capable of producing
  // audio, to be later consumed by a consumer.
  constructor(ringbuf, channel_count) {
    this.ringbuf = ringbuf;
    this.channel_count = channel_count;
    this.conversion_buffer = new Uint8Array(512);
  }
  // Enqueue interleaved audio. `buf` must be a Float32Array, and must have a
  // number of elements divisible by the number of channels this
  // AudioStreamProducer has been initialized with.
  //
  // Returns the number of elements written.
  enqueue_audio(input) {
    // Programming error: the input array doesn't have a correct number of
    // samples.
    if (!(input.length % this.channel_count)) {
      throw "Enqueued a incorrect number of samples";
    }
    if (this.ringbuf.available_write() < input.length) {
      return false;
    }
    return this.ringbuf.push(input);
  }
}

class AudioStreamConsumer {
  // From a RingBuffer and a channel count, build a object capable of getting
  // audio from a ring buffer, in packets of `packet_size` frames.
  constructor(ringbuf, channel_count, packet_size) {
    this.ringbuf = ringbuf;
    this.channel_count = channel_count;
  }
  dequeue_audio(output) {
    if (!(output.length % this.channel_count)) {
      throw "Trying to dequeue a incorrect numbre of samples";
    }

    return this.ringbuf.pop(output);
  }
}
