use crate::base::cpp_object::CppObjectTrait;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    pub fn WCDBRustStatementCreateIndex_create() -> *mut c_void;
    pub fn WCDBRustStatementCreateIndex_configIfNotExist(cpp_obj: *mut c_void);

    pub fn WCDBRustStatementCreateIndex_configIndexedColumns(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        columns_vec_len: c_int,
    );
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
        todo!("qixinbing")
    }

    pub fn unique(&self) -> &Self {
        todo!("qixinbing")
    }

    pub fn if_not_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateIndex_configIfNotExist(self.get_cpp_obj());
        }
        self
    }

    pub fn of(&self, schema_name: &str) -> &Self {
        todo!("qixinbing")
    }

    // pub fn of_schema(&self,schema: Schema)-> &Self {
    //     todo!("qixinbing")
    // }

    pub fn on(&self, table_name: &str) -> &Self {
        todo!("qixinbing")
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
        todo!("qixinbing")
    }

    pub fn where_expression(&self, condition: Expression) -> &Self {
        todo!("qixinbing")
    }
}
