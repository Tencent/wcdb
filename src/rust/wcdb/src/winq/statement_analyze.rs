use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_schema::StringSchema;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementAnalyze_createCppObj() -> *mut c_void;

    fn WCDBRustStatementAnalyze_toAnalyze(cpp_obj: *mut c_void);

    fn WCDBRustStatementAnalyze_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustStatementAnalyze_configTable(cpp_obj: *mut c_void, table_name: *const c_char);

    fn WCDBRustStatementAnalyze_configIndex(cpp_obj: *mut c_void, index_name: *const c_char);
}

#[derive(Debug)]
pub struct StatementAnalyze {
    statement: Statement,
}

impl CppObjectTrait for StatementAnalyze {
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

impl CppObjectConvertibleTrait for StatementAnalyze {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementAnalyze {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementAnalyze {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementAnalyze {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementAnalyze {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementAnalyze_createCppObj() };
        StatementAnalyze {
            statement: Statement::new(CPPType::AnalyzeSTMT, Some(cpp_obj)),
        }
    }

    pub fn analyze(&self) -> &Self {
        unsafe {
            WCDBRustStatementAnalyze_toAnalyze(self.get_cpp_obj());
        }
        self
    }

    pub fn schema<'a, T>(&self, schema: T) -> &Self
    where
        T: Into<StringSchema<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = schema.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        unsafe {
            WCDBRustStatementAnalyze_configSchema(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            );
        }
        self
    }

    pub fn table(&self, table_name: &str) -> &Self {
        let c_str = table_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementAnalyze_configTable(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn index(&self, index_name: &str) -> &Self {
        let c_str = index_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementAnalyze_configIndex(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }
}
