use std::ffi::{c_char, c_void, CString};
use std::ptr::null_mut;
use crate::base::cpp_object::CppObjectTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::{CPPType, IdentifierTrait};

extern "C" {
    pub fn WCDBRustColumn_createWithName(name: *const c_char, binding: *mut c_void) -> *mut c_void;
}

pub struct Column {
    expression_operable: ExpressionOperable,
}

impl CppObjectTrait for Column {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.expression_operable.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.expression_operable.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.expression_operable.release_cpp_object();
    }
}

impl IdentifierTrait for Column {
    fn get_type() -> i32 {
        CPPType::Column as i32
    }
}

impl Column {
    pub fn new(name: &str) -> Column {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), null_mut()) };
        Column {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_binding(name: &str, binding_raw: *mut c_void) -> Self {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), binding_raw) };
        Column {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }
}
