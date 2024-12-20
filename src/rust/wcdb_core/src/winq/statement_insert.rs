use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::c_void;

extern "C" {
    pub fn WCDBRustStatementInsert_create() -> *mut c_void;
}

pub struct StatementInsert {
    statement: Statement,
}

impl StatementTrait for StatementInsert {}

impl StatementInsert {
    pub fn new() -> StatementInsert {
        let cpp_obj = unsafe { WCDBRustStatementInsert_create() };
        StatementInsert {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }
}
