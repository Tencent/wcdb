use crate::base::cpp_object::CppObjectTrait;
use crate::orm::field::Field;
use crate::winq::identifier::{CPPType, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void, CString};
use std::fmt::Debug;
use std::ptr::null_mut;

extern "C" {
    pub fn WCDBRustStatementUpdate_create() -> *mut c_void;
    pub fn WCDBRustStatementUpdate_configTable(
        cpp_obj: *mut c_void,
        type_i: c_int,
        table: *mut c_void,
        table_name: *const c_char,
    );
    pub fn WCDBRustStatementUpdate_configColumnsToBindParameters(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *mut c_char,
        columns_vec_len: c_int,
    );
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

    pub fn update(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementUpdate_configTable(
                self.get_cpp_obj(),
                CPPType::String as i32,
                null_mut(),
                c_table_name.as_ptr(),
            );
        }
        self
    }

    pub fn set_columns_to_bind_parameters<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }
        let columns_void_vec_len = fields.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(fields.len());
        for field in fields {
            c_void_vec.push(field.get_cpp_obj());
        }

        unsafe {
            WCDBRustStatementUpdate_configColumnsToBindParameters(
                self.get_cpp_obj(),
                CPPType::Column as i32,
                c_void_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }
}
