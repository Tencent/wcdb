use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::orm::field::Field;
use crate::winq::column::Column;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::identifier::{CPPType, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::multi_type_array::MultiTypeArray;
use crate::winq::object::Object;
use crate::winq::statement::{Statement, StatementTrait};
use crate::winq::upsert::Upsert;
use std::ffi::{c_char, c_double, c_int, c_long, c_void, CString};
use std::fmt::Debug;

extern "C" {
    fn WCDBRustStatementInsert_create() -> *mut c_void;
    fn WCDBRustStatementInsert_configTableName(cpp_obj: *mut c_void, table_name: *const c_char);
    fn WCDBRustStatementInsert_configConflictAction(cpp_obj: *mut c_void, action: c_int);
    fn WCDBRustStatementInsert_configColumns(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        columns_vec_len: c_int,
    );
    fn WCDBRustStatementInsert_configValuesWithBindParameters(cpp_obj: *mut c_void, count: c_int);

    fn WCDBRustStatementInsert_configDefaultValues(cpp_obj: *mut c_void);

    fn WCDBRustStatementInsert_configValues(
        cpp_obj: *mut c_void,
        types: *const c_int,
        long_values: *const c_long,
        double_values: *const c_double,
        string_values: *const *const c_char,
        value_len: c_int,
    );
    fn WCDBRustStatementInsert_configUpsert(cpp_obj: *mut c_void, upsert: *mut c_void);
}

#[derive(Debug)]
pub struct StatementInsert {
    statement: Statement,
}

impl CppObjectTrait for StatementInsert {
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

impl IdentifierTrait for StatementInsert {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementInsert {
    fn get_type() -> i32 {
        CPPType::InsertSTMT as i32
    }
}

impl StatementTrait for StatementInsert {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementInsert {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementInsert_create() };
        StatementInsert {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn insert_into(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementInsert_configTableName(self.get_cpp_obj(), c_table_name.as_ptr());
        }
        self
    }

    pub fn or_replace(&self) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configConflictAction(
                self.get_cpp_obj(),
                ConflictAction::Replace as i32,
            );
        }
        self
    }

    pub fn or_rollback(&self) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configConflictAction(
                self.get_cpp_obj(),
                ConflictAction::Rollback as i32,
            );
        }
        self
    }

    pub fn or_abort(&self) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configConflictAction(
                self.get_cpp_obj(),
                ConflictAction::Abort as i32,
            );
        }
        self
    }

    pub fn or_fail(&self) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configConflictAction(
                self.get_cpp_obj(),
                ConflictAction::Fail as i32,
            );
        }
        self
    }

    pub fn or_ignore(&self) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configConflictAction(
                self.get_cpp_obj(),
                ConflictAction::Ignore as i32,
            );
        }
        self
    }

    pub fn columns<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }
        let columns_void_vec_len = fields.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(fields.len());
        for field in fields {
            c_void_vec.push(field.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementInsert_configColumns(
                self.get_cpp_obj(),
                CPPType::Column as i32,
                c_void_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }

    pub fn column_objs(&self, columns: &Vec<Column>) -> &Self {
        if columns.is_empty() {
            return self;
        }
        let column_len = columns.len();
        let mut c_vec: Vec<*mut c_void> = Vec::with_capacity(column_len);
        for column in columns {
            c_vec.push(column.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementInsert_configColumns(
                self.get_cpp_obj(),
                CPPType::Column as i32,
                c_vec.as_ptr(),
                std::ptr::null(),
                column_len as c_int,
            );
        }
        self
    }

    pub fn column_names(&self, names: Vec<String>) -> &Self {
        if names.is_empty() {
            return self;
        }
        let column_len = names.len();
        let mut c_vec: Vec<*const c_char> = Vec::with_capacity(column_len);
        for name in names {
            let c_name = CString::new(name).unwrap_or_default();
            c_vec.push(c_name.as_ptr());
        }
        unsafe {
            WCDBRustStatementInsert_configColumns(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null(),
                c_vec.as_ptr(),
                column_len as c_int,
            );
        }
        self
    }

    pub fn values_with_bind_parameters(&self, parameters_count: usize) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configValuesWithBindParameters(
                self.get_cpp_obj(),
                parameters_count as i32,
            )
        };
        self
    }

    pub fn default_values(&self) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configDefaultValues(self.get_cpp_obj());
        }
        self
    }

    pub fn values(&self, values: Option<Vec<Object>>) -> &Self {
        match values {
            None => return self,
            Some(v) if v.is_empty() => return self,
            Some(v) => {
                let array = MultiTypeArray::new_with_objects(&v);
                let mut c_vec: Vec<*const c_char> = Vec::new();
                let mut string_values_len: usize = 0;
                match array.string_values {
                    None => {}
                    Some(val) => {
                        string_values_len = val.len();
                        for x in val {
                            let c_name = CString::new(x).unwrap_or_default();
                            c_vec.push(c_name.as_ptr());
                        }
                    }
                }
                let value_len = array
                    .types
                    .len()
                    .max(array.long_values.len())
                    .max(array.double_values.len())
                    .max(string_values_len);
                unsafe {
                    WCDBRustStatementInsert_configValues(
                        self.get_cpp_obj(),
                        array.types.as_ptr(),
                        array.long_values.as_ptr(),
                        array.double_values.as_ptr(),
                        c_vec.as_ptr(),
                        value_len as c_int,
                    );
                }
            }
        }
        self
    }

    pub fn upsert(&self, upsert: &Upsert) -> &Self {
        unsafe {
            WCDBRustStatementInsert_configUpsert(self.get_cpp_obj(), CppObject::get(upsert));
        }
        self
    }
}
