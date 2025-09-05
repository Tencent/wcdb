use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::orm::field::Field;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::common_table_expression::CommonTableExpression;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::qualified_table::QualifiedTable;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_longlong, c_void, CString};
use std::fmt::Debug;
use std::os::raw::c_double;
use std::ptr::{null, null_mut};

extern "C" {
    fn WCDBRustStatementUpdate_create() -> *mut c_void;

    fn WCDBRustStatementUpdate_configWith(
        cpp_obj: *mut c_void,
        expressions: *const *mut c_void,
        expressions_length: c_int,
    );

    // fn WCDBRustStatementUpdate_configRecursive(cpp_obj: *mut c_void);

    fn WCDBRustStatementUpdate_configTable(
        cpp_obj: *mut c_void,
        type_i: c_int,
        table: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustStatementUpdate_configColumnsToBindParameters(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        columns_vec_len: c_size_t,
    );

    fn WCDBRustStatementUpdate_configCondition(cpp_obj: *mut c_void, condition: *mut c_void);

    fn WCDBRustStatementUpdate_configOrders(
        cpp_obj: *mut c_void,
        orders: *const *mut c_void,
        vec_len: c_size_t,
    );
    fn WCDBRustStatementUpdate_configLimitCount(
        cpp_obj: *mut c_void,
        config_type: c_int,
        limit: i64,
    );

    fn WCDBRustStatementUpdate_configOffset(cpp_obj: *mut c_void, config_type: c_int, offset: i64);

    fn WCDBRustStatementUpdate_configConfliction(cpp_obj: *mut c_void, action: c_int);

    fn WCDBRustStatementUpdate_configValue(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        // todo denxudong 补充 *mut c_void
        // arg_cpp_obj: *mut c_void,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustStatementUpdate_configColumns(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        columns: *const *mut c_void,
        column_names: *const *const c_char,
        vec_len: c_size_t,
    );

    fn WCDBRustStatementUpdate_configLimitRange(
        cpp_obj: *mut c_void,
        from_type: c_int,
        from: c_longlong,
        to_type: c_int,
        to: c_longlong,
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

impl CppObjectConvertibleTrait for StatementUpdate {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementUpdate {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementUpdate {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
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
            statement: Statement::new(CPPType::UpdateSTMT, Some(cpp_obj)),
        }
    }

    pub fn with(&self, expressions: &Vec<CommonTableExpression>) -> &Self {
        if expressions.is_empty() {
            return self;
        }
        let mut cpp_obj_vec: Vec<*mut c_void> = Vec::with_capacity(expressions.len());
        for x in expressions {
            cpp_obj_vec.push(CppObject::get(x));
        }
        unsafe {
            WCDBRustStatementUpdate_configWith(
                self.get_cpp_obj(),
                cpp_obj_vec.as_ptr(),
                cpp_obj_vec.len() as c_int,
            );
        }
        self
    }

    // pub fn with_recursive(&self, expressions: &Vec<CommonTableExpression>) -> &Self {
    //     if expressions.is_empty() {
    //         return self;
    //     }
    //     let mut cpp_obj_vec: Vec<*mut c_void> = Vec::with_capacity(expressions.len());
    //     for x in expressions {
    //         cpp_obj_vec.push(CppObject::get(x));
    //     }
    //     unsafe {
    //         WCDBRustStatementUpdate_configWith(
    //             self.get_cpp_obj(),
    //             cpp_obj_vec.as_ptr(),
    //             cpp_obj_vec.len() as c_int,
    //         );
    //     }
    //     unsafe { WCDBRustStatementUpdate_configRecursive(self.get_cpp_obj()) }
    //     self
    // }

    pub fn update(&self, table_name: &str) -> &Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe {
            WCDBRustStatementUpdate_configTable(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                null_mut(),
                c_table_name.as_ptr(),
            );
        }
        self
    }

    pub fn update_qualified_table(&self, table: QualifiedTable) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configTable(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&table) as c_int,
                CppObject::get(&table),
                null(),
            )
        }
        self
    }

    pub fn or_replace(&self) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configConfliction(
                self.get_cpp_obj(),
                ConflictAction::Replace as c_int,
            )
        }
        self
    }

    pub fn or_rollback(&self) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configConfliction(
                self.get_cpp_obj(),
                ConflictAction::Rollback as c_int,
            )
        }
        self
    }

    pub fn or_abort(&self) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configConfliction(
                self.get_cpp_obj(),
                ConflictAction::Abort as c_int,
            )
        }
        self
    }

    pub fn or_fail(&self) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configConfliction(
                self.get_cpp_obj(),
                ConflictAction::Fail as c_int,
            )
        }
        self
    }

    pub fn or_ignore(&self) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configConfliction(
                self.get_cpp_obj(),
                ConflictAction::Ignore as c_int,
            )
        }
        self
    }

    pub fn set_columns_to_bind_parameters<T>(&self, fields: &Vec<&Field<T>>) -> &Self {
        if fields.is_empty() {
            return self;
        }
        let columns_void_vec_len = fields.len();
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(fields.len());
        for field in fields {
            c_void_vec.push(field.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementUpdate_configColumnsToBindParameters(
                self.get_cpp_obj(),
                CPPType::Column as c_int,
                c_void_vec.as_ptr(),
                null(),
                columns_void_vec_len,
            );
        }
        self
    }

    // todo qixinbing 合并代码
    pub fn set_column_objs_to_bind_parameters(&self, columns: &Vec<Column>) -> &Self {
        if columns.is_empty() {
            return self;
        }
        let columns_vec_len = columns.len();
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(columns_vec_len);
        for column in columns {
            c_void_vec.push(column.get_cpp_obj());
        }
        unsafe {
            WCDBRustStatementUpdate_configColumnsToBindParameters(
                self.get_cpp_obj(),
                CPPType::Column as i32,
                c_void_vec.as_ptr(),
                null(),
                columns_vec_len,
            );
        }
        self
    }

    // todo dengxudong 重要不紧急
    // public StatementUpdate setColumnsToValues(@NotNull Column[] columns, @NotNull Object[] values)

    pub fn set_columns(&self, columns: &Vec<&Column>) -> &Self {
        if columns.is_empty() {
            return self;
        }
        let mut columns_void_vec: Vec<*mut c_void> = Vec::with_capacity(columns.len());
        for x in columns {
            columns_void_vec.push(CppObject::get(*x));
        }
        unsafe {
            WCDBRustStatementUpdate_configColumns(
                self.get_cpp_obj(),
                CPPType::Column as c_int,
                columns_void_vec.as_ptr(),
                null(),
                columns_void_vec.len(),
            )
        }
        self
    }

    pub fn set_column_names(&self, columns: &Vec<String>) -> &Self {
        if columns.is_empty() {
            return self;
        }

        let mut c_strings = Vec::new();
        let mut columns_void_vec: Vec<*const c_char> = Vec::with_capacity(columns.len());
        for x in columns {
            let c_string = x.to_cstring();
            columns_void_vec.push(c_string.as_ptr());
            c_strings.push(c_string);
        }

        unsafe {
            WCDBRustStatementUpdate_configColumns(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                null(),
                columns_void_vec.as_ptr(),
                columns_void_vec.len(),
            )
        }
        self
    }

    pub fn to_bool(&self, arg: bool) -> &Self {
        let ret = if arg { 1 } else { 0 } as *mut c_void;
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Bool as i32,
                ret,
                0 as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_u8(&self, arg: u8) -> &Self {
        let ret = arg as *mut c_void;
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                ret,
                0 as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_u16(&self, arg: u16) -> &Self {
        let ret = arg as *mut c_void;
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                ret,
                0 as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_i32(&self, arg: i32) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                arg as *mut c_void,
                0 as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_i64(&self, arg: i64) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                arg as *mut c_void,
                0 as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_f32(&self, arg: f32) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Double as i32,
                0 as *mut c_void,
                arg as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_f64(&self, arg: f64) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                CPPType::Double as i32,
                0 as *mut c_void,
                arg as c_double,
                null(),
            )
        }
        self
    }

    pub fn to_string(&self, arg: Option<String>) -> &Self {
        match arg {
            None => unsafe {
                WCDBRustStatementUpdate_configValue(
                    self.get_cpp_obj(),
                    CPPType::Null as i32,
                    0 as *mut c_void,
                    0 as c_double,
                    null(),
                )
            },
            Some(str) => {
                let c_str = str.to_cstring();
                unsafe {
                    WCDBRustStatementUpdate_configValue(
                        self.get_cpp_obj(),
                        CPPType::String as i32,
                        0 as *mut c_void,
                        0 as c_double,
                        c_str.as_ptr(),
                    )
                }
            }
        }
        self
    }

    // pub fn to_expression_convertible<T>(&self, arg: &T) -> &Self
    // where
    //     T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustStatementUpdate_configValue(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(arg) as c_int,
    //             CppObject::get(arg),
    //             0 as c_double,
    //             null(),
    //         )
    //     }
    //     self
    // }

    pub fn r#where(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configCondition(self.get_cpp_obj(), CppObject::get(condition));
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

    pub fn limit_i64_i64(&self, from: i64, to: i64) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configLimitRange(
                self.get_cpp_obj(),
                CPPType::Int as i32,
                from as i64,
                CPPType::Int as i32,
                to as i64,
            )
        }
        self
    }

    // pub fn limit_i64_expression_convertible<T>(&self, from: i64, to: &T) -> &Self
    // where
    //     T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustStatementUpdate_configLimitRange(
    //             self.get_cpp_obj(),
    //             CPPType::Int as c_int,
    //             from,
    //             Identifier::get_cpp_type(to) as c_int,
    //             CppObject::get(to) as c_longlong,
    //         )
    //     }
    //     self
    // }
    //
    // pub fn limit_expression_convertible<T>(&self, from: &T, to: &T) -> &Self
    // where
    //     T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustStatementUpdate_configLimitRange(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(from),
    //             CppObject::get(from) as i64,
    //             Identifier::get_cpp_type(to),
    //             CppObject::get(to) as i64,
    //         )
    //     }
    //     self
    // }
    //
    // pub fn limit_expression_convertible_i64<T>(&self, from: &T, to: i64) -> &Self
    // where
    //     T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustStatementUpdate_configLimitRange(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(from) as c_int,
    //             CppObject::get(from) as i64,
    //             CPPType::Int as i32,
    //             to as i64,
    //         )
    //     }
    //     self
    // }

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

    // pub fn limit_count_expression_convertible<T>(&self, count: &T) -> &Self
    // where
    //     T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustStatementUpdate_configLimitCount(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(count) as c_int,
    //             CppObject::get(count) as i64,
    //         );
    //     }
    //     self
    // }

    pub fn offset(&self, offset: i64) -> &Self {
        unsafe {
            WCDBRustStatementUpdate_configOffset(self.get_cpp_obj(), CPPType::Int as c_int, offset);
        }
        self
    }

    // pub fn offset_expression_convertible<T>(&self, offset: &T) -> &Self
    // where
    //     T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustStatementUpdate_configOffset(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(offset) as c_int,
    //             CppObject::get(offset) as i64,
    //         );
    //     }
    //     self
    // }
}
