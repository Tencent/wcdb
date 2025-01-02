use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{Identifier, IdentifierTrait};
use std::ffi::c_void;
use std::fmt::Debug;

pub struct Statement {
    identifier: Identifier,
}

impl CppObjectTrait for Statement {
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

impl IdentifierTrait for Statement {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

pub trait StatementTrait: IdentifierTrait + Debug {
    fn is_write_statement(&self) -> bool;
}

impl Statement {
    pub fn new_with_obj(cpp_obj: *mut c_void) -> Statement {
        Statement {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}
