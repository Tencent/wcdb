use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_basic_expression::BasicExpression;
use crate::base::param::enum_string_expression::StringExpression;
use crate::base::param::enum_string_schema::StringSchema;
use crate::utils::ToCString;
use crate::winq::bind_parameter::BindParameter;
use crate::winq::column::Column;
use crate::winq::column_type::ColumnType;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::{ExpressionOperable, ExpressionOperableTrait};
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::literal_value::LiteralValue;
use crate::winq::result_column::ResultColumn;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use crate::winq::statement_select::StatementSelect;
use std::ffi::{c_char, c_double, c_int, c_longlong, c_void};

extern "C" {
    fn WCDBRustExpression_create(value_type: c_int, cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustExpression_createWithFunction(func: *const c_char) -> *mut c_void;

    fn WCDBRustExpression_createWithExistStatement(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustExpression_createWithNotExistStatement(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustExpression_setWithSchema(
        expression: *mut c_void,
        schema_type: c_int,
        schema_object: *mut c_void,
        schema_string: *const c_char,
    );

    fn WCDBRustExpression_argument(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: c_longlong,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_invoke(cpp_obj: *mut c_void);

    fn WCDBRustExpression_invokeAll(cpp_obj: *mut c_void);

    fn WCDBRustExpression_distinct(cpp_obj: *mut c_void);

    fn WCDBRustExpression_cast(
        cpp_type: c_int,
        object: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustExpression_as(cpp_obj: *mut c_void, column_type: c_int);

    fn WCDBRustExpression_configAlias(cpp_obj: *mut c_void, alias: *const c_char) -> *mut c_void;

    fn WCDBRustExpression_caseWithExp(
        cpp_type: c_int,
        object: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustExpression_setWithWhenExp(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: c_longlong,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_setWithThenExp(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: c_longlong,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_setWithElseExp(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: c_longlong,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_escapeWith(cpp_obj: *mut c_void, string_value: *const c_char);

    fn WCDBRustExpression_createWithWindowFunction(func_name: *const c_char) -> *mut c_void;

    fn WCDBRustExpression_filter(cpp_obj: *mut c_void, condition: *mut c_void);

    fn WCDBRustExpression_overWindowDef(cpp_obj: *mut c_void, window_def: *mut c_void);

    fn WCDBRustExpression_overWindow(cpp_obj: *mut c_void, window: *const c_char);
}

#[derive(Debug)]
pub struct Expression {
    expression_operable: ExpressionOperable,
}

impl CppObjectTrait for Expression {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.expression_operable.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.expression_operable.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.expression_operable.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for Expression {
    fn as_cpp_object(&self) -> &CppObject {
        self.expression_operable.as_cpp_object()
    }
}

impl IdentifierTrait for Expression {
    fn get_type(&self) -> CPPType {
        self.expression_operable.get_type()
    }

    fn get_description(&self) -> String {
        self.expression_operable.get_description()
    }
}

impl IdentifierConvertibleTrait for Expression {
    fn as_identifier(&self) -> &Identifier {
        self.expression_operable.as_identifier()
    }
}

impl ExpressionConvertibleTrait for Expression {}

impl ExpressionOperableTrait for Expression {
    fn is_null(&self) -> Expression {
        self.expression_operable.is_null()
    }

    fn not_null(&self) -> Expression {
        self.expression_operable.not_null()
    }

    fn or<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression {
        self.expression_operable.or(operand)
    }

    fn and<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression {
        self.expression_operable.and(operand)
    }

    fn multiply<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.multiply(operand)
    }

    fn divide<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.divide(operand)
    }

    fn mod_<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.mod_(operand)
    }

    fn add<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.add(operand)
    }

    fn minus<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.minus(operand)
    }

    fn left_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.left_shift(operand)
    }

    fn right_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.right_shift(operand)
    }

    fn bit_and<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.bit_and(operand)
    }

    fn bit_or<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.bit_or(operand)
    }

    fn lt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.lt(operand)
    }

    fn le<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.le(operand)
    }

    fn gt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.gt(operand)
    }

    fn ge<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.ge(operand)
    }

    fn eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.eq(operand)
    }

    fn not_eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.not_eq(operand)
    }

    fn concat<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.concat(operand)
    }

    fn between<'a, T, V>(&self, begin: T, end: V) -> Expression
    where
        T: Into<BasicExpression<'a>>,
        V: Into<BasicExpression<'a>>,
    {
        self.expression_operable.between(begin, end)
    }

    fn not_between<'a, T, V>(&self, begin: T, end: V) -> Expression
    where
        T: Into<BasicExpression<'a>>,
        V: Into<BasicExpression<'a>>,
    {
        self.expression_operable.not_between(begin, end)
    }

    fn in_<'a, S>(&self, operands: Vec<S>) -> Expression
    where
        S: Into<BasicExpression<'a>>,
    {
        self.expression_operable
            .in_(Identifier::get_cpp_type(self), operands, false)
    }

    fn not_in<'a, S>(&self, operands: Vec<S>) -> Expression
    where
        S: Into<BasicExpression<'a>>,
    {
        self.expression_operable
            .not_in(Identifier::get_cpp_type(self), operands, true)
    }

    fn in_table(&self, table: &str) -> Expression {
        self.expression_operable.in_table(table)
    }

    fn not_in_table(&self, table: &str) -> Expression {
        self.expression_operable.not_in_table(table)
    }

    fn collate(&self, collation: &str) -> Expression {
        self.expression_operable.collate(collation)
    }

    fn like(&self, content: &str) -> Expression {
        self.expression_operable.like(content)
    }

    fn not_like(&self, content: &str) -> Expression {
        self.expression_operable.not_like(content)
    }

    fn glob(&self, content: &str) -> Expression {
        self.expression_operable.glob(content)
    }

    fn not_glob(&self, content: &str) -> Expression {
        self.expression_operable.not_glob(content)
    }

    fn match_(&self, content: &str) -> Expression {
        self.expression_operable.match_(content)
    }

    fn not_match(&self, content: &str) -> Expression {
        self.expression_operable.not_match(content)
    }

    fn regexp(&self, content: &str) -> Expression {
        self.expression_operable.regexp(content)
    }

    fn not_regexp(&self, content: &str) -> Expression {
        self.expression_operable.not_regexp(content)
    }

    fn is<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.is(operand)
    }

    fn is_not<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<BasicExpression<'a>>,
    {
        self.expression_operable.is_not(operand)
    }

    fn avg(&self) -> Expression {
        self.expression_operable.avg()
    }

    fn count(&self) -> Expression {
        self.expression_operable.count()
    }

    fn group_concat(&self) -> Expression {
        self.expression_operable.group_concat()
    }

    fn group_concat_string(&self, separator: &str) -> Expression {
        self.expression_operable.group_concat_string(separator)
    }

    fn max(&self) -> Expression {
        self.expression_operable.max()
    }

    fn min(&self) -> Expression {
        self.expression_operable.min()
    }

    fn sum(&self) -> Expression {
        self.expression_operable.sum()
    }

    fn total(&self) -> Expression {
        self.expression_operable.total()
    }

    fn abs(&self) -> Expression {
        self.expression_operable.abs()
    }

    fn hex(&self) -> Expression {
        self.expression_operable.hex()
    }

    fn length(&self) -> Expression {
        self.expression_operable.length()
    }

    fn lower(&self) -> Expression {
        self.expression_operable.lower()
    }

    fn upper(&self) -> Expression {
        self.expression_operable.upper()
    }

    fn round(&self) -> Expression {
        self.expression_operable.round()
    }

    fn match_info(&self) -> Expression {
        self.expression_operable.match_info()
    }

    fn offsets(&self) -> Expression {
        self.expression_operable.offsets()
    }

    fn snippet(&self) -> Expression {
        self.expression_operable.snippet()
    }

    fn bm25(&self) -> Expression {
        self.expression_operable.bm25()
    }

    fn highlight(&self) -> Expression {
        self.expression_operable.highlight()
    }

    fn substring_match_info(&self) -> Expression {
        self.expression_operable.substring_match_info()
    }
}

impl IndexedColumnConvertibleTrait for Expression {}

impl ResultColumnConvertibleTrait for Expression {}

impl From<&LiteralValue> for Expression {
    fn from(value: &LiteralValue) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(
                Identifier::get_cpp_type(value) as c_int,
                CppObject::get(value),
            )
        };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }
}

impl From<&BindParameter> for Expression {
    fn from(value: &BindParameter) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(
                Identifier::get_cpp_type(value) as c_int,
                CppObject::get(value),
            )
        };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }
}

impl From<&Column> for Expression {
    fn from(value: &Column) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(
                Identifier::get_cpp_type(value) as c_int,
                CppObject::get(value),
            )
        };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }
}

impl From<&StatementSelect> for Expression {
    fn from(value: &StatementSelect) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(
                Identifier::get_cpp_type(value) as c_int,
                CppObject::get(value),
            )
        };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }
}

impl Expression {
    pub(crate) fn new_empty() -> Self {
        Expression {
            expression_operable: ExpressionOperable::new(CPPType::Expression, None),
        }
    }

    pub fn new<'a, T>(value: T) -> Self
    where
        T: Into<ExpressionNewParam<'a>>,
    {
        let (value_cpp_type, value_cpp_obj) = match value.into() {
            ExpressionNewParam::BindParameter(value) => {
                (Identifier::get_cpp_type(value), CppObject::get(value))
            }
            ExpressionNewParam::LiteralValue(value) => {
                (Identifier::get_cpp_type(value), CppObject::get(value))
            }
            ExpressionNewParam::Column(value) => {
                (Identifier::get_cpp_type(value), CppObject::get(value))
            }
            ExpressionNewParam::StatementSelect(value) => {
                (Identifier::get_cpp_type(value), CppObject::get(value))
            }
        };
        let cpp_obj = unsafe { WCDBRustExpression_create(value_cpp_type as c_int, value_cpp_obj) };
        Expression {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn function(func_name: &str) -> Self {
        let cpp_obj =
            unsafe { WCDBRustExpression_createWithFunction(func_name.to_cstring().as_ptr()) };
        ExpressionOperable::create_expression(cpp_obj)
    }

    pub fn schema<'a, T>(&self, param: T) -> &Self
    where
        T: Into<StringSchema<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = param.into().get_params();
        let name_ptr = match name_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        unsafe {
            WCDBRustExpression_setWithSchema(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
                name_ptr,
            )
        }
        self
    }

    pub fn distinct(&self) -> &Self {
        unsafe {
            WCDBRustExpression_distinct(self.get_cpp_obj());
        }
        self
    }

    pub fn invoke(&self) -> &Self {
        unsafe {
            WCDBRustExpression_invoke(self.get_cpp_obj());
        }
        self
    }

    pub fn invoke_all(&self) -> &Self {
        unsafe {
            WCDBRustExpression_invokeAll(self.get_cpp_obj());
        }
        self
    }

    pub fn argument<'a, T>(&self, param: T) -> &Self
    where
        T: Into<BasicExpression<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_cstr_opt) = param.into().get_params();
        let arg_string_ptr = match arg_cstr_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        unsafe {
            WCDBRustExpression_argument(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long,
                arg_double,
                arg_string_ptr,
            );
        }
        self
    }

    pub fn escape(&self, content: &str) -> &Self {
        let cstr = content.to_cstring();
        unsafe {
            WCDBRustExpression_escapeWith(self.get_cpp_obj(), cstr.as_ptr());
        }
        self
    }

    pub fn exists(select: &StatementSelect) -> Self {
        let cpp_obj =
            unsafe { WCDBRustExpression_createWithExistStatement(CppObject::get(select)) };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn not_exists(select: &StatementSelect) -> Self {
        let cpp_obj =
            unsafe { WCDBRustExpression_createWithNotExistStatement(CppObject::get(select)) };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn cast<'a, T>(param: T) -> Self
    where
        T: Into<StringExpression<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = param.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        let cpp_obj = unsafe { WCDBRustExpression_cast(cpp_type as c_int, cpp_obj, name_ptr) };

        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    // todo qixinbing as 方法合并
    pub fn as_(&self, column_type: ColumnType) -> &Self {
        unsafe { WCDBRustExpression_as(self.get_cpp_obj(), column_type as c_int) };
        &self
    }

    pub fn as_result_column(&self, alias: &str) -> ResultColumn {
        let c_string = alias.to_cstring();
        let cpp_obj =
            unsafe { WCDBRustExpression_configAlias(self.get_cpp_obj(), c_string.as_ptr()) };
        ResultColumn::new_with_cpp_obj(cpp_obj)
    }

    pub fn case_() -> Self {
        let mut ret = Expression::new_empty();
        let cpp_obj =
            unsafe { WCDBRustExpression_caseWithExp(0, 0 as *mut c_void, std::ptr::null()) };
        ret.set_cpp_obj(cpp_obj);
        ret
    }

    // todo qixinbing 是否把 Option 放到 T 内部？
    pub fn case<'a, T>(param_opt: Option<T>) -> Self
    where
        T: Into<StringExpression<'a>>,
    {
        let param = match param_opt {
            None => {
                return Self::case_();
            }
            Some(val) => val,
        };
        let (cpp_type, cpp_obj, name_opt) = param.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        let cpp_obj =
            unsafe { WCDBRustExpression_caseWithExp(cpp_type as c_int, cpp_obj, name_ptr) };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn when<'a, T>(&self, param: T) -> &Self
    where
        T: Into<BasicExpression<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string_opt) = param.into().get_params();
        let arg_string_ptr = match arg_string_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        unsafe {
            WCDBRustExpression_setWithWhenExp(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long as c_longlong,
                arg_double,
                arg_string_ptr,
            );
        }
        self
    }

    pub fn then<'a, T>(&self, param: T) -> &Self
    where
        T: Into<BasicExpression<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string_opt) = param.into().get_params();
        let arg_string_ptr = match arg_string_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        unsafe {
            WCDBRustExpression_setWithThenExp(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long as c_longlong,
                arg_double,
                arg_string_ptr,
            );
        }
        self
    }

    pub fn else_<'a, T>(&self, param: T) -> &Self
    where
        T: Into<BasicExpression<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string_opt) = param.into().get_params();
        let arg_string_ptr = match arg_string_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        unsafe {
            WCDBRustExpression_setWithElseExp(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long as c_longlong,
                arg_double,
                arg_string_ptr,
            );
        }
        self
    }

    pub fn window_function(func_name: &str) -> Expression {
        let cpp_obj =
            unsafe { WCDBRustExpression_createWithWindowFunction(func_name.to_cstring().as_ptr()) };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn filter(&self, condition: &Expression) -> &Expression {
        unsafe { WCDBRustExpression_filter(self.get_cpp_obj(), condition.get_cpp_obj()) };
        self
    }

    // pub fn over<T: ExpressionOverParam>(&self, param: T) -> &Self {
    //     param.call_native(self.get_cpp_obj());
    //     self
    // }
}

pub enum ExpressionNewParam<'a> {
    BindParameter(&'a BindParameter),
    LiteralValue(&'a LiteralValue),
    Column(&'a Column),
    StatementSelect(&'a StatementSelect),
}

impl<'a> From<&'a BindParameter> for ExpressionNewParam<'a> {
    fn from(value: &'a BindParameter) -> Self {
        ExpressionNewParam::BindParameter(value)
    }
}

impl<'a> From<&'a LiteralValue> for ExpressionNewParam<'a> {
    fn from(value: &'a LiteralValue) -> Self {
        ExpressionNewParam::LiteralValue(value)
    }
}

impl<'a> From<&'a Column> for ExpressionNewParam<'a> {
    fn from(value: &'a Column) -> Self {
        ExpressionNewParam::Column(value)
    }
}

impl<'a> From<&'a StatementSelect> for ExpressionNewParam<'a> {
    fn from(value: &'a StatementSelect) -> Self {
        ExpressionNewParam::StatementSelect(value)
    }
}
