use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementVacuum_createCppObj() -> *mut c_void;

    fn WCDBRustStatementVacuum_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        path: *const c_char,
    );
}

#[derive(Debug)]
pub struct StatementVacuum {
    statement: Statement,
}

impl CppObjectTrait for StatementVacuum {
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

impl CppObjectConvertibleTrait for StatementVacuum {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementVacuum {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementVacuum {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementVacuum {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementVacuum {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementVacuum_createCppObj() };
        StatementVacuum {
            statement: Statement::new(CPPType::VacuumSTMT, Some(cpp_obj)),
        }
    }

    pub fn vacuum(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustStatementVacuum_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            );
        }
        self
    }

    pub fn vacuum_with_string(&self, schema_name: &str) -> &Self {
        let c_str = schema_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementVacuum_configSchema(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                std::ptr::null_mut(),
                c_str.as_ptr(),
            );
        }
        self
    }
}
