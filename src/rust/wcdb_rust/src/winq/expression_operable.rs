use std::ffi::c_void;

use crate::winq::identifier::Identifier;

pub(crate) struct ExpressionOperable {
    identifier: Identifier,
}

impl ExpressionOperable {
    pub fn new_with_obj(cpp_obj: *mut c_void) -> ExpressionOperable {
        ExpressionOperable {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}
