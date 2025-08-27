use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementDropIndex_createCppObj() -> *mut c_void;

    fn WCDBRustStatementDropIndex_configIndex(cpp_obj: *mut c_void, index_name: *const c_char);

    fn WCDBRustStatementDropIndex_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        schema_cpp_obj: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustStatementDropIndex_configIfExist(cpp_obj: *mut c_void);
}

pub struct StatementDropIndex {
    statement: Statement,
}

impl CppObjectTrait for StatementDropIndex {
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

impl CppObjectConvertibleTrait for StatementDropIndex {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementDropIndex {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementDropIndex {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementDropIndex {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementDropIndex {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDropIndex_createCppObj() };
        StatementDropIndex {
            statement: Statement::new(CPPType::DropIndexSTMT, Some(cpp_obj)),
        }
    }

    pub fn drop_index(&self, index_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementDropIndex_configIndex(
                self.get_cpp_obj(),
                index_name.to_cstring().as_ptr(),
            );
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementDropIndex_configSchema(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                schema_name.to_cstring().as_ptr(),
            );
        }
        self
    }

    // pub fn of_schema(&self, schema: Schema) -> &Self {
    //     self
    // }

    pub fn if_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementDropIndex_configIfExist(self.get_cpp_obj());
        }
        self
    }
}
