use std::ffi::c_void;
use std::ops::{Deref, DerefMut};

extern "C" {
    pub fn WCDBRustBase_releaseObject(cpp_obj: *mut c_void);
}

pub struct CppObject {
    cpp_obj: *mut c_void,
}

impl Deref for CppObject {
    type Target = *mut c_void;

    fn deref(&self) -> &Self::Target {
        &self.cpp_obj
    }
}

impl DerefMut for CppObject {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.cpp_obj
    }
}

impl Drop for CppObject {
    fn drop(&mut self) {
        unsafe { WCDBRustBase_releaseObject(self.cpp_obj) };
    }
}

impl CppObject {
    pub fn new() -> CppObject {
        CppObject { cpp_obj: std::ptr::null_mut() }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> CppObject {
        CppObject { cpp_obj }
    }
}
