use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::orm::field::Field;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;
use core::ffi::c_size_t;
use std::ffi::{c_char, c_double, c_int, c_long, c_void, CString};
use std::fmt::Debug;
use std::ptr::null;

extern "C" {
    fn WCDBRustStatementSelect_create() -> *mut c_void;
    fn WCDBRustStatementSelect_configResultColumns(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        void_vec: *const *mut c_void,
        double_vec: *const c_double,
        string_vec: *const *const c_char,
        vec_len: c_size_t,
    );
    fn WCDBRustStatementSelect_configTableOrSubqueries(
        cpp_obj: *mut c_void,
        type_vec: *const c_int,
        long_vec: *const c_long,
        double_vec: *const c_double,
        string_vec: *const *const c_char,
        vec_len: c_size_t,
    );
    fn WCDBRustStatementSelect_configCondition(cpp_obj: *mut c_void, condition: *mut c_void);

    fn WCDBRustStatementSelect_configOrders(
        cpp_obj: *mut c_void,
        orders: *const c_long,
        orders_length: c_int,
    );

    fn WCDBRustStatementSelect_configGroups(
        cpp_obj: *mut c_void,
        types: *const c_int,
        exps: *const *mut c_void,
        unused: *const c_double,
        colum_names: *const *const c_char,
        length: c_int,
    );

    fn WCDBRustStatementSelect_configLimitCount(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        count: c_long,
    );

    fn WCDBRustStatementSelect_configOffset(cpp_obj: *mut c_void, cpp_type: c_int, count: c_long);
}

#[derive(Debug)]
pub struct StatementSelect {
    statement: Statement,
}

impl IdentifierConvertibleTrait for StatementSelect {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl CppObjectConvertibleTrait for StatementSelect {
    fn as_cpp_object(&self) -> *mut c_void {
        self.statement.as_cpp_object()
    }
}

impl IndexedColumnConvertibleTrait for StatementSelect {}

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

    pub fn select_with_result_column_convertible_trait<T>(&self, result_columns: &Vec<T>) -> &Self
    where
        T: ResultColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        if result_columns.is_empty() {
            return self;
        }
        let size = result_columns.len();
        let mut types: Vec<i32> = Vec::with_capacity(size);
        let mut cpp_obj_vec: Vec<*mut c_void> = Vec::with_capacity(size);
        for x in result_columns {
            types.push(Identifier::get_cpp_type(x));
            cpp_obj_vec.push(CppObject::get(x));
        }
        unsafe {
            WCDBRustStatementSelect_configResultColumns(
                self.get_cpp_obj(),
                types.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                std::ptr::null(),
                types.len(),
            );
        }
        self
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

    pub fn select_columns(&self, columns: &Vec<&Column>) -> &Self {
        if columns.is_empty() {
            return self;
        }

        let mut types_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for column in columns {
            types_vec.push(CPPType::Column as i32);
            cpp_obj_vec.push(column.get_cpp_obj());
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

    pub fn from_with_table_or_subquery_convertible_trait<T>(
        &self,
        table_or_subqueries: &Vec<T>,
    ) -> &Self
    where
        T: TableOrSubqueryConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        if table_or_subqueries.is_empty() {
            return self;
        }
        let total_count = table_or_subqueries.len();
        let mut cpp_objs: Vec<*mut c_void> = Vec::with_capacity(total_count);
        let mut types = Vec::with_capacity(total_count);
        for x in table_or_subqueries {
            types.push(Identifier::get_cpp_type(x));
            cpp_objs.push(CppObject::get(x));
        }
        unsafe {
            WCDBRustStatementSelect_configTableOrSubqueries(
                self.get_cpp_obj(),
                types.as_ptr(),
                cpp_objs.as_ptr() as *const c_long,
                std::ptr::null(),
                std::ptr::null(),
                total_count,
            )
        }
        self
    }

    pub fn where_expression(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementSelect_configCondition(self.get_cpp_obj(), condition.get_cpp_obj());
        }
        self
    }

    pub fn group_by_with_expression_convertible_trait<T>(&self, column_names: &Vec<T>) -> &Self
    where
        T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        if column_names.is_empty() {
            return self;
        }
        let len = column_names.len();
        let mut cpp_objs: Vec<*mut c_void> = Vec::with_capacity(len);
        let mut types = Vec::with_capacity(len);
        for x in column_names {
            types.push(Identifier::get_cpp_type(x));
            cpp_objs.push(CppObject::get(x));
        }
        let length = len as c_int;
        unsafe {
            WCDBRustStatementSelect_configGroups(
                self.get_cpp_obj(),
                types.as_ptr(),
                cpp_objs.as_ptr(),
                null(),
                null(),
                length,
            );
        }
        self
    }

    pub fn group_by(&self, column_names: &Vec<String>) -> &Self {
        if column_names.is_empty() {
            return self;
        }
        let len = column_names.len();
        let mut cstr_vector: Vec<*const c_char> = Vec::with_capacity(len);
        let mut types = Vec::with_capacity(len);
        for x in column_names {
            cstr_vector.push(x.to_cstring().into_raw());
            types.push(CPPType::String as i32);
        }
        let length = len as c_int;
        unsafe {
            WCDBRustStatementSelect_configGroups(
                self.get_cpp_obj(),
                types.as_ptr(),
                null(),
                null(),
                cstr_vector.as_ptr(),
                length,
            );
        }
        self
    }

    // todo dengxudong 缺逻辑 重要不紧急
    // StatementSelect groupBy(@Nullable Object... expressions)

    pub fn order_by(&self, orders: &Vec<OrderingTerm>) -> &Self {
        if orders.is_empty() {
            return self;
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
