use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::expression_convertible_param::ExpressionConvertibleParam;
use crate::base::param::i64_expression_convertible_param::I64ExpressionConvertibleParam;
use crate::base::param::string_column_trait_param::StringColumnTraitParam;
use crate::base::param::string_qualified_table_param::StringQualifiedTableParam;
use crate::utils::ToCString;
use crate::winq::common_table_expression::CommonTableExpression;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::{Statement, StatementTrait};
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_longlong, c_void};
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

    fn WCDBRustStatementUpdate_configOffset(
        cpp_obj: *mut c_void,
        config_type: c_int,
        offset: *mut c_void,
    );

    fn WCDBRustStatementUpdate_configConfliction(cpp_obj: *mut c_void, action: c_int);

    fn WCDBRustStatementUpdate_configValue(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        // todo denxudong 补充 *mut c_void
        // arg_cpp_obj: *mut c_void,
        int_value: c_longlong,
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

    pub fn with_recursive(&self, expressions: &Vec<CommonTableExpression>) -> &Self {
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
        // todo qixinbing 待实现
        // unsafe { WCDBRustStatementUpdate_configRecursive(self.get_cpp_obj()) }
        self
    }

    pub fn update<'a, S>(&self, table_vec: S) -> &Self
    where
        S: Into<StringQualifiedTableParam<'a>>,
    {
        let value = table_vec.into();
        let (cpp_type, table, table_name) = match value {
            StringQualifiedTableParam::String(str) => {
                let table_name = str.as_str().to_cstring().as_ptr();
                (CPPType::String, null_mut(), table_name)
            }
            StringQualifiedTableParam::QualifiedTable(obj) => {
                let cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                (cpp_type, CppObject::get(obj), null())
            }
        };
        unsafe {
            WCDBRustStatementUpdate_configTable(
                self.get_cpp_obj(),
                cpp_type as c_int,
                table,
                table_name,
            );
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

    pub fn set_columns_to_bind_parameters<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumnTraitParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StringColumnTraitParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringColumnTraitParam::Column(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustStatementUpdate_configColumnsToBindParameters(
                    self.get_cpp_obj(),
                    CPPType::String as i32,
                    null(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len(),
                );
            }
        } else {
            unsafe {
                WCDBRustStatementUpdate_configColumnsToBindParameters(
                    self.get_cpp_obj(),
                    CPPType::Column as c_int,
                    cpp_obj_vec.as_ptr(),
                    null(),
                    cpp_obj_vec.len(),
                );
            }
        }
        self
    }

    // todo dengxudong 重要不紧急
    // public StatementUpdate setColumnsToValues(@NotNull Column[] columns, @NotNull Object[] values)

    pub fn set<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumnTraitParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StringColumnTraitParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringColumnTraitParam::Column(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustStatementUpdate_configColumns(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    null(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len(),
                )
            }
        } else {
            unsafe {
                WCDBRustStatementUpdate_configColumns(
                    self.get_cpp_obj(),
                    CPPType::Column as i32,
                    cpp_obj_vec.as_ptr(),
                    null(),
                    cpp_obj_vec.len(),
                );
            }
        }
        self
    }

    pub fn to<'a, V>(&self, value: V) -> &Self
    where
        V: Into<ExpressionConvertibleParam<'a>>,
    {
        let value = value.into();
        let (cpp_type, int_value, double_value, string_value) = value.get_params();
        unsafe {
            WCDBRustStatementUpdate_configValue(
                self.get_cpp_obj(),
                cpp_type as c_int,
                int_value,
                double_value,
                string_value,
            )
        }
        self
    }

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

    pub fn limit<'a, V, T>(&self, from: V, to: T) -> &Self
    where
        V: Into<I64ExpressionConvertibleParam<'a>>,
        T: Into<I64ExpressionConvertibleParam<'a>>,
    {
        let to = to.into();

        match to {
            I64ExpressionConvertibleParam::I64(to_value) => {
                self.config_limit_range_to_i64(from, to_value);
            }
            I64ExpressionConvertibleParam::ExpressionConvertible(to_value_obj) => {
                match to_value_obj {
                    None => {
                        self.config_limit_count(from);
                    }
                    Some(to_value) => {
                        self.config_limit_range(from, to_value);
                    }
                }
            }
        }
        self
    }

    fn config_limit_count<'a, V>(&self, from: V)
    where
        V: Into<I64ExpressionConvertibleParam<'a>>,
    {
        let from = from.into();
        match from {
            I64ExpressionConvertibleParam::I64(from_value) => unsafe {
                WCDBRustStatementUpdate_configLimitCount(
                    self.get_cpp_obj(),
                    CPPType::Int as i32,
                    from_value,
                );
            },
            I64ExpressionConvertibleParam::ExpressionConvertible(from_value_opt) => {
                match from_value_opt {
                    None => unsafe {
                        WCDBRustStatementUpdate_configLimitCount(
                            self.get_cpp_obj(),
                            CPPType::Null as c_int,
                            0,
                        );
                    },
                    Some(from_value) => unsafe {
                        WCDBRustStatementUpdate_configLimitCount(
                            self.get_cpp_obj(),
                            Identifier::get_cpp_type(from_value) as c_int,
                            CppObject::get(from_value) as c_longlong,
                        );
                    },
                }
            }
        }
    }

    fn config_limit_range<'a, V>(&self, from: V, to: &'a dyn ExpressionConvertibleTrait)
    where
        V: Into<I64ExpressionConvertibleParam<'a>>,
    {
        let from = from.into();
        match from {
            I64ExpressionConvertibleParam::I64(from_value) => unsafe {
                WCDBRustStatementUpdate_configLimitRange(
                    self.get_cpp_obj(),
                    CPPType::Int as c_int,
                    from_value as c_longlong,
                    Identifier::get_cpp_type(to) as c_int,
                    CppObject::get(to) as c_longlong,
                )
            },
            I64ExpressionConvertibleParam::ExpressionConvertible(from_value_opt) => {
                match from_value_opt {
                    None => unsafe {
                        WCDBRustStatementUpdate_configLimitRange(
                            self.get_cpp_obj(),
                            CPPType::Null as c_int,
                            0 as c_longlong,
                            Identifier::get_cpp_type(to) as c_int,
                            CppObject::get(to) as c_longlong,
                        )
                    },
                    Some(from_value) => unsafe {
                        WCDBRustStatementUpdate_configLimitRange(
                            self.get_cpp_obj(),
                            Identifier::get_cpp_type(from_value) as c_int,
                            CppObject::get(from_value) as c_longlong,
                            Identifier::get_cpp_type(to) as c_int,
                            CppObject::get(to) as c_longlong,
                        )
                    },
                }
            }
        }
    }

    fn config_limit_range_to_i64<'a, V>(&self, from: V, to: i64)
    where
        V: Into<I64ExpressionConvertibleParam<'a>>,
    {
        let from = from.into();
        match from {
            I64ExpressionConvertibleParam::I64(from_value) => unsafe {
                WCDBRustStatementUpdate_configLimitRange(
                    self.get_cpp_obj(),
                    CPPType::Int as c_int,
                    from_value as c_longlong,
                    CPPType::Int as c_int,
                    to as c_longlong,
                )
            },
            I64ExpressionConvertibleParam::ExpressionConvertible(from_value_opt) => {
                match from_value_opt {
                    None => unsafe {
                        WCDBRustStatementUpdate_configLimitRange(
                            self.get_cpp_obj(),
                            CPPType::Null as c_int,
                            0 as c_longlong,
                            CPPType::Int as c_int,
                            to as c_longlong,
                        )
                    },
                    Some(from_value) => unsafe {
                        WCDBRustStatementUpdate_configLimitRange(
                            self.get_cpp_obj(),
                            Identifier::get_cpp_type(from_value) as c_int,
                            CppObject::get(from_value) as c_longlong,
                            CPPType::Int as c_int,
                            to as c_longlong,
                        )
                    },
                }
            }
        }
    }

    pub fn offset<'a, V>(&self, offset: V) -> &Self
    where
        V: Into<I64ExpressionConvertibleParam<'a>>,
    {
        let offset = offset.into();
        let (config_type, offset) = match offset {
            I64ExpressionConvertibleParam::I64(value) => (CPPType::Int, value as *mut c_void),
            I64ExpressionConvertibleParam::ExpressionConvertible(value_opt) => match value_opt {
                None => (CPPType::Null, 0 as *mut c_void),
                Some(value) => unsafe { (Identifier::get_cpp_type(value), CppObject::get(value)) },
            },
        };
        unsafe {
            WCDBRustStatementUpdate_configOffset(self.get_cpp_obj(), config_type as c_int, offset);
        }
        self
    }
}
