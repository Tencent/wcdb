use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementDropTable_create() -> *mut c_void;

    fn WCDBRustStatementDropTable_configTableName(cpp_obj: *mut c_void, table_name: *const c_char);

    fn WCDBRustStatementDropTable_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        schema_cpp_obj: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustStatementDropTable_configIfExist(cpp_obj: *mut c_void);
}

pub struct StatementDropTable {
    statement: Statement,
}

impl CppObjectTrait for StatementDropTable {
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

impl CppObjectConvertibleTrait for StatementDropTable {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementDropTable {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementDropTable {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementDropTable {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementDropTable {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDropTable_create() };
        StatementDropTable {
            statement: Statement::new(CPPType::DropTableSTMT, Some(cpp_obj)),
        }
    }

    pub fn drop_table(&self, table_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementDropTable_configTableName(
                self.get_cpp_obj(),
                table_name.to_cstring().as_ptr(),
            );
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementDropTable_configSchema(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                schema_name.to_cstring().as_ptr(),
            )
        }
        self
    }

    // pub fn of_schema(&self, schema: Schema) -> &Self {
    //     self
    // }

    pub fn if_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementDropTable_configIfExist(self.get_cpp_obj());
        }
        self
    }
}
