use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};
use std::ptr::null;

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
        columns_vec_len: c_int,
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

impl IdentifierTrait for StatementCreateIndex {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementCreateIndex {
    fn get_type() -> i32 {
        CPPType::CreateIndexSTMT as i32
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
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn create_index(&self, index_name: &str) -> &Self {
        let cstr = index_name.to_cstring();
        unsafe { WCDBRustStatementCreateIndex_configIndex(self.get_cpp_obj(), cstr.as_ptr()) }
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

    pub fn of(&self, schema_name: &str) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configSchema(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                0 as *mut c_void,
                schema_name.to_cstring().as_ptr(),
            )
        }
        self
    }

    // pub fn of_schema(&self,schema: Schema)-> &Self {
    //     unsafe {
    //         WCDBRustStatementCreateIndex_configSchema(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             schema_name.to_cstring().as_ptr(),
    //         )
    //     }
    //     self
    // }

    pub fn on(&self, table_name: &str) -> &Self {
        let cstr = table_name.to_cstring();
        unsafe { WCDBRustStatementCreateIndex_configTable(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }

    pub fn indexed_by<T>(&self, column_convertible_vec: Vec<&T>) -> &Self
    where
        T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        if column_convertible_vec.is_empty() {
            return self;
        }
        let columns_void_vec_len = column_convertible_vec.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(column_convertible_vec.len());
        let cpp_type = Identifier::get_cpp_type(column_convertible_vec[0]);
        for column_convertible in column_convertible_vec {
            c_void_vec.push(column_convertible.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementCreateIndex_configIndexedColumns(
                self.get_cpp_obj(),
                cpp_type,
                c_void_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }

    pub fn indexed_by_column_names(&self, column_names: &Vec<String>) -> &Self {
        let mut c_string_array: Vec<*const c_char> = Vec::new();
        for x in column_names {
            c_string_array.push(x.to_cstring().into_raw());
        }
        unsafe {
            WCDBRustStatementCreateIndex_configIndexedColumns(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                null(),
                c_string_array.as_ptr(),
                c_string_array.len() as c_int,
            );
        }
        self
    }

    pub fn where_expression(&self, condition: Expression) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configWhere(self.get_cpp_obj(), CppObject::get(&condition))
        }
        self
    }
}
