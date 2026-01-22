use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::bind_parameter::BindParameter;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use libc::c_int;
use std::ffi::{c_char, c_void};

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

    pub fn attach_with_string(&self, path: &str) -> &Self {
        let c_str = path.to_string().to_cstring();
        unsafe {
            WCDBRustStatementAttach_configPath(
                self.get_cpp_obj(),
                CPPType::String as std::ffi::c_int,
                std::ptr::null(),
                c_str.as_ptr(),
            );
        }
        self
    }

    pub fn attach_with_bind_parameter(&self, bind_parameter: BindParameter) -> &Self {
        unsafe {
            WCDBRustStatementAttach_configPath(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&bind_parameter) as std::ffi::c_int,
                CppObject::get(&bind_parameter),
                std::ptr::null(),
            );
        }
        self
    }

    pub fn as_with_name(&self, schema_name: &str) -> &Self {
        let c_str = schema_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementAttach_configSchema(
                self.get_cpp_obj(),
                CPPType::String as std::ffi::c_int,
                std::ptr::null(),
                c_str.as_ptr(),
            );
        }
        self
    }

    pub fn as_with_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustStatementAttach_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as std::ffi::c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            )
        }
        self
    }

    pub fn key_with_name(&self, key: &str) -> &Self {
        let c_str = key.to_string().to_cstring();
        unsafe {
            WCDBRustStatementAttach_configKey(
                self.get_cpp_obj(),
                CPPType::String as std::ffi::c_int,
                std::ptr::null(),
                c_str.as_ptr(),
            );
        }
        self
    }

    pub fn key_with_bind_parameter(&self, bind_parameter: BindParameter) -> &Self {
        unsafe {
            WCDBRustStatementAttach_configKey(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&bind_parameter) as c_int,
                CppObject::get(&bind_parameter),
                std::ptr::null(),
            );
        }
        self
    }
}
