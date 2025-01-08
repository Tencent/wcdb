use crate::base::cpp_object::CppObjectTrait;
use crate::orm::field::Field;
use crate::winq::identifier::{
    CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait, WCDBRustWinq_isWriteStatement,
};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_double, c_long, c_void, CString};
use std::fmt::Debug;

extern "C" {
    pub fn WCDBRustStatementSelect_create() -> *mut c_void;
    pub fn WCDBRustStatementSelect_configResultColumns(
        cpp_obj: *mut c_void,
        type_vec: *const i32,
        void_vec: *const *mut c_void,
        double_vec: *const c_double,
        string_vec: *const *const c_char,
        vec_len: i32,
    ) -> c_void;

    pub fn WCDBRustStatementSelect_configTableOrSubqueries(
        cpp_obj: *mut c_void,
        type_vec: *const i32,
        long_vec: *const c_long,
        double_vec: *const c_double,
        string_vec: *const *const c_char,
        vec_len: i32,
    ) -> c_void;
}

#[derive(Debug)]
pub struct StatementSelect {
    statement: Statement,
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
        self.statement.is_write_statement()
    }
}

impl StatementSelect {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementSelect_create() };
        StatementSelect {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn select<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }

        let mut types_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for field in fields {
            types_vec.push(Identifier::get_cpp_type((*field).get_column()));
            cpp_obj_vec.push((*field).get_cpp_obj());
        }

        unsafe {
            WCDBRustStatementSelect_configResultColumns(
                self.get_cpp_obj(),
                types_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                std::ptr::null(),
                types_vec.len() as i32,
            );
        }
        self
    }

    pub fn from(&self, table_name: &str) -> &Self {
        let types_vec = vec![CPPType::String as i32];

        let c_table_name = CString::new(table_name).unwrap_or_default();
        let str_vec = vec![c_table_name.as_ptr()];
        unsafe {
            WCDBRustStatementSelect_configTableOrSubqueries(
                self.get_cpp_obj(),
                types_vec.as_ptr(),
                std::ptr::null(),
                std::ptr::null(),
                str_vec.as_ptr(),
                types_vec.len() as i32,
            );
        }
        self
    }
}
