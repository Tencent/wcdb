use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::Identifier;
use std::ffi::c_void;

pub(crate) struct ExpressionOperable {
    identifier: Identifier,
}

impl CppObjectTrait for ExpressionOperable {
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

impl ExpressionOperable {
    pub fn new() -> Self {
        ExpressionOperable {
            identifier: Identifier::new(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> Self {
        ExpressionOperable {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}
