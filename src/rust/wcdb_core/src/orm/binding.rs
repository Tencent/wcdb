use std::ffi::{c_char, c_void};

use crate::base::cpp_object::CppObject;
use crate::core::handle::Handle;
use crate::utils::ToCString;

extern "C" {
    pub fn WCDBRustBinding_create() -> *mut c_void;
    pub fn WCDBRustBinding_createTable(
        cpp_obj: *mut c_void,
        path: *const c_char,
        handle: *mut c_void,
    ) -> bool;
}

pub struct Binding {
    cpp_obj: CppObject,
}

impl Binding {
    // pub fn new() -> Binding {
    //     Binding { cpp_obj: CppObject::new(unsafe { WCDBRustBinding_create() }) }
    // }

    pub fn create_table(&self, table_name: &str, handle: Handle) -> bool {
        let c_table_name = table_name.to_cstring();
        unsafe {
            WCDBRustBinding_createTable(
                *self.cpp_obj,
                c_table_name.as_ptr(),
                handle.get_cpp_handle(),
            )
        }
    }
}
