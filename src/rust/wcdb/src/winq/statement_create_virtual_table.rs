use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use libc::c_int;
use std::ffi::{c_char, c_void};

extern "C" {
    fn WCDBRustStatementCreateVirtualTable_createCppObj() -> *mut c_void;

    fn WCDBRustStatementCreateVirtualTable_configTable(cpp_obj: *mut c_void, name: *const c_char);

    fn WCDBRustStatementCreateVirtualTable_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );

    fn WCDBRustStatementCreateVirtualTable_configIfNotExist(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateVirtualTable_configModule(
        cpp_obj: *mut c_void,
        module: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustStatementCreateVirtualTable_configArguments(
        cpp_obj: *mut c_void,
        argument: *const *const c_char,
        argument_len: c_size_t,
    ) -> *mut c_void;
}

#[derive(Debug)]
pub struct StatementCreateVirtualTable {
    statement: Statement,
}

impl CppObjectTrait for StatementCreateVirtualTable {
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

impl CppObjectConvertibleTrait for StatementCreateVirtualTable {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementCreateVirtualTable {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementCreateVirtualTable {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementCreateVirtualTable {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementCreateVirtualTable {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementCreateVirtualTable_createCppObj() };
        StatementCreateVirtualTable {
            statement: Statement::new(CPPType::CreateVirtualTableSTMT, Some(cpp_obj)),
        }
    }

    pub fn create_virtual_table(&self, name: &str) -> &Self {
        let c_str = name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateVirtualTable_configTable(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn of_with_string(&self, schema_name: &str) -> &Self {
        let c_str = schema_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateVirtualTable_configSchema(
                self.get_cpp_obj(),
                CPPType::String as std::ffi::c_int,
                std::ptr::null(),
                c_str.as_ptr(),
            );
        }
        self
    }

    pub fn of_with_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustStatementCreateVirtualTable_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as std::ffi::c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            )
        }
        self
    }

    pub fn if_not_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateVirtualTable_configIfNotExist(self.get_cpp_obj());
        }
        self
    }

    pub fn using_module(&self, module: &str) -> &Self {
        let c_str = module.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateVirtualTable_configModule(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn arguments(&self, arguments: Vec<String>) -> &Self {
        let mut c_string_array: Vec<*const c_char> = Vec::with_capacity(arguments.len());
        for x in arguments {
            c_string_array.push(x.to_cstring().into_raw());
        }
        unsafe {
            WCDBRustStatementCreateVirtualTable_configArguments(
                self.get_cpp_obj(),
                c_string_array.as_ptr(),
                c_string_array.len(),
            );
        }
        self
    }
}
