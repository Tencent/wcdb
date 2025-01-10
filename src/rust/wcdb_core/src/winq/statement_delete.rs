use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_void, CString};
use std::fmt::Debug;
use std::os::raw::c_long;

extern "C" {
    pub fn WCDBRustStatementDelete_create() -> *mut c_void;
    pub fn WCDBRustStatementDelete_configTable(
        cpp_obj: *mut c_void,
        table_type: c_int,
        table_long: c_long,
        table_string: *const c_char,
    );
    pub fn WCDBRustStatementDelete_configCondition(cpp_obj: *mut c_void, condition: *mut c_void);
    pub fn WCDBRustStatementDelete_configOrders(
        cpp_obj: *mut c_void,
        orders: *const *mut c_void,
        vec_len: c_size_t,
    );
    pub fn WCDBRustStatementDelete_configLimitCount(
        cpp_obj: *mut c_void,
        config_type: c_int,
        limit: c_long,
    );
}

#[derive(Debug)]
pub struct StatementDelete {
    statement: Statement,
}

impl CppObjectTrait for StatementDelete {
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

impl IdentifierTrait for StatementDelete {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementDelete {
    fn get_type() -> i32 {
        CPPType::DeleteSTMT as i32
    }
}

impl StatementTrait for StatementDelete {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementDelete {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementDelete_create() };
        StatementDelete {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn delete_from(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementDelete_configTable(
                self.get_cpp_obj(),
                CPPType::String as i32,
                0,
                c_table_name.as_ptr(),
            );
        }
        self
    }

    pub fn where_expression(&self, condition: Expression) -> &Self {
        unsafe {
            WCDBRustStatementDelete_configCondition(
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
            WCDBRustStatementDelete_configOrders(
                self.get_cpp_obj(),
                order_raw_vec.as_ptr(),
                order_raw_vec.len(),
            );
        }
        self
    }

    pub fn limit(&self, count: i64) -> &Self {
        unsafe {
            WCDBRustStatementDelete_configLimitCount(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                count,
            );
        }
        self
    }

    pub fn offset(&self, offset: i64) -> &Self {
        unsafe {
            WCDBRustStatementDelete_configLimitCount(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                offset,
            );
        }
        self
    }
}
