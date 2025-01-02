use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::winq::expression::Expression;
use crate::winq::identifier::{
    CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait, WCDBRustWinq_isWriteStatement,
};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void, CString};
use std::fmt::Debug;
use std::os::raw::c_long;

extern "C" {
    pub fn WCDBRustStatementDelete_create() -> *mut c_void;
    pub fn WCDBRustStatementDelete_configTable(
        cpp_obj: *mut c_void,
        type_i: c_int,
        table: c_long,
        table_name: *const c_char,
    ) -> c_void;

    pub fn WCDBRustStatementDelete_configCondition(
        cpp_obj: *mut c_void,
        condition: *mut c_void,
    ) -> c_void;
}

pub struct StatementDelete {
    statement: Statement,
}

impl Debug for StatementDelete {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "StatementDelete: {}",
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

impl IdentifierTrait for StatementDelete {}

impl IdentifierStaticTrait for StatementDelete {
    fn get_type() -> i32 {
        CPPType::DeleteSTMT as i32
    }
}

impl StatementTrait for StatementDelete {
    fn is_write_statement(&self) -> bool {
        unsafe { WCDBRustWinq_isWriteStatement(self.get_cpp_obj()) }
    }
}

impl StatementDelete {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDelete_create() };
        StatementDelete {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn delete_from(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementDelete_configTable(
                self.get_cpp_obj(),
                CPPType::String as i32,
                0,
                c_table_name.as_ptr(),
            );
        }
        self
    }

    pub fn where_expression(&self, condition: Expression) -> &Self {
        unsafe {
            WCDBRustStatementDelete_configCondition(
                self.get_cpp_obj(),
                CppObject::get(condition.get_expression_operable()),
            );
        }
        self
    }
}
