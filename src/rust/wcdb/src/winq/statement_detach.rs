use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_schema::StringSchema;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementDetach_createCppObj() -> *mut c_void;

    fn WCDBRustStatementDetach_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );
}

#[derive(Debug)]
pub struct StatementDetach {
    statement: Statement,
}

impl CppObjectTrait for StatementDetach {
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

impl CppObjectConvertibleTrait for StatementDetach {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementDetach {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementDetach {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementDetach {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementDetach {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDetach_createCppObj() };
        StatementDetach {
            statement: Statement::new(CPPType::DetachSTMT, Some(cpp_obj)),
        }
    }

    pub fn detach<'a, T>(&self, schema: T) -> &Self
    where
        T: Into<StringSchema<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = schema.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        unsafe {
            WCDBRustStatementDetach_configSchema(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            );
        }
        self
    }
}
