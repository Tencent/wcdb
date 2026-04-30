use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_void};

extern "C" {
    fn WCDBRustStatementRollback_createCppObj() -> *mut c_void;

    fn WCDBRustStatementRollback_configSavepoint(cpp_obj: *mut c_void, savepoint: *const c_char);
}

#[derive(Debug)]
pub struct StatementRollback {
    statement: Statement,
}

impl CppObjectTrait for StatementRollback {
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

impl CppObjectConvertibleTrait for StatementRollback {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementRollback {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementRollback {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementRollback {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementRollback {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementRollback_createCppObj() };
        StatementRollback {
            statement: Statement::new(CPPType::RollbackSTMT, Some(cpp_obj)),
        }
    }

    pub fn rollback_to(&self, savepoint: &str) -> &Self {
        let c_str = savepoint.to_string().to_cstring();
        unsafe {
            WCDBRustStatementRollback_configSavepoint(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }
}
