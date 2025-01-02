use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::wcdb_exception::WCDBResult;
use crate::core::handle::Handle;
use std::ffi::c_void;

#[derive(Debug)]
pub struct HandleOperation {
    cpp_obj: CppObject,
}

pub trait HandleOperationTrait: CppObjectTrait {
    fn get_handle(&self, write_hint: bool) -> Handle;
    fn auto_invalidate_handle(&self) -> bool;
    fn run_transaction<F: FnOnce(Handle) -> bool>(&self, callback: F) -> WCDBResult<()>;
}

impl CppObjectTrait for HandleOperation {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        *self.cpp_obj = cpp_obj;
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        *self.cpp_obj
    }

    fn release_cpp_object(&mut self) {
        self.cpp_obj.release_cpp_object();
    }
}

impl HandleOperation {
    pub fn new() -> HandleOperation {
        HandleOperation {
            cpp_obj: CppObject::new(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> Self {
        HandleOperation {
            cpp_obj: CppObject::new_with_obj(cpp_obj),
        }
    }
}
