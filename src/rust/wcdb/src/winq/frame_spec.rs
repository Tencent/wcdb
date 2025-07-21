use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::c_void;

extern "C" {
    fn WCDBRustFrameSpec_createCppObj() -> *mut c_void;
    fn WCDBRustFrameSpec_configRange(cpp_obj: *mut c_void);
    fn WCDBRustFrameSpec_configUnboundedPreceding(cpp_obj: *mut c_void);
}

#[derive(Debug)]
pub struct FrameSpec {
    identifier: Identifier,
}

impl CppObjectTrait for FrameSpec {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object();
    }
}

impl IdentifierTrait for FrameSpec {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for FrameSpec {
    fn get_type() -> i32 {
        CPPType::FrameSpec as i32
    }
}

impl FrameSpec {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustFrameSpec_createCppObj() };
        FrameSpec {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn range(self) -> Self {
        unsafe { WCDBRustFrameSpec_configRange(self.get_cpp_obj()) }
        self
    }

    pub fn unbounded_preceding(self) -> Self {
        unsafe { WCDBRustFrameSpec_configUnboundedPreceding(self.get_cpp_obj()) }
        self
    }
}
