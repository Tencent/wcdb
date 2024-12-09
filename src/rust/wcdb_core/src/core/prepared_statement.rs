use crate::base::cpp_object::CppObject;
use std::ffi::c_void;

extern "C" {
    pub fn WCDBRustHandleStatement_getInteger(cpp_obj: *mut c_void, index: usize) -> i64;
}

pub struct PreparedStatement {
    cpp_obj: CppObject,
}

impl PreparedStatement {
    pub fn new(cpp_obj: *mut c_void) -> PreparedStatement {
        PreparedStatement {
            cpp_obj: CppObject::new_with_obj(cpp_obj),
        }
    }

    pub fn get_int(&self, index: usize) -> i32 {
        unsafe { WCDBRustHandleStatement_getInteger(*self.cpp_obj, index) as i32 }
    }
}
