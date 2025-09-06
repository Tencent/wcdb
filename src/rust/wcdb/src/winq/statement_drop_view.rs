use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementDropView_createCppObj() -> *mut c_void;

    fn WCDBRustStatementDropView_configView(cpp_obj: *mut c_void, view_name: *const c_char);

    fn WCDBRustStatementDropView_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );

    fn WCDBRustStatementDropView_configIfExist(cpp_obj: *mut c_void);
}

#[derive(Debug)]
pub struct StatementDropView {
    statement: Statement,
}

impl CppObjectTrait for StatementDropView {
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

impl CppObjectConvertibleTrait for StatementDropView {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementDropView {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementDropView {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementDropView {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementDropView {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDropView_createCppObj() };
        StatementDropView {
            statement: Statement::new(CPPType::DropViewSTMT, Some(cpp_obj)),
        }
    }

    pub fn drop_view(&self, view_name: &str) -> &Self {
        let c_str = view_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementDropView_configView(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        let c_str = schema_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementDropView_configSchema(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                std::ptr::null(),
                c_str.as_ptr(),
            );
        }
        self
    }

    pub fn of_with_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustStatementDropView_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            )
        }
        self
    }

    pub fn if_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementDropView_configIfExist(self.get_cpp_obj());
        }
        self
    }
}
