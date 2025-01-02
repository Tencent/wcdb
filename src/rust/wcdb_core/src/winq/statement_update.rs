use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{
    CPPType, IdentifierStaticTrait, IdentifierTrait, WCDBRustWinq_isWriteStatement,
};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::c_void;
use std::fmt::Debug;

extern "C" {
    pub fn WCDBRustStatementUpdate_create() -> *mut c_void;
}

#[derive(Debug)]
pub struct StatementUpdate {
    statement: Statement,
}

impl CppObjectTrait for StatementUpdate {
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

impl IdentifierTrait for StatementUpdate {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementUpdate {
    fn get_type() -> i32 {
        CPPType::UpdateSTMT as i32
    }
}

impl StatementTrait for StatementUpdate {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementUpdate {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementUpdate_create() };
        StatementUpdate {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }
}
