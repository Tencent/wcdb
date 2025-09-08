use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::expression_convertible_param::ExpressionConvertibleParam;
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
use crate::winq::schema::Schema;
use crate::winq::statement_select::StatementSelect;
use crate::winq::window_def::WindowDef;
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

    fn or<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression
    {
        self.expression_operable.or(operand)
    }

    fn and<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression
    {
        self.expression_operable.and(operand)
    }

    fn multiply<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.multiply(operand)
    }

    fn divide<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.divide(operand)
    }

    fn r#mod<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.r#mod(operand)
    }

    fn add<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.add(operand)
    }

    fn minus<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.minus(operand)
    }

    fn left_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.left_shift(operand)
    }

    fn right_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.right_shift(operand)
    }

    fn bit_and<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.bit_and(operand)
    }

    fn bit_or<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.bit_or(operand)
    }

    fn lt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.lt(operand)
    }

    fn le<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.le(operand)
    }

    fn gt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.gt(operand)
    }

    fn ge<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.ge(operand)
    }

    fn eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.eq(operand)
    }

    fn not_eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.not_eq(operand)
    }

    fn concat<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.concat(operand)
    }

    fn between<'a, T>(&self, begin: T, end: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.between(begin, end)
    }

    fn not_between<'a, T>(&self, begin: T, end: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.not_between(begin, end)
    }

    fn r#in<'a, I, S>(&self, operands: I) -> Expression
    where
        I: IntoIterator<Item = S>,
        S: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.r#in(operands)
    }

    fn not_in<'a, I, S>(&self, operands: I) -> Expression
    where
        I: IntoIterator<Item = S>,
        S: Into<ExpressionConvertibleParam<'a>>,
    {
        self.expression_operable.not_in(operands)
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

    fn r#match(&self, content: &str) -> Expression {
        self.expression_operable.r#match(content)
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

    fn is(&self, operand: bool) -> Expression {
        self.expression_operable.is(operand)
    }

    fn is_not(&self, operand: bool) -> Expression {
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

pub trait ExpressionSchemaParam {
    fn call_schema(&self, expression_cpp_obj: *mut c_void);
}

impl ExpressionSchemaParam for &str {
    fn call_schema(&self, expression_cpp_obj: *mut c_void) {
        let cstr = self.to_cstring();
        unsafe {
            WCDBRustExpression_setWithSchema(
                expression_cpp_obj,
                CPPType::String as c_int,
                std::ptr::null_mut(),
                cstr.as_ptr(),
            )
        }
    }
}

impl ExpressionSchemaParam for Schema {
    fn call_schema(&self, expression_cpp_obj: *mut c_void) {
        unsafe {
            WCDBRustExpression_setWithSchema(
                expression_cpp_obj,
                Identifier::get_cpp_type(self) as c_int,
                CppObject::get(self),
                std::ptr::null_mut(),
            )
        }
    }
}

pub trait ExpressionCastParam {
    fn create_cpp_obj(&self) -> *mut c_void;
}

impl ExpressionCastParam for &str {
    fn create_cpp_obj(&self) -> *mut c_void {
        let cstr = self.to_cstring();
        unsafe {
            WCDBRustExpression_cast(
                CPPType::String as c_int,
                std::ptr::null_mut(),
                cstr.as_ptr(),
            )
        }
    }
}

impl<T: ExpressionConvertibleTrait + IdentifierTrait> ExpressionCastParam for T {
    fn create_cpp_obj(&self) -> *mut c_void {
        unsafe {
            WCDBRustExpression_cast(
                Identifier::get_cpp_type(self) as c_int,
                CppObject::get(self),
                std::ptr::null_mut(),
            )
        }
    }
}

pub trait ExpressionCaseParam {
    fn create_cpp_obj(&self) -> *mut c_void;
}

impl ExpressionCaseParam for &str {
    fn create_cpp_obj(&self) -> *mut c_void {
        let cstr = self.to_cstring();
        unsafe {
            WCDBRustExpression_caseWithExp(
                CPPType::String as c_int,
                std::ptr::null_mut(),
                cstr.as_ptr(),
            )
        }
    }
}

impl<T: ExpressionConvertibleTrait + IdentifierTrait> ExpressionCaseParam for T {
    fn create_cpp_obj(&self) -> *mut c_void {
        unsafe {
            WCDBRustExpression_caseWithExp(
                Identifier::get_cpp_type(self) as c_int,
                CppObject::get(self),
                std::ptr::null_mut(),
            )
        }
    }
}

pub trait ExpressionOverParam {
    fn call_native(&self, cpp_obj: *mut c_void);
}

impl ExpressionOverParam for WindowDef {
    fn call_native(&self, cpp_obj: *mut c_void) {
        unsafe { WCDBRustExpression_overWindowDef(cpp_obj, CppObject::get(self)) }
    }
}

impl ExpressionOverParam for &str {
    fn call_native(&self, cpp_obj: *mut c_void) {
        let cstr = self.to_cstring();
        unsafe { WCDBRustExpression_overWindow(cpp_obj, cstr.as_ptr()) }
    }
}

impl Expression {
    pub fn new(cpp_obj_opt: Option<*mut c_void>) -> Self {
        Expression {
            expression_operable: ExpressionOperable::new(CPPType::Expression, cpp_obj_opt),
        }
    }

    pub fn function(func_name: &str) -> Self {
        let cpp_obj =
            unsafe { WCDBRustExpression_createWithFunction(func_name.to_cstring().as_ptr()) };
        Expression::new(Some(cpp_obj))
    }

    pub fn schema<T: ExpressionSchemaParam>(&self, param: T) -> &Self {
        param.call_schema(self.get_cpp_obj());
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

    pub fn argument<'a, T>(self, param: T) -> Self
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_cpp_obj) = param.into().get_params();
        unsafe {
            WCDBRustExpression_argument(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long,
                arg_double,
                arg_cpp_obj as *const c_char,
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

    pub fn cast<T: ExpressionCastParam>(param: T) -> Self {
        let cpp_obj = param.create_cpp_obj();
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn r#as(&self, column_type: ColumnType) -> &Self {
        unsafe { WCDBRustExpression_as(self.get_cpp_obj(), column_type as c_int) };
        &self
    }

    pub fn as_result_column(&self, alias: &str) -> ResultColumn {
        let c_string = alias.to_cstring();
        let cpp_obj =
            unsafe { WCDBRustExpression_configAlias(self.get_cpp_obj(), c_string.as_ptr()) };
        ResultColumn::new(cpp_obj)
    }

    pub fn r#case<T: ExpressionCaseParam>(param_opt: Option<T>) -> Self {
        let cpp_obj = match param_opt {
            None => unsafe {
                WCDBRustExpression_caseWithExp(
                    CPPType::Invalid as c_int,
                    std::ptr::null_mut(),
                    std::ptr::null_mut(),
                )
            },
            Some(param) => param.create_cpp_obj(),
        };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Expression, Some(cpp_obj)),
        }
    }

    pub fn when<'a, T>(&self, param: T) -> &Self
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string) = param.into().get_params();
        unsafe {
            WCDBRustExpression_setWithWhenExp(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long as c_longlong,
                arg_double,
                arg_string,
            );
        }
        self
    }

    pub fn then<'a, T>(&self, param: T) -> &Self
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string) = param.into().get_params();
        unsafe {
            WCDBRustExpression_setWithThenExp(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long as c_longlong,
                arg_double,
                arg_string,
            );
        }
        self
    }

    pub fn r#else<'a, T>(&self, param: T) -> &Self
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string) = param.into().get_params();
        unsafe {
            WCDBRustExpression_setWithElseExp(
                self.get_cpp_obj(),
                arg_type as c_int,
                arg_long as c_longlong,
                arg_double,
                arg_string,
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

    pub fn over<T: ExpressionOverParam>(&self, param: T) -> &Self {
        param.call_native(self.get_cpp_obj());
        self
    }
}
