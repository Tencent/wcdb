use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;
use core::ffi::c_size_t;
use std::borrow::Cow;
use std::ffi::{c_char, c_double, c_int, c_longlong, c_void};
use std::fmt::Debug;

extern "C" {
    fn WCDBRustStatementSelect_create() -> *mut c_void;

    fn WCDBRustStatementSelect_configResultColumns(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        column_vec: *const c_longlong,
        unused_vec: *const c_double,
        column_name_vec: *const *const c_char,
        vec_len: c_size_t,
    );

    fn WCDBRustStatementSelect_configTableOrSubqueries(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        long_vec: *const c_longlong,
        unused_vec: *const c_double,
        table_name_vec: *const *const c_char,
        vec_len: c_size_t,
    );

    fn WCDBRustStatementSelect_configCondition(cpp_obj: *mut c_void, condition: *mut c_void);

    fn WCDBRustStatementSelect_configOrders(
        cpp_obj: *mut c_void,
        order_vec: *const c_longlong,
        orders_length: c_size_t,
    );

    fn WCDBRustStatementSelect_configGroups(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        exps_vec: *const c_longlong,
        unused_vec: *const c_double,
        colum_name_vec: *const *const c_char,
        length: c_size_t,
    );

    fn WCDBRustStatementSelect_configLimitCount(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        count: c_longlong,
    );

    fn WCDBRustStatementSelect_configOffset(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        count: c_longlong,
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

impl CppObjectConvertibleTrait for StatementSelect {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementSelect {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementSelect {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl TableOrSubqueryConvertibleTrait for StatementSelect {}

impl StatementTrait for StatementSelect {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementSelect {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementSelect_create() };
        StatementSelect {
            statement: Statement::new(CPPType::SelectSTMT, Some(cpp_obj)),
        }
    }

    pub fn select<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StatementSelectSelectParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type_vec = vec![];
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StatementSelectSelectParam::String(str) => {
                    cpp_type_vec.push(CPPType::String as c_int);
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StatementSelectSelectParam::ResultColumn(obj) => {
                    cpp_type_vec.push(Identifier::get_cpp_type(obj.as_identifier()) as c_int);
                    cpp_obj_vec.push(CppObject::get(obj) as c_longlong);
                }
            }
        }
        unsafe {
            WCDBRustStatementSelect_configTableOrSubqueries(
                self.get_cpp_obj(),
                cpp_type_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                cpp_str_vec.as_ptr(),
                cpp_type_vec.len(),
            );
        }
        self
    }

    pub fn from<'a, I, S>(&self, table_arg_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StatementSelectFromParam<'a>>,
    {
        let mut data_vec = table_arg_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type_vec = vec![];
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];

        for item in data_vec {
            match item {
                StatementSelectFromParam::String(str) => {
                    cpp_type_vec.push(CPPType::String as c_int);
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StatementSelectFromParam::TableOrSubquery(obj) => {
                    cpp_type_vec.push(Identifier::get_cpp_type(obj.as_identifier()) as c_int);
                    cpp_obj_vec.push(CppObject::get(obj) as c_longlong);
                }
            }
        }
        unsafe {
            WCDBRustStatementSelect_configTableOrSubqueries(
                self.get_cpp_obj(),
                cpp_type_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                cpp_str_vec.as_ptr(),
                cpp_type_vec.len(),
            );
        }
        self
    }

    pub fn r#where(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configCondition(self.get_cpp_obj(), CppObject::get(condition));
        }
        self
    }

    pub fn group_by<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StatementSelectGroupByParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type_vec = vec![];
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StatementSelectGroupByParam::String(str) => {
                    cpp_type_vec.push(CPPType::String as c_int);
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StatementSelectGroupByParam::ExpressionConvertible(obj) => {
                    cpp_type_vec.push(Identifier::get_cpp_type(obj.as_identifier()) as c_int);
                    cpp_obj_vec.push(CppObject::get(obj) as c_longlong);
                }
            }
        }
        unsafe {
            WCDBRustStatementSelect_configGroups(
                self.get_cpp_obj(),
                cpp_type_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                cpp_str_vec.as_ptr(),
                cpp_type_vec.len(),
            );
        }
        self
    }

    pub fn order_by<O, Oi>(&self, order_vec: O) -> &Self
    where
        O: IntoIterator<Item = Oi>,
        Oi: AsRef<OrderingTerm>,
    {
        let order_vec: Vec<Oi> = order_vec.into_iter().collect();
        if order_vec.is_empty() {
            return self;
        }
        let mut cpp_orders = vec![];
        for order in order_vec {
            cpp_orders.push(order.as_ref().get_cpp_obj() as c_longlong);
        }
        let orders_length = cpp_orders.len();
        unsafe {
            WCDBRustStatementSelect_configOrders(
                self.get_cpp_obj(),
                cpp_orders.as_ptr(),
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
                count,
            )
        }
        self
    }

    pub fn offset(&self, offset: i64) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configOffset(self.get_cpp_obj(), CPPType::Int as c_int, offset)
        }
        self
    }
}

pub enum StatementSelectSelectParam<'a> {
    String(String),
    ResultColumn(&'a dyn ResultColumnConvertibleTrait),
}

impl<'a> From<String> for StatementSelectSelectParam<'a> {
    fn from(value: String) -> Self {
        StatementSelectSelectParam::String(value)
    }
}

impl<'a> From<&str> for StatementSelectSelectParam<'a> {
    fn from(value: &str) -> Self {
        StatementSelectSelectParam::String(value.to_string())
    }
}

impl<'a, T: ResultColumnConvertibleTrait> From<&'a T> for StatementSelectSelectParam<'a> {
    fn from(value: &'a T) -> Self {
        StatementSelectSelectParam::ResultColumn(value)
    }
}

pub enum StatementSelectFromParam<'a> {
    String(String),
    TableOrSubquery(&'a dyn TableOrSubqueryConvertibleTrait),
}

impl<'a> From<String> for StatementSelectFromParam<'a> {
    fn from(value: String) -> Self {
        StatementSelectFromParam::String(value)
    }
}

impl<'a> From<&str> for StatementSelectFromParam<'a> {
    fn from(value: &str) -> Self {
        StatementSelectFromParam::String(value.to_string())
    }
}

impl<'a, T: TableOrSubqueryConvertibleTrait + 'a> From<&'a T> for StatementSelectFromParam<'a> {
    fn from(value: &'a T) -> Self {
        StatementSelectFromParam::TableOrSubquery(value)
    }
}

pub enum StatementSelectGroupByParam<'a> {
    String(String),
    ExpressionConvertible(&'a dyn ExpressionConvertibleTrait),
}

impl<'a> From<String> for StatementSelectGroupByParam<'a> {
    fn from(value: String) -> Self {
        StatementSelectGroupByParam::String(value)
    }
}

impl<'a> From<&str> for StatementSelectGroupByParam<'a> {
    fn from(value: &str) -> Self {
        StatementSelectGroupByParam::String(value.to_string())
    }
}

impl<'a, T: ExpressionConvertibleTrait> From<&'a T> for StatementSelectGroupByParam<'a> {
    fn from(value: &'a T) -> Self {
        StatementSelectGroupByParam::ExpressionConvertible(value)
    }
}
