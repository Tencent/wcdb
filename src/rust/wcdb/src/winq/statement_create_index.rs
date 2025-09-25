use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_indexed_column::StringIndexedColumn;
use crate::base::param::enum_string_schema::StringSchema;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustStatementCreateIndex_create() -> *mut c_void;

    fn WCDBRustStatementCreateIndex_configIndex(cpp_obj: *mut c_void, index_name: *const c_char);

    fn WCDBRustStatementCreateIndex_configUnique(cpp_obj: *mut c_void);

    fn WCDBRustStatementCreateIndex_configSchemaName(
        cpp_obj: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustStatementCreateIndex_configIfNotExist(cpp_obj: *mut c_void);

    fn WCDBRustStatementCreateIndex_configIndexedColumns(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        columns_vec_len: c_size_t,
    );

    fn WCDBRustStatementCreateIndex_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        schema: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustStatementCreateIndex_configTable(cpp_obj: *mut c_void, table_name: *const c_char);

    fn WCDBRustStatementCreateIndex_configWhere(cpp_obj: *mut c_void, condition: *mut c_void);
}

pub struct StatementCreateIndex {
    statement: Statement,
}

impl CppObjectTrait for StatementCreateIndex {
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

impl CppObjectConvertibleTrait for StatementCreateIndex {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementCreateIndex {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementCreateIndex {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementCreateIndex {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementCreateIndex {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementCreateIndex_create() };
        StatementCreateIndex {
            statement: Statement::new(CPPType::CreateIndexSTMT, Some(cpp_obj)),
        }
    }

    pub fn create_index(&self, index_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configIndex(
                self.get_cpp_obj(),
                index_name.to_cstring().as_ptr(),
            )
        }
        self
    }

    pub fn unique(&self) -> &Self {
        unsafe { WCDBRustStatementCreateIndex_configUnique(self.get_cpp_obj()) }
        self
    }

    pub fn if_not_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configIfNotExist(self.get_cpp_obj());
        }
        self
    }

    pub fn of<'a, T>(&self, schema: T) -> &Self
    where
        T: Into<StringSchema<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = schema.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        unsafe {
            WCDBRustStatementCreateIndex_configSchema(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            )
        }
        self
    }

    pub fn on(&self, table_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configTable(
                self.get_cpp_obj(),
                table_name.to_cstring().as_ptr(),
            )
        }
        self
    }

    pub fn indexed_by<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringIndexedColumn<'a>>,
    {
        let data_vec = column_vec.into_iter().map(Into::into).collect::<Vec<_>>();
        if data_vec.is_empty() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        let mut c_strings = Vec::new();
        for item in data_vec {
            match item {
                StringIndexedColumn::String(str) => {
                    let c_string = str.to_cstring();
                    cpp_str_vec.push(c_string.as_ptr());
                    c_strings.push(c_string);
                }
                StringIndexedColumn::IndexedColumnConvertible(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustStatementCreateIndex_configIndexedColumns(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len(),
                );
            }
        } else {
            unsafe {
                WCDBRustStatementCreateIndex_configIndexedColumns(
                    self.get_cpp_obj(),
                    cpp_type as c_int,
                    cpp_obj_vec.as_ptr(),
                    std::ptr::null(),
                    cpp_obj_vec.len(),
                );
            }
        }
        self
    }

    pub fn where_(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configWhere(self.get_cpp_obj(), CppObject::get(condition))
        }
        self
    }
}
