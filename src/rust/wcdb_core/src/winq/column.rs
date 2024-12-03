use std::ffi::{c_char, c_void, CString};
use std::ptr::null_mut;

use crate::winq::expression_operable::ExpressionOperable;

extern "C" {
    pub fn WCDBRustColumn_createWithName(name: *const c_char, binding: *mut c_void) -> *mut c_void;
}

pub struct Column {
    expression_operable: ExpressionOperable,
}

impl Column {
    pub fn new(name: &str) -> Column {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), null_mut()) };
        Column {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_binding(name: &str, table_binding: *mut c_void) -> Column {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), table_binding) };
        Column {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }
}
