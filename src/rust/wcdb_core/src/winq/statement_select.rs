use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{
    CPPType, IdentifierStaticTrait, IdentifierTrait, WCDBRustWinq_isWriteStatement,
};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::c_void;
use std::fmt::Debug;

extern "C" {
    pub fn WCDBRustStatementSelect_create() -> *mut c_void;
}

pub struct StatementSelect {
    statement: Statement,
}

impl Debug for StatementSelect {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "StatementSelect: {}", self.get_description())
    }
}

impl CppObjectTrait for StatementSelect {
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

impl IdentifierTrait for StatementSelect {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementSelect {
    fn get_type() -> i32 {
        CPPType::SelectSTMT as i32
    }
}

impl StatementTrait for StatementSelect {
    fn is_write_statement(&self) -> bool {
        unsafe { WCDBRustWinq_isWriteStatement(self.get_cpp_obj()) }
    }
}

impl StatementSelect {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementSelect_create() };
        StatementSelect {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }
}
