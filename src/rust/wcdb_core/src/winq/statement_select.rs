use crate::base::cpp_object::CppObjectTrait;
use crate::orm::field::Field;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use std::ffi::{c_char, c_double, c_int, c_long, c_void, CString};
use std::fmt::Debug;

extern "C" {
    pub fn WCDBRustStatementSelect_create() -> *mut c_void;
    pub fn WCDBRustStatementSelect_configResultColumns(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        void_vec: *const *mut c_void,
        double_vec: *const c_double,
        string_vec: *const *const c_char,
        vec_len: c_size_t,
    );
    pub fn WCDBRustStatementSelect_configTableOrSubqueries(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        long_vec: *const c_long,
        double_vec: *const c_double,
        string_vec: *const *const c_char,
        vec_len: c_size_t,
    );
    pub fn WCDBRustStatementSelect_configCondition(cpp_obj: *mut c_void, condition: *mut c_void);

    pub fn WCDBRustStatementSelect_configOrders(
        cpp_obj: *mut c_void,
        orders: *const c_long,
        orders_length: c_int,
    );

    pub fn WCDBRustStatementSelect_configLimitCount(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        count: c_long,
    );

    pub fn WCDBRustStatementSelect_configOffset(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        count: c_long,
    );
}

#[derive(Debug)]
pub struct StatementSelect {
    statement: Statement,
}

impl CppObjectTrait for StatementSelect {
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

impl IdentifierTrait for StatementSelect {
    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierStaticTrait for StatementSelect {
    fn get_type() -> i32 {
        CPPType::SelectSTMT as i32
    }
}

impl StatementTrait for StatementSelect {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementSelect {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementSelect_create() };
        StatementSelect {
            statement: Statement::new_with_obj(cpp_obj),
        }
    }

    pub fn select<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }

        let mut types_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for field in fields {
            types_vec.push(Identifier::get_cpp_type((*field).get_column()));
            cpp_obj_vec.push((*field).get_cpp_obj());
        }

        unsafe {
            WCDBRustStatementSelect_configResultColumns(
                self.get_cpp_obj(),
                types_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                std::ptr::null(),
                types_vec.len(),
            );
        }
        self
    }

    pub fn from(&self, table_name: &str) -> &Self {
        let types_vec = vec![CPPType::String as i32];

        let c_table_name = CString::new(table_name).unwrap_or_default();
        let str_vec = vec![c_table_name.as_ptr()];
        unsafe {
            WCDBRustStatementSelect_configTableOrSubqueries(
                self.get_cpp_obj(),
                types_vec.as_ptr(),
                std::ptr::null(),
                std::ptr::null(),
                str_vec.as_ptr(),
                types_vec.len(),
            );
        }
        self
    }

    pub fn where_expression(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configCondition(self.get_cpp_obj(), condition.get_cpp_obj());
        }
        self
    }

    pub fn order_by(&self, orders: Vec<OrderingTerm>) -> &Self {
        if orders.is_empty() {
            self;
        }
        let mut cpp_orders: Vec<*mut c_void> = Vec::new();
        for x in orders {
            cpp_orders.push(x.get_cpp_obj());
        }
        let orders_length = cpp_orders.len() as c_int;
        unsafe {
            WCDBRustStatementSelect_configOrders(
                self.get_cpp_obj(),
                cpp_orders.as_ptr() as *const c_long,
                orders_length,
            )
        }
        self
    }

    pub fn limit(&self, count: i64) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configLimitCount(
                self.get_cpp_obj(),
                CPPType::Int as c_int,
                count as c_long,
            )
        }
        self
    }

    pub fn offset(&self, offset: i64) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configOffset(
                self.get_cpp_obj(),
                CPPType::Int as c_int,
                offset as c_long,
            )
        }
        self
    }
}
