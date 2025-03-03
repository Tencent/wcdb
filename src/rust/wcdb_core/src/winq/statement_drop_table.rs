use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void, CString};

extern "C" {
    pub fn WCDBRustStatementDropTable_create() -> *mut c_void;
    pub fn WCDBRustStatementDropTable_configTableName(
        cpp_obj: *mut c_void,
        table_name: *const c_char,
    );
    pub fn WCDBRustStatementDropTable_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        schema_cpp_obj: *mut c_void,
        schema_name: *const c_char,
    );
    pub fn WCDBRustStatementDropTable_configIfExist(cpp_obj: *mut c_void);
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

impl IdentifierTrait for StatementDropTable {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementDropTable {
    fn get_type() -> i32 {
        CPPType::DropTableSTMT as i32
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
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn drop_table(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementDropTable_configTableName(self.get_cpp_obj(), c_table_name.as_ptr());
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        let c_schema_name = CString::new(schema_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementDropTable_configSchema(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                c_schema_name.as_ptr(),
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
