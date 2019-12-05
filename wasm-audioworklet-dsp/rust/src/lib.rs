#[macro_use]
extern crate lazy_static;

pub mod allpass;
pub mod biquad;
pub mod delay_line;
pub mod filter;
pub mod softclip;
pub mod utils;

use crate::delay_line::DelayLine;
use crate::allpass::Allpass;
use crate::filter::Filter;
use crate::softclip::Softclip;
use crate::utils::{coprime_with_progression, hadamard, matrix_vector_multiply};
use std::sync::Mutex;

pub struct Processor {
    pub load: f32,
    pub delay: DelayLine,
}

impl Processor {
    pub fn new() -> Processor {
        let mut delay = DelayLine::new(44100);
        delay.set_duration(44100 / 2); // 100ms
        Processor {
            load: 1.0,
            delay,
        }
    }

    pub fn process(&mut self, in_ptr: *mut f32, out_ptr: *mut f32, size: usize) {
        let in_buf: &mut [f32] = unsafe { std::slice::from_raw_parts_mut(in_ptr, size) };
        let out_buf: &mut [f32] = unsafe { std::slice::from_raw_parts_mut(out_ptr, size) };

        let mut delayed = 0.0;
        for i in 0..size {
            self.delay.write(in_buf[i]);
            self.delay.read(&mut delayed);
            out_buf[i] = in_buf[i] + delayed;
        }
    }
}

#[no_mangle]
pub extern "C" fn alloc(size: usize) -> *mut f32 {
    let mut buf = Vec::<f32>::with_capacity(size);
    let ptr = buf.as_mut_ptr();
    std::mem::forget(buf);
    ptr as *mut f32
}

lazy_static! {
    static ref PROCESSOR: Mutex<Processor> = Mutex::new(Processor::new());
}

#[no_mangle]
pub extern "C" fn process(in_ptr: *mut f32, out_ptr: *mut f32, size: usize) {
    let mut processor = PROCESSOR.lock().unwrap();
    processor.process(in_ptr, out_ptr, size);
}

#[no_mangle]
pub extern "C" fn set_load(load: f32) {
    let mut processor = PROCESSOR.lock().unwrap();
    processor.load = load;
}
