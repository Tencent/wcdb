use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::winq::identifier::{CPPType, IdentifierTrait};
use crate::winq::pragma::Pragma;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_float, c_int, c_long, c_void};
use std::ptr::null;

extern "C" {
    fn WCDBRustStatementPragma_create() -> *mut c_void;

    fn WCDBRustStatementPragma_configPragma(
        cpp_obj: *mut c_void,
        pragma: *mut c_void,
    ) -> *mut c_void;

    fn WCDBRustStatementPragma_configToValue(
        cpp_obj: *mut c_void,
        val_type: c_int,
        long_value: c_long,
        double_value: c_float,
        string_value: *const c_char,
    );
}

pub struct StatementPragma {
    statement: Statement,
}

impl IdentifierTrait for StatementPragma {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl CppObjectTrait for StatementPragma {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.statement.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.statement.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.statement.release_cpp_object()
    }
}

impl StatementTrait for StatementPragma {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementPragma {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementPragma_create() };
        StatementPragma {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn pragma(&self, pragma: Pragma) -> &StatementPragma {
        unsafe {
            WCDBRustStatementPragma_configPragma(
                self.statement.get_cpp_obj(),
                CppObject::get(&pragma),
            )
        };
        self
    }

    pub fn to_value(&self, value: i32) -> &StatementPragma {
        unsafe {
            WCDBRustStatementPragma_configToValue(
                self.statement.get_cpp_obj(),
                CPPType::Int as c_int,
                value as c_long,
                0 as c_float,
                null(),
            );
        }
        self
    }

    pub fn to_value_bool(&self, value: bool) -> &StatementPragma {
        let value = if value { 1 } else { 0 } as c_long;
        unsafe {
            WCDBRustStatementPragma_configToValue(
                self.statement.get_cpp_obj(),
                CPPType::Bool as c_int,
                value,
                0 as c_float,
                null(),
            );
        }
        self
    }
}
