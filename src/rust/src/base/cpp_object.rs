use std::ffi::c_void;
use std::ops::Deref;

extern "C" {
    pub fn WCDBRustBase_releaseObject(cpp_obj: *mut c_void);
}

pub struct CppObject {
    cpp_obj: *mut c_void,
}

impl CppObject {
    pub fn new(cpp_obj: *mut c_void) -> CppObject {
        CppObject { cpp_obj }
    }

    pub fn get(&self) -> *mut c_void {
        self.cpp_obj
    }
}

impl Deref for CppObject {
    type Target = *mut c_void;

    fn deref(&self) -> &Self::Target {
        &self.cpp_obj
    }
}

impl Drop for CppObject {
    fn drop(&mut self) {
        unsafe { WCDBRustBase_releaseObject(self.cpp_obj) };
    }
}
