#[macro_use]
extern crate lazy_static;

use std::sync::Mutex;

pub struct Processor {
    pub load: f32,
}

impl Processor {
    pub fn new() -> Processor {
        Processor { load: 1.0 }
    }

    pub fn process(&mut self, in_ptr: *mut f32, out_ptr: *mut f32, size: usize) {
        let in_buf: &mut [f32] = unsafe { std::slice::from_raw_parts_mut(in_ptr, size) };
        let out_buf: &mut [f32] = unsafe { std::slice::from_raw_parts_mut(out_ptr, size) };

        for i in 0..size {
            out_buf[i] = in_buf[i];
        }
    }
}

#[no_mangle]
pub extern "C" fn alloc(size: usize) -> *mut f32 {
    let mut buf = Vec::<u8>::with_capacity(size);
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
