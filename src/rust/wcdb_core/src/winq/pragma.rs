use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::Identifier;
use std::ffi::{c_char, c_void, CString};

extern "C" {
    fn WCDBRustPragma_create(name: *const c_char) -> *mut c_void;
}

pub struct Pragma {
    identifier: Identifier,
}

impl CppObjectTrait for Pragma {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object()
    }
}

impl Pragma {
    pub fn new(name: String) -> Self {
        let c_name = CString::new(name).unwrap().into_raw();
        let cpp_obj = unsafe { WCDBRustPragma_create(c_name) };
        Pragma {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn user_version() -> Self {
        Pragma::new("user_version".to_string())
    }
}
