#[macro_use]
extern crate lazy_static;

use std::sync::Mutex;

pub struct Processor {
    pub load: f32,
}

impl Processor {
    pub fn new() -> Processor {
        Processor {
            load: 1.0,
        }
    }

    pub fn process(&mut self, in_ptr: *mut f32, out_ptr: *mut f32, size: usize) {
        let in_buf: &mut [f32] = unsafe { std::slice::from_raw_parts_mut(in_ptr, size) };
        let out_buf: &mut [f32] = unsafe { std::slice::from_raw_parts_mut(out_ptr, size) };

        // Write input to output with a variable load, trying to defeat all compiler tricks by
        // writing n times the signal with a 1/n gain.
        let compensation_gain = 1. / (self.load * 1000.);
        let iterations = (self.load * 1000.) as u64;
        for _j in 0..iterations {
            for i in 0..size {
                out_buf[i] += compensation_gain * in_buf[i];
            }
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
