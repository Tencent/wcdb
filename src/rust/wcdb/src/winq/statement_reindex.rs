use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use libc::c_int;
use std::ffi::{c_char, c_void};

extern "C" {
    fn WCDBRustStatementReindex_createCppObj() -> *mut c_void;

    fn WCDBRustStatementReindex_configCollation(cpp_obj: *mut c_void, collation: *const c_char);

    fn WCDBRustStatementReindex_configTable(cpp_obj: *mut c_void, table: *const c_char);

    fn WCDBRustStatementReindex_configIndex(cpp_obj: *mut c_void, index_name: *const c_char);

    fn WCDBRustStatementReindex_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        path: *const c_char,
    );
}

#[derive(Debug)]
pub struct StatementReindex {
    statement: Statement,
}

impl CppObjectTrait for StatementReindex {
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

impl CppObjectConvertibleTrait for StatementReindex {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementReindex {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementReindex {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementReindex {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementReindex {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementReindex_createCppObj() };
        StatementReindex {
            statement: Statement::new(CPPType::ReindexSTMT, Some(cpp_obj)),
        }
    }

    pub fn reindex_collation(&self, collation: &str) -> &Self {
        let c_str = collation.to_string().to_cstring();
        unsafe {
            WCDBRustStatementReindex_configCollation(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn reindex_table(&self, table: &str) -> &Self {
        let c_str = table.to_string().to_cstring();
        unsafe {
            WCDBRustStatementReindex_configTable(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn reindex(&self, index: &str) -> &Self {
        let c_str = index.to_string().to_cstring();
        unsafe {
            WCDBRustStatementReindex_configIndex(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        let c_str = schema_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementReindex_configSchema(
                self.get_cpp_obj(),
                CPPType::String as std::ffi::c_int,
                std::ptr::null_mut(),
                c_str.as_ptr(),
            )
        }
        self
    }

    pub fn of_with_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustStatementReindex_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as std::ffi::c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            )
        }
        self
    }
}
