use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::column::Column;
use crate::winq::column_def::ColumnDef;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void, CString};

extern "C" {
    fn WCDBRustStatementAlterTable_createCppObj() -> *mut c_void;

    fn WCDBRustStatementAlterTable_configTable(cpp_obj: *mut c_void, table_name: *const c_char);

    fn WCDBRustStatementAlterTable_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        schema_cpp_obj: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustStatementAlterTable_configRenameToTable(
        cpp_obj: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustStatementAlterTable_configRenameColumn(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        column_cpp_obj: *mut c_void,
        column_name: *const c_char,
    );

    fn WCDBRustStatementAlterTable_configRenameToColumn(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        column_cpp_obj: *mut c_void,
        column_name: *const c_char,
    );

    fn WCDBRustStatementAlterTable_configAddColumn(
        cpp_obj: *mut c_void,
        column_def_cpp_obj: *mut c_void,
    );
}

pub struct StatementAlterTable {
    statement: Statement,
}

impl CppObjectTrait for StatementAlterTable {
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

impl CppObjectConvertibleTrait for StatementAlterTable {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementAlterTable {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementAlterTable {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementAlterTable {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementAlterTable {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementAlterTable_createCppObj() };
        StatementAlterTable {
            statement: Statement::new(CPPType::AlterTableSTMT, Some(cpp_obj)),
        }
    }

    pub fn alter_table(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementAlterTable_configTable(self.get_cpp_obj(), c_table_name.as_ptr());
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        let c_schema_name = CString::new(schema_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementAlterTable_configSchema(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                c_schema_name.as_ptr(),
            );
        }
        self
    }

    // pub fn of_schema(&self, schema: Schema) -> &Self {
    //     self
    // }

    pub fn rename_to(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementAlterTable_configRenameToTable(
                self.get_cpp_obj(),
                c_table_name.as_ptr(),
            );
        }
        self
    }

    pub fn rename_column_by_name(&self, column_name: &str) -> &Self {
        let c_column_name = CString::new(column_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementAlterTable_configRenameColumn(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                c_column_name.as_ptr(),
            );
        }
        self
    }

    pub fn rename_column(&self, column: &Column) -> &Self {
        unsafe {
            WCDBRustStatementAlterTable_configRenameColumn(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(column) as c_int,
                column.get_cpp_obj(),
                std::ptr::null(),
            );
        }
        self
    }

    pub fn to_column_by_name(&self, column_name: &str) -> &Self {
        let c_column_name = CString::new(column_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementAlterTable_configRenameToColumn(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                c_column_name.as_ptr(),
            );
        }
        self
    }

    pub fn to_column(&self, column: &Column) -> &Self {
        unsafe {
            WCDBRustStatementAlterTable_configRenameToColumn(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(column) as c_int,
                column.get_cpp_obj(),
                std::ptr::null(),
            );
        }
        self
    }

    pub fn add_column(&self, column_def: &ColumnDef) -> &Self {
        unsafe {
            WCDBRustStatementAlterTable_configAddColumn(
                self.get_cpp_obj(),
                column_def.get_cpp_obj(),
            );
        }
        self
    }
}
