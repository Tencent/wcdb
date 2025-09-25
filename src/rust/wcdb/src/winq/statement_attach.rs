use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_bind_parameter::StringBindParameter;
use crate::base::param::enum_string_schema::StringSchema;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementAttach_createCppObj() -> *mut c_void;

    fn WCDBRustStatementAttach_configPath(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );

    fn WCDBRustStatementAttach_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );

    fn WCDBRustStatementAttach_configKey(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );
}

#[derive(Debug)]
pub struct StatementAttach {
    statement: Statement,
}

impl CppObjectTrait for StatementAttach {
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

impl CppObjectConvertibleTrait for StatementAttach {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementAttach {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementAttach {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementAttach {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementAttach {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementAttach_createCppObj() };
        StatementAttach {
            statement: Statement::new(CPPType::AttachSTMT, Some(cpp_obj)),
        }
    }

    pub fn attach<'a, T>(&self, param: T) -> &Self
    where
        T: Into<StringBindParameter<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = param.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        unsafe {
            WCDBRustStatementAttach_configPath(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            );
        }
        self
    }

    pub fn as_<'a, T>(&self, schema: T) -> &Self
    where
        T: Into<StringSchema<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = schema.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        unsafe {
            WCDBRustStatementAttach_configSchema(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            );
        }
        self
    }

    pub fn key<'a, T>(&self, param: T) -> &Self
    where
        T: Into<StringBindParameter<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = param.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        unsafe {
            WCDBRustStatementAttach_configKey(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            );
        }
        self
    }
}
