use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait, WCDBRustWinq_isWriteStatement};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::c_void;
use std::fmt::Debug;

#[derive(Debug)]
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

impl CppObjectConvertibleTrait for Statement {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for Statement {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for Statement {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

pub trait StatementTrait: IdentifierTrait {
    fn is_write_statement(&self) -> bool;
}

impl StatementTrait for Statement {
    fn is_write_statement(&self) -> bool {
        unsafe { WCDBRustWinq_isWriteStatement(self.get_cpp_obj()) }
    }
}

impl Statement {
    pub fn new(cpp_type: CPPType, cpp_obj_opt: Option<*mut c_void>) -> Statement {
        Statement {
            identifier: Identifier::new(cpp_type, cpp_obj_opt),
        }
    }
}
