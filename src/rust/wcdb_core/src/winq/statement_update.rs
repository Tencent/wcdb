use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::orm::field::Field;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_void, CString};
use std::fmt::Debug;
use std::os::raw::c_long;
use std::ptr::null_mut;

extern "C" {
    pub fn WCDBRustStatementUpdate_create() -> *mut c_void;
    pub fn WCDBRustStatementUpdate_configTable(
        cpp_obj: *mut c_void,
        type_i: c_int,
        table: *mut c_void,
        table_name: *const c_char,
    );
    pub fn WCDBRustStatementUpdate_configColumnsToBindParameters(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        columns_vec_len: c_int,
    );

    pub fn WCDBRustStatementUpdate_configCondition(cpp_obj: *mut c_void, condition: *mut c_void);

    pub fn WCDBRustStatementUpdate_configOrders(
        cpp_obj: *mut c_void,
        orders: *const *mut c_void,
        vec_len: c_size_t,
    );
    pub fn WCDBRustStatementUpdate_configLimitCount(
        cpp_obj: *mut c_void,
        config_type: c_int,
        limit: c_long,
    );
    pub fn WCDBRustStatementUpdate_configOffset(
        cpp_obj: *mut c_void,
        config_type: c_int,
        offset: c_long,
    );
}

#[derive(Debug)]
pub struct StatementUpdate {
    statement: Statement,
}

impl CppObjectTrait for StatementUpdate {
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

impl IdentifierTrait for StatementUpdate {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementUpdate {
    fn get_type() -> i32 {
        CPPType::UpdateSTMT as i32
    }
}

impl StatementTrait for StatementUpdate {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementUpdate {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementUpdate_create() };
        StatementUpdate {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn update(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementUpdate_configTable(
                self.get_cpp_obj(),
                CPPType::String as i32,
                null_mut(),
                c_table_name.as_ptr(),
            );
        }
        self
    }

    pub fn set_columns_to_bind_parameters<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }
        let columns_void_vec_len = fields.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(fields.len());
        for field in fields {
            c_void_vec.push(field.get_cpp_obj());
        }

        unsafe {
            WCDBRustStatementUpdate_configColumnsToBindParameters(
                self.get_cpp_obj(),
                CPPType::Column as i32,
                c_void_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }

    pub fn where_expression(&self, condition: Expression) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configCondition(
                self.get_cpp_obj(),
                CppObject::get(condition.get_expression_operable()),
            );
        }
        self
    }

    pub fn order_by(&self, orders: &Vec<OrderingTerm>) -> &Self {
        if orders.is_empty() {
            return self;
        }
        let mut order_raw_vec = Vec::with_capacity(orders.len());
        for order in orders {
            order_raw_vec.push(order.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementUpdate_configOrders(
                self.get_cpp_obj(),
                order_raw_vec.as_ptr(),
                order_raw_vec.len(),
            );
        }
        self
    }

    pub fn limit(&self, count: i64) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configLimitCount(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                count,
            );
        }
        self
    }

    pub fn offset(&self, offset: i64) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configOffset(self.get_cpp_obj(), CPPType::Int as i32, offset);
        }
        self
    }
}
