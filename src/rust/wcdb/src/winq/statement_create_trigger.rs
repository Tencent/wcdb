use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::StringColumnParam;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use crate::winq::statement_delete::StatementDelete;
use crate::winq::statement_insert::StatementInsert;
use crate::winq::statement_select::StatementSelect;
use crate::winq::statement_update::StatementUpdate;
use libc::c_int;
use std::ffi::{c_char, c_void, CString};

extern "C" {
    fn WCDBRustStatementCreateTrigger_createCppObj() -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configTrigger(
        cpp_obj: *mut c_void,
        name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configTemp(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configIfNotExist(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configBefore(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configAfter(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configInsteadOf(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configDelete(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configInsert(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configUpdate(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configColumns(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const *mut c_void,
        object_len: c_int,
        column_names: *const *const c_char,
        column_names_len: c_int,
    ) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configTable(
        cpp_obj: *mut c_void,
        table: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_configForEachRow(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustStatementCreateTrigger_configWhen(
        cpp_obj: *mut c_void,
        condition: *const c_void,
    ) -> *mut c_void;

    fn WCDBRustStatementCreateTrigger_executeInsert(
        cpp_obj: *mut c_void,
        insert: *const c_void,
    ) -> *mut c_void;
    fn WCDBRustStatementCreateTrigger_executeUpdate(
        cpp_obj: *mut c_void,
        insert: *const c_void,
    ) -> *mut c_void;
    fn WCDBRustStatementCreateTrigger_executeDelete(
        cpp_obj: *mut c_void,
        insert: *const c_void,
    ) -> *mut c_void;
    fn WCDBRustStatementCreateTrigger_executeSelect(
        cpp_obj: *mut c_void,
        insert: *const c_void,
    ) -> *mut c_void;
}

#[derive(Debug)]
pub struct StatementCreateTrigger {
    statement: Statement,
}

impl CppObjectTrait for StatementCreateTrigger {
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

impl CppObjectConvertibleTrait for StatementCreateTrigger {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementCreateTrigger {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementCreateTrigger {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementCreateTrigger {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementCreateTrigger {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementCreateTrigger_createCppObj() };
        StatementCreateTrigger {
            statement: Statement::new(CPPType::CreateTriggerSTMT, Some(cpp_obj)),
        }
    }

    pub fn create_trigger(&self, name: &str) -> &Self {
        let c_str = name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateTrigger_configTrigger(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn create_temp_trigger(&self, name: &str) -> &Self {
        let c_str = name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateTrigger_configTrigger(self.get_cpp_obj(), c_str.as_ptr());

            WCDBRustStatementCreateTrigger_configTemp(self.get_cpp_obj());
        }
        self
    }

    pub fn of_with_string(&self, schema: &str) -> &Self {
        let c_str = schema.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateTrigger_configSchema(
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
            WCDBRustStatementCreateTrigger_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as std::ffi::c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            );
        }
        self
    }

    pub fn if_not_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configIfNotExist(self.get_cpp_obj());
        }
        self
    }

    pub fn before(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configBefore(self.get_cpp_obj());
        }
        self
    }

    pub fn after(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configAfter(self.get_cpp_obj());
        }
        self
    }

    pub fn instead_of(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configInsteadOf(self.get_cpp_obj());
        }
        self
    }

    pub fn delete(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configDelete(self.get_cpp_obj());
        }
        self
    }

    pub fn insert(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configInsert(self.get_cpp_obj());
        }
        self
    }

    pub fn update(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configUpdate(self.get_cpp_obj());
        }
        self
    }

    pub fn of_columns<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumnParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StringColumnParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringColumnParam::Column(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustStatementCreateTrigger_configColumns(
                    self.get_cpp_obj(),
                    CPPType::String as std::ffi::c_int,
                    std::ptr::null_mut(),
                    0 as c_int,
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len() as std::ffi::c_int,
                );
            }
        } else {
            unsafe {
                WCDBRustStatementCreateTrigger_configColumns(
                    self.get_cpp_obj(),
                    cpp_type as c_int,
                    cpp_obj_vec.as_ptr(),
                    cpp_obj_vec.len() as c_int,
                    std::ptr::null_mut(),
                    0 as c_int,
                );
            }
        }
        self
    }

    pub fn on_table(&self, table_name: &str) -> &Self {
        let c_str = table_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateTrigger_configTable(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn for_each_row(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configForEachRow(self.get_cpp_obj());
        }
        self
    }

    pub fn when(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_configWhen(
                self.get_cpp_obj(),
                CppObject::get(condition),
            );
        }
        self
    }

    pub fn execute<T: StatementTrait>(&self, statement: &T) -> &Self {
        unsafe {
            WCDBRustStatementCreateTrigger_executeInsert(
                self.get_cpp_obj(),
                CppObject::get(statement),
            );
        }
        self
    }
}
