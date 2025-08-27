use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column_def::ColumnDef;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementCreateTable_create() -> *mut c_void;

    fn WCDBRustStatementCreateTable_configTableName(
        cpp_obj: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustStatementCreateTable_configColumns(
        cpp_obj: *mut c_void,
        columns_void_vec: *const *mut c_void,
        columns_vec_len: c_int,
    );
}

pub struct StatementCreateTable {
    statement: Statement,
}

impl CppObjectTrait for StatementCreateTable {
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

impl CppObjectConvertibleTrait for StatementCreateTable {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementCreateTable {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementCreateTable {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementCreateTable {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementCreateTable {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementCreateTable_create() };
        StatementCreateTable {
            statement: Statement::new(CPPType::CreateTableSTMT, Some(cpp_obj)),
        }
    }

    pub fn create_table(&self, table_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementCreateTable_configTableName(
                self.get_cpp_obj(),
                table_name.to_cstring().as_ptr(),
            );
        }
        self
    }

    pub fn define(&self, column_defs: Vec<&ColumnDef>) -> &Self {
        if column_defs.is_empty() {
            return self;
        }
        let len = column_defs.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(column_defs.len());
        for column_def in column_defs {
            c_void_vec.push(column_def.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementCreateTable_configColumns(
                self.get_cpp_obj(),
                c_void_vec.as_ptr(),
                len,
            );
        }
        self
    }
}
