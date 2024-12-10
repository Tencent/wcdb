use crate::winq::identifier::Identifier;
use std::ffi::c_void;

pub(crate) struct ExpressionOperable {
    identifier: Identifier,
}

impl ExpressionOperable {
    pub fn new_with_obj(cpp_obj: *mut c_void) -> Self {
        ExpressionOperable {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}

/// Identifier
impl ExpressionOperable {
    pub fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }
}
