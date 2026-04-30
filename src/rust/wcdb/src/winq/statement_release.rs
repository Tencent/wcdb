use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_void};

extern "C" {
    fn WCDBRustStatementRelease_createCppObj() -> *mut c_void;

    fn WCDBRustStatementRelease_configSavepoint(cpp_obj: *mut c_void, savepoint: *const c_char);
}

#[derive(Debug)]
pub struct StatementRelease {
    statement: Statement,
}

impl CppObjectTrait for StatementRelease {
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

impl CppObjectConvertibleTrait for StatementRelease {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementRelease {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementRelease {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementRelease {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementRelease {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementRelease_createCppObj() };
        StatementRelease {
            statement: Statement::new(CPPType::ReleaseSTMT, Some(cpp_obj)),
        }
    }

    pub fn release(&self, savepoint: &str) -> &Self {
        let c_str = savepoint.to_string().to_cstring();
        unsafe {
            WCDBRustStatementRelease_configSavepoint(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }
}
