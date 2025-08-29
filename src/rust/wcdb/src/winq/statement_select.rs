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

impl StatementTrait for StatementSelect {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl TableOrSubqueryConvertibleTrait for StatementSelect {}

pub trait StatementSelectSelectParam {
    fn get_params(self) -> (CPPType, *mut c_void);
}

impl<T: ResultColumnConvertibleTrait + IdentifierTrait> StatementSelectSelectParam for &T {
    fn get_params(self) -> (CPPType, *mut c_void) {
        (
            Identifier::get_type(self.as_identifier()),
            CppObject::get(self),
        )
    }
}

impl StatementSelectSelectParam for &str {
    fn get_params(self) -> (CPPType, *mut c_void) {
        (CPPType::String, self.to_cstring().as_ptr() as *mut c_void)
    }
}

pub trait StatementSelectFromParam {
    fn get_params(self) -> (CPPType, *mut c_void);
}

impl<T: TableOrSubqueryConvertibleTrait> StatementSelectFromParam for &T {
    fn get_params(self) -> (CPPType, *mut c_void) {
        (
            Identifier::get_type(self.as_identifier()),
            CppObject::get(self),
        )
    }
}

impl StatementSelectFromParam for String {
    fn get_params(self) -> (CPPType, *mut c_void) {
        (CPPType::String, self.to_cstring().as_ptr() as *mut c_void)
    }
}

pub trait StatementSelectGroupByParam {
    fn get_params(self) -> (CPPType, *mut c_void);
}

impl<T: ExpressionConvertibleTrait> StatementSelectGroupByParam for &T {
    fn get_params(self) -> (CPPType, *mut c_void) {
        (
            Identifier::get_type(self.as_identifier()),
            CppObject::get(self),
        )
    }
}

impl StatementSelectGroupByParam for String {
    fn get_params(self) -> (CPPType, *mut c_void) {
        (CPPType::String, self.to_cstring().as_ptr() as *mut c_void)
    }
}

impl StatementSelect {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementSelect_create() };
        StatementSelect {
            statement: Statement::new(CPPType::SelectSTMT, Some(cpp_obj)),
        }
    }

    pub fn select<T: ResultColumnConvertibleTrait>(&self, fields: &Vec<&T>) -> &Self {
        if fields.is_empty() {
            return self;
        }

        let mut types_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for field in fields {
            types_vec.push(Identifier::get_cpp_type(field.as_identifier()) as c_int);
            cpp_obj_vec.push(CppObject::get(field.as_cpp_object()) as c_longlong);
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

    // pub fn select<'a, T, R>(&self, param_vec: &'a T) -> &Self
    // where
    //     for<'b> &'b T: IntoIterator<Item = R>,
    //     R: StatementSelectSelectParam,
    // {
    //     let mut types = vec![];
    //     let mut cpp_obj_vec = vec![];
    //     let mut column_name_vec = vec![];
    //     for param in param_vec {
    //         let params = param.get_params();
    //         match params.0 {
    //             CPPType::String => column_name_vec.push(params.1 as *const c_char),
    //             _ => cpp_obj_vec.push(params.1 as c_longlong),
    //         }
    //         types.push(params.0 as c_int);
    //     }
    //     unsafe {
    //         WCDBRustStatementSelect_configResultColumns(
    //             self.get_cpp_obj(),
    //             types.as_ptr(),
    //             cpp_obj_vec.as_ptr(),
    //             std::ptr::null(),
    //             column_name_vec.as_ptr(),
    //             types.len(),
    //         );
    //     }
    //     self
    // }

    pub fn from<'a, T, R>(&self, param_vec: &T) -> &Self
    where
        T: IntoIterator<Item = R>,
        R: StatementSelectFromParam,
    {
        self
    }

    // pub fn from<'a, T, R>(&self, param_vec: &T) -> &Self
    // where
    //     T: IntoIterator<Item = R>,
    //     R: StatementSelectFromParam,
    // {
    //     let mut types = vec![];
    //     let mut cpp_obj_vec = vec![];
    //     let mut cstr_vec = vec![];
    //     for param in param_vec {
    //         let params = param.get_params();
    //         match params.0 {
    //             CPPType::String => cstr_vec.push(params.1 as *const c_char),
    //             _ => cpp_obj_vec.push(params.1 as c_longlong),
    //         }
    //         types.push(params.0 as c_int);
    //     }
    //     unsafe {
    //         WCDBRustStatementSelect_configTableOrSubqueries(
    //             self.get_cpp_obj(),
    //             types.as_ptr(),
    //             cpp_obj_vec.as_ptr(),
    //             std::ptr::null(),
    //             cstr_vec.as_ptr(),
    //             types.len(),
    //         );
    //     }
    //     self
    // }

    pub fn r#where(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configCondition(self.get_cpp_obj(), CppObject::get(condition));
        }
        self
    }

    pub fn group_by<'a, T, R>(&self, param_vec: T)
    where
        T: IntoIterator<Item = R>,
        R: StatementSelectGroupByParam,
    {
        let mut type_vec = vec![];
        let mut cpp_obj_vec = vec![];
        let mut colum_name_vec = vec![];
        for param in param_vec {
            let params = param.get_params();
            match params.0 {
                CPPType::String => colum_name_vec.push(params.1 as *const c_char),
                _ => cpp_obj_vec.push(params.1 as c_longlong),
            }
            type_vec.push(params.0 as c_int);
        }
        unsafe {
            WCDBRustStatementSelect_configGroups(
                self.get_cpp_obj(),
                type_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                colum_name_vec.as_ptr(),
                type_vec.len(),
            );
        }
    }

    pub fn order_by(&self, orders: &Vec<OrderingTerm>) -> &Self {
        if orders.is_empty() {
            return self;
        }
        let mut cpp_orders = vec![];
        for x in orders {
            cpp_orders.push(x.get_cpp_obj() as c_longlong);
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
