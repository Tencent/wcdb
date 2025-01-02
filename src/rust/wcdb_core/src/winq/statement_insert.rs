use crate::base::cpp_object::CppObjectTrait;
use crate::orm::field::Field;
use crate::winq::identifier::{
    CPPType, IdentifierStaticTrait, IdentifierTrait, WCDBRustWinq_isWriteStatement,
};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_void, CString};
use std::fmt::Debug;

extern "C" {
    pub fn WCDBRustStatementInsert_create() -> *mut c_void;
    pub fn WCDBRustStatementInsert_configTableName(
        cpp_obj: *mut c_void,
        table_name: *const c_char,
    ) -> *mut c_void;
    pub fn WCDBRustStatementInsert_configColumns(
        cpp_obj: *mut c_void,
        columns_type: i32,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *mut c_char,
        columns_vec_len: i32,
    );
    pub fn WCDBRustStatementInsert_configValuesWithBindParameters(cpp_obj: *mut c_void, count: i32);
}

#[derive(Debug)]
pub struct StatementInsert {
    statement: Statement,
}

impl CppObjectTrait for StatementInsert {
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

impl IdentifierTrait for StatementInsert {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementInsert {
    fn get_type() -> i32 {
        CPPType::InsertSTMT as i32
    }
}

impl StatementTrait for StatementInsert {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementInsert {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementInsert_create() };
        StatementInsert {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn insert_into(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementInsert_configTableName(self.get_cpp_obj(), c_table_name.as_ptr());
        }
        self
    }

    pub fn columns<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }
        let columns_void_vec_len = fields.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(fields.len());
        for field in fields {
            c_void_vec.push(field.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementInsert_configColumns(
                self.get_cpp_obj(),
                CPPType::Column as i32,
                c_void_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }

    pub fn values_with_bind_parameters(&self, parameters_count: usize) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configValuesWithBindParameters(
                self.get_cpp_obj(),
                parameters_count as i32,
            )
        };
        self
    }
}
