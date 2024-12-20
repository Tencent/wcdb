use std::ffi::c_void;
use crate::winq::identifier::Identifier;

pub struct Statement {
    identifier: Identifier,
}

pub trait StatementTrait {
    
}

impl Statement {
    pub fn new_with_obj(cpp_obj: *mut c_void) -> Statement {
        Statement {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}