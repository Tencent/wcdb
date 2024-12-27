use std::ffi::c_void;
use std::fmt::Debug;
use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, IdentifierTrait};
use crate::winq::statement::{Statement, StatementTrait};

extern "C" {
    pub fn WCDBRustStatementDelete_create() -> *mut c_void;
}

pub struct StatementDelete {
    statement: Statement,
}

impl Debug for StatementDelete {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "StatementInsert: {}",
            self.statement.identifier.get_description()
        )
    }
}

impl CppObjectTrait for StatementDelete {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.statement.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.statement.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.statement.release_cpp_object();
    }
}

impl IdentifierTrait for StatementDelete {
    fn get_type() -> i32 {
        CPPType::DeleteSTMT as i32
    }
}

impl StatementTrait for StatementDelete {}

impl StatementDelete {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDelete_create() };
        StatementDelete {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }
}
