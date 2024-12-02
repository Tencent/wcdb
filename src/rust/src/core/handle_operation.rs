use std::ffi::c_void;

use crate::base::cpp_object::CppObject;

pub struct HandleOperation {
    cpp_obj: CppObject,
}

impl HandleOperation {}

impl HandleOperation {
    pub fn new() -> HandleOperation {
        HandleOperation {
            cpp_obj: CppObject::new(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> HandleOperation {
        HandleOperation {
            cpp_obj: CppObject::new_with_obj(cpp_obj),
        }
    }

    pub fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        *self.cpp_obj = cpp_obj;
    }

    pub fn get_cpp_obj(&self) -> *mut c_void {
        *self.cpp_obj
    }
}
