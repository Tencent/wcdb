use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::value::Value;
use crate::utils::ToCString;
use crate::winq::bind_parameter::BindParameter;
use crate::winq::column::Column;
use crate::winq::column_type::ColumnType;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::expression_operable_trait::ExpressionOperableTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::literal_value::LiteralValue;
use crate::winq::result_column::ResultColumn;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use crate::winq::statement_select::StatementSelect;
use crate::winq::window_def::WindowDef;
use num_traits::FromPrimitive;
use std::ffi::{c_char, c_double, c_int, c_void, CString};
use std::ptr::null;

extern "C" {
    fn WCDBRustExpression_create(value_type: c_int, cpp_obj: *mut c_void) -> *mut c_void;
    // fn WCDBRustExpression_argument(
    //     cpp_obj: *mut c_void,
    //     type_i: c_int,
    //     int_value: i64,
    //     double_value: c_double,
    //     string_value: *const c_char,
    // );

    fn WCDBRustExpression_createWithFunction(func: *const c_char) -> *mut c_void;

    fn WCDBRustExpression_argument(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        // 这里无法加一个 void_ptr: *mut c_void, 为了和 int_value 区分开，int_value 还保持 c_int 类型。
        // 因为 C 层宏WCDBRustCommonValueParameter用的地方太多，改动太大
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_escapeWith(cpp_obj: *mut c_void, string_value: *const c_char);
    fn WCDBRustExpression_createWithExistStatement(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustExpression_createWithNotExistStatement(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustExpression_cast(
        cpp_type: c_int,
        object: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustExpression_caseWithExp(
        cpp_type: c_int,
        object: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustExpression_setWithWhenExp(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_setWithThenExp(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_setWithElseExp(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustExpression_createWithWindowFunction(func_name: *const c_char) -> *mut c_void;
    fn WCDBRustExpression_filter(cpp_obj: *mut c_void, condition: *mut c_void);
    fn WCDBRustExpression_overWindow(cpp_obj: *mut c_void, window: *const c_char);
    fn WCDBRustExpression_overWindowDef(cpp_obj: *mut c_void, window_def: *mut c_void);
    fn WCDBRustExpression_distinct(cpp_obj: *mut c_void);
    fn WCDBRustExpression_invoke(cpp_obj: *mut c_void);
    fn WCDBRustExpression_invokeAll(cpp_obj: *mut c_void);
    fn WCDBRustExpression_configAlias(cpp_obj: *mut c_void, alias: *const c_char) -> *mut c_void;
    fn WCDBRustExpression_as(cpp_obj: *mut c_void, column_type: c_int);
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

impl IdentifierTrait for Expression {
    fn get_description(&self) -> String {
        self.expression_operable.get_description()
    }
}
impl CppObjectConvertibleTrait for Expression {
    fn as_cpp_object(&self) -> *mut c_void {
        self.expression_operable.get_cpp_obj()
    }
}

impl IdentifierConvertibleTrait for Expression {
    fn as_identifier(&self) -> &Identifier {
        self.expression_operable.as_identifier()
    }
}

impl IdentifierStaticTrait for Expression {
    fn get_type() -> i32 {
        CPPType::Expression as i32
    }
}

impl IndexedColumnConvertibleTrait for Expression {}

impl ExpressionConvertibleTrait for Expression {}

impl ExpressionOperableTrait for Expression {
    fn is_null(&self) -> Expression {
        self.expression_operable
            .null_operate(Self::get_type(), false)
    }

    fn not_null(&self) -> Expression {
        self.expression_operable
            .null_operate(Self::get_type(), true)
    }

    fn or<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable.or(Self::get_type(), operand)
    }

    fn and<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable.and(Self::get_type(), operand)
    }

    fn multiply_expression_convertible<T>(&mut self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .multiply_expression_convertible(Self::get_type(), operand)
    }

    fn multiply_byte(&mut self, operand: i8) -> Expression {
        self.expression_operable
            .multiply_long(Self::get_type(), operand as i64)
    }

    fn multiply_short(&mut self, operand: i16) -> Expression {
        self.expression_operable
            .multiply_long(Self::get_type(), operand as i64)
    }

    fn multiply_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .multiply_long(Self::get_type(), operand as i64)
    }

    fn multiply_long(&mut self, operand: i64) -> Expression {
        self.expression_operable
            .multiply_long(Self::get_type(), operand)
    }

    fn multiply_float(&mut self, operand: f32) -> Expression {
        self.expression_operable
            .multiply_long(Self::get_type(), operand as i64)
    }

    fn multiply_double(&mut self, operand: f64) -> Expression {
        self.expression_operable
            .multiply_double(Self::get_type(), operand)
    }

    fn divide_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .divide_expression_convertible(Self::get_type(), operand)
    }

    fn divide_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .divide_long(Self::get_type(), operand as i64)
    }

    fn divide_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .divide_long(Self::get_type(), operand as i64)
    }

    fn divide_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .divide_long(Self::get_type(), operand as i64)
    }

    fn divide_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .divide_long(Self::get_type(), operand)
    }

    fn divide_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .divide_long(Self::get_type(), operand as i64)
    }

    fn divide_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .divide_double(Self::get_type(), operand)
    }

    fn mod_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .mod_expression_convertible(Self::get_type(), operand)
    }

    fn mod_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .mod_long(Self::get_type(), operand as i64)
    }

    fn mod_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .mod_long(Self::get_type(), operand as i64)
    }

    fn mod_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .mod_long(Self::get_type(), operand as i64)
    }

    fn mod_long(&self, operand: i64) -> Expression {
        self.expression_operable.mod_long(Self::get_type(), operand)
    }

    fn mod_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .mod_long(Self::get_type(), operand as i64)
    }

    fn mod_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .mod_double(Self::get_type(), operand)
    }

    fn add_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .add_expression_convertible(Self::get_type(), operand)
    }

    fn add_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .add_long(Self::get_type(), operand as i64)
    }

    fn add_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .add_long(Self::get_type(), operand as i64)
    }

    fn add_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .add_long(Self::get_type(), operand as i64)
    }

    fn add_long(&self, operand: i64) -> Expression {
        self.expression_operable.add_long(Self::get_type(), operand)
    }

    fn add_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .add_long(Self::get_type(), operand as i64)
    }

    fn add_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .add_double(Self::get_type(), operand)
    }

    fn minus_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .minus_expression_convertible(Self::get_type(), operand)
    }

    fn minus_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .minus_long(Self::get_type(), operand as i64)
    }

    fn minus_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .minus_long(Self::get_type(), operand as i64)
    }

    fn minus_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .minus_long(Self::get_type(), operand as i64)
    }

    fn minus_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .minus_long(Self::get_type(), operand)
    }

    fn minus_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .minus_long(Self::get_type(), operand as i64)
    }

    fn minus_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .minus_double(Self::get_type(), operand)
    }

    fn left_shift_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .left_shift_expression_convertible(Self::get_type(), operand)
    }

    fn left_shift_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .left_shift_long(Self::get_type(), operand as i64)
    }

    fn left_shift_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .left_shift_long(Self::get_type(), operand as i64)
    }

    fn left_shift_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .left_shift_long(Self::get_type(), operand as i64)
    }

    fn left_shift_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .left_shift_long(Self::get_type(), operand)
    }

    fn right_shift_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .right_shift_expression_convertible(Self::get_type(), operand)
    }

    fn right_shift_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .right_shift_long(Self::get_type(), operand as i64)
    }

    fn right_shift_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .right_shift_long(Self::get_type(), operand as i64)
    }

    fn right_shift_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .right_shift_long(Self::get_type(), operand as i64)
    }

    fn right_shift_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .right_shift_long(Self::get_type(), operand)
    }

    fn bit_and_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .bit_and_expression_convertible(Self::get_type(), operand)
    }

    fn bit_and_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .bit_and_long(Self::get_type(), operand as i64)
    }

    fn bit_and_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .bit_and_long(Self::get_type(), operand as i64)
    }

    fn bit_and_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .bit_and_long(Self::get_type(), operand as i64)
    }

    fn bit_and_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .bit_and_long(Self::get_type(), operand)
    }

    fn bit_or_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .bit_or_expression_convertible(Self::get_type(), operand)
    }

    fn bit_or_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .bit_or_long(Self::get_type(), operand as i64)
    }

    fn bit_or_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .bit_or_long(Self::get_type(), operand as i64)
    }

    fn bit_or_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .bit_or_long(Self::get_type(), operand as i64)
    }

    fn bit_or_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .bit_or_long(Self::get_type(), operand)
    }

    fn lt_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .lt_expression_convertible(Self::get_type(), operand)
    }

    fn lt_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .lt_long(Self::get_type(), operand as i64)
    }

    fn lt_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .lt_long(Self::get_type(), operand as i64)
    }

    fn lt_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .lt_long(Self::get_type(), operand as i64)
    }

    fn lt_long(&self, operand: i64) -> Expression {
        self.expression_operable.lt_long(Self::get_type(), operand)
    }

    fn lt_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .lt_double(Self::get_type(), operand)
    }

    fn lt_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .lt_string(Self::get_type(), operand)
    }

    fn le_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .le_expression_convertible(Self::get_type(), operand)
    }

    fn le_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .le_long(Self::get_type(), operand as i64)
    }

    fn le_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .le_long(Self::get_type(), operand as i64)
    }

    fn le_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .le_long(Self::get_type(), operand as i64)
    }

    fn le_long(&self, operand: i64) -> Expression {
        self.expression_operable.le_long(Self::get_type(), operand)
    }

    fn le_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .le_double(Self::get_type(), operand as f64)
    }

    fn le_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .le_double(Self::get_type(), operand)
    }

    fn le_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .le_string(Self::get_type(), operand)
    }

    fn gt_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .gt_expression_convertible(Self::get_type(), operand)
    }

    fn gt_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .gt_long(Self::get_type(), operand as i64)
    }

    fn gt_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .gt_long(Self::get_type(), operand as i64)
    }

    fn gt_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .gt_long(Self::get_type(), operand as i64)
    }

    fn gt_long(&self, operand: i64) -> Expression {
        self.expression_operable.gt_long(Self::get_type(), operand)
    }

    fn gt_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .gt_double(Self::get_type(), operand as f64)
    }

    fn gt_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .gt_double(Self::get_type(), operand)
    }

    fn gt_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .gt_string(Self::get_type(), operand)
    }

    fn ge_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .ge_expression_convertible(Self::get_type(), operand)
    }

    fn ge_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .ge_long(Self::get_type(), operand as i64)
    }

    fn ge_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .ge_long(Self::get_type(), operand as i64)
    }

    fn ge_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .ge_long(Self::get_type(), operand as i64)
    }

    fn ge_long(&self, operand: i64) -> Expression {
        self.expression_operable.ge_long(Self::get_type(), operand)
    }

    fn ge_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .ge_double(Self::get_type(), operand as f64)
    }

    fn ge_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .ge_double(Self::get_type(), operand)
    }

    fn ge_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .ge_string(Self::get_type(), operand)
    }

    fn eq_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .eq_expression_convertible(Self::get_type(), operand)
    }

    fn eq_bool(&self, operand: bool) -> Expression {
        self.expression_operable.eq_bool(Self::get_type(), operand)
    }

    fn eq_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .eq_long(Self::get_type(), operand as i64)
    }

    fn eq_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .eq_long(Self::get_type(), operand as i64)
    }

    fn eq_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .eq_long(Self::get_type(), operand as i64)
    }

    fn eq_long(&self, operand: i64) -> Expression {
        self.expression_operable.eq_long(Self::get_type(), operand)
    }

    fn eq_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .eq_double(Self::get_type(), operand as f64)
    }

    fn eq_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .eq_double(Self::get_type(), operand)
    }

    fn eq_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .eq_string(Self::get_type(), operand)
    }

    fn not_eq_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_eq_expression_convertible(Self::get_type(), operand)
    }

    fn not_eq_bool(&self, operand: bool) -> Expression {
        self.expression_operable
            .not_eq_bool(Self::get_type(), operand)
    }

    fn not_eq_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .not_eq_long(Self::get_type(), operand as i64)
    }

    fn not_eq_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .not_eq_long(Self::get_type(), operand as i64)
    }

    fn not_eq_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .not_eq_long(Self::get_type(), operand as i64)
    }

    fn not_eq_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .not_eq_long(Self::get_type(), operand)
    }

    fn not_eq_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .not_eq_double(Self::get_type(), operand as f64)
    }

    fn not_eq_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .not_eq_double(Self::get_type(), operand)
    }

    fn not_eq_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .not_eq_string(Self::get_type(), operand)
    }

    fn concat_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .concat_expression_convertible(Self::get_type(), operand)
    }

    fn concat_byte(&self, operand: i8) -> Expression {
        self.expression_operable
            .concat_long(Self::get_type(), operand as i64)
    }

    fn concat_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .concat_long(Self::get_type(), operand as i64)
    }

    fn concat_int(&self, operand: i32) -> Expression {
        self.expression_operable
            .concat_long(Self::get_type(), operand as i64)
    }

    fn concat_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .concat_long(Self::get_type(), operand)
    }

    fn concat_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .concat_double(Self::get_type(), operand as f64)
    }

    fn concat_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .concat_double(Self::get_type(), operand)
    }

    fn concat_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .concat_string(Self::get_type(), operand)
    }

    fn between_expr_expr<T>(&self, begin: &T, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_operate_with_expression_convertible(Self::get_type(), begin, end)
    }

    fn between_expr_long<T>(&self, begin: &T, end: i64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_expr_long(Self::get_type(), begin, end)
    }

    fn between_expr_double<T>(&self, begin: &T, end: f64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_expr_double(Self::get_type(), begin, end)
    }

    fn between_expr_string<T>(&self, begin: &T, end: &str) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_expr_string(Self::get_type(), begin, end)
    }

    fn between_long_expr<T>(&self, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_long_expr(Self::get_type(), begin, end)
    }

    fn between_long_long(&self, begin: i64, end: i64) -> Expression {
        self.expression_operable
            .between_long_long(Self::get_type(), begin, end)
    }

    fn between_long_double(&self, begin: i64, end: f64) -> Expression {
        self.expression_operable
            .between_long_double(Self::get_type(), begin, end)
    }

    fn between_long_string(&self, begin: i64, end: &str) -> Expression {
        self.expression_operable
            .between_long_string(Self::get_type(), begin, end)
    }

    fn between_double_expr<T>(&self, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_double_expr(Self::get_type(), begin, end)
    }

    fn between_double_long(&self, begin: f64, end: i64) -> Expression {
        self.expression_operable
            .between_double_long(Self::get_type(), begin, end)
    }

    fn between_double_double(&self, begin: f64, end: f64) -> Expression {
        self.expression_operable
            .between_double_double(Self::get_type(), begin, end)
    }

    fn between_double_string(&self, begin: f64, end: &str) -> Expression {
        self.expression_operable
            .between_double_string(Self::get_type(), begin, end)
    }

    fn between_string_expr<T>(&self, begin: &str, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .between_string_expr(Self::get_type(), begin, end)
    }

    fn between_string_long(&self, begin: &str, end: i64) -> Expression {
        self.expression_operable
            .between_string_long(Self::get_type(), begin, end)
    }

    fn between_string_double(&self, begin: &str, end: f64) -> Expression {
        self.expression_operable
            .between_string_double(Self::get_type(), begin, end)
    }

    fn between_string_string(&self, begin: &str, end: &str) -> Expression {
        self.expression_operable
            .between_string_string(Self::get_type(), begin, end)
    }
    fn not_between_expr_expr<T>(&self, begin: &T, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_expr_expr(Self::get_type(), begin, end)
    }

    fn not_between_expr_long<T>(&self, begin: &T, end: i64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_expr_long(Self::get_type(), begin, end)
    }

    fn not_between_expr_double<T>(&self, begin: &T, end: f64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_expr_double(Self::get_type(), begin, end)
    }

    fn not_between_expr_string<T>(&self, begin: &T, end: &str) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_expr_string(Self::get_type(), begin, end)
    }

    fn not_between_long_expr<T>(&self, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_long_expr(Self::get_type(), begin, end)
    }

    fn not_between_long_long(&self, begin: i64, end: i64) -> Expression {
        self.expression_operable
            .not_between_long_long(Self::get_type(), begin, end)
    }

    fn not_between_long_double(&self, begin: i64, end: f64) -> Expression {
        self.expression_operable
            .not_between_long_double(Self::get_type(), begin, end)
    }

    fn not_between_long_string(&self, begin: i64, end: &str) -> Expression {
        self.expression_operable
            .not_between_long_string(Self::get_type(), begin, end)
    }

    fn not_between_double_expr<T>(&self, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_double_expr(Self::get_type(), begin, end)
    }

    fn not_between_double_long(&self, begin: f64, end: i64) -> Expression {
        self.expression_operable
            .not_between_double_long(Self::get_type(), begin, end)
    }

    fn not_between_double_double(&self, begin: f64, end: f64) -> Expression {
        self.expression_operable
            .not_between_double_double(Self::get_type(), begin, end)
    }

    fn not_between_double_string(&self, begin: f64, end: &str) -> Expression {
        self.expression_operable
            .not_between_double_string(Self::get_type(), begin, end)
    }

    fn not_between_string_expr<T>(&self, begin: &str, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .not_between_string_expr(Self::get_type(), begin, end)
    }

    fn not_between_string_long(&self, begin: &str, end: i64) -> Expression {
        self.expression_operable
            .not_between_string_long(Self::get_type(), begin, end)
    }

    fn not_between_string_double(&self, begin: &str, end: f64) -> Expression {
        self.expression_operable
            .not_between_string_double(Self::get_type(), begin, end)
    }

    fn not_between_string_string(&self, begin: &str, end: &str) -> Expression {
        self.expression_operable
            .not_between_string_string(Self::get_type(), begin, end)
    }

    fn in_short(&self, operands: Vec<i16>) -> Expression {
        self.expression_operable
            .in_short(Self::get_type(), operands, false)
    }
    fn in_int(&self, operands: Vec<i32>) -> Expression {
        self.expression_operable
            .in_int(Self::get_type(), operands, false)
    }

    fn in_long(&self, operands: Vec<i64>) -> Expression {
        self.expression_operable
            .in_long(Self::get_type(), operands, false)
    }

    fn in_float(&self, operands: Vec<f32>) -> Expression {
        self.expression_operable
            .in_float(Self::get_type(), operands, false)
    }

    fn in_double(&self, operands: Vec<f64>) -> Expression {
        self.expression_operable
            .in_double(Self::get_type(), operands, false)
    }

    fn in_string(&self, operands: Vec<&str>) -> Expression {
        self.expression_operable
            .in_string(Self::get_type(), operands, false)
    }

    fn in_value(&self, operands: Vec<Value>) -> Expression {
        self.expression_operable
            .in_object(Option::Some(operands), Self::get_type(), false)
    }

    fn not_in_short(&self, operands: Vec<i16>) -> Expression {
        self.expression_operable
            .in_short(Self::get_type(), operands, true)
    }

    fn not_in_int(&self, operands: Vec<i32>) -> Expression {
        self.expression_operable
            .in_int(Self::get_type(), operands, true)
    }

    fn not_in_long(&self, operands: Vec<i64>) -> Expression {
        self.expression_operable
            .in_long(Self::get_type(), operands, true)
    }

    fn not_in_float(&self, operands: Vec<f32>) -> Expression {
        self.expression_operable
            .in_float(Self::get_type(), operands, true)
    }

    fn not_in_double(&self, operands: Vec<f64>) -> Expression {
        self.expression_operable
            .in_double(Self::get_type(), operands, true)
    }

    fn not_in_string(&self, operands: Vec<&str>) -> Expression {
        self.expression_operable
            .in_string(Self::get_type(), operands, true)
    }

    fn not_in_value(&self, operands: Vec<Value>) -> Expression {
        self.expression_operable
            .in_object(Option::Some(operands), Self::get_type(), true)
    }

    fn in_table(&self, table: &str) -> Expression {
        self.expression_operable.in_table(Self::get_type(), table)
    }

    fn collate(&self, collation: &str) -> Expression {
        self.expression_operable
            .collate(Self::get_type(), collation)
    }

    fn substr_short(&self, start: i16, length: i16) -> Expression {
        self.expression_operable
            .substr_int(Self::get_type(), start as i32, length as i32)
    }

    fn substr_int(&self, start: i32, length: i32) -> Expression {
        self.expression_operable
            .substr_int(Self::get_type(), start, length)
    }

    fn substr_long(&self, start: i64, length: i64) -> Expression {
        self.expression_operable
            .substr_long(Self::get_type(), start, length)
    }

    fn like(&self, content: &str) -> Expression {
        self.expression_operable
            .like(Self::get_type(), content, false)
    }

    fn not_like(&self, content: &str) -> Expression {
        self.expression_operable
            .like(Self::get_type(), content, true)
    }

    fn glob(&self, content: &str) -> Expression {
        self.expression_operable
            .glob(Self::get_type(), content, false)
    }

    fn not_glob(&self, content: &str) -> Expression {
        self.expression_operable
            .glob(Self::get_type(), content, true)
    }

    fn match_string(&self, content: &str) -> Expression {
        self.expression_operable
            .match_string(Self::get_type(), content, false)
    }

    fn not_match(&self, content: &str) -> Expression {
        self.expression_operable
            .match_string(Self::get_type(), content, true)
    }

    fn regexp(&self, content: &str) -> Expression {
        self.expression_operable
            .regexp(Self::get_type(), content, false)
    }

    fn not_regexp(&self, content: &str) -> Expression {
        self.expression_operable
            .regexp(Self::get_type(), content, true)
    }

    fn is_bool(&self, operand: bool) -> Expression {
        self.expression_operable
            .is_bool(Self::get_type(), operand, false)
    }

    fn is_byte(&self, operand: u8) -> Expression {
        self.expression_operable
            .is_byte(Self::get_type(), operand, false)
    }

    fn is_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .is_short(Self::get_type(), operand, false)
    }

    fn is_i32(&self, operand: i32) -> Expression {
        self.expression_operable
            .is_i32(Self::get_type(), operand, false)
    }

    fn is_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .is_long(Self::get_type(), operand, false)
    }

    fn is_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .is_float(Self::get_type(), operand, false)
    }

    fn is_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .is_double(Self::get_type(), operand, false)
    }

    fn is_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .is_string(Self::get_type(), operand, false)
    }

    fn is_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .is_expression_convertible(Self::get_type(), operand, false)
    }

    fn is_not_bool(&self, operand: bool) -> Expression {
        self.expression_operable
            .is_bool(Self::get_type(), operand, true)
    }

    fn is_not_byte(&self, operand: u8) -> Expression {
        self.expression_operable
            .is_byte(Self::get_type(), operand, true)
    }

    fn is_not_short(&self, operand: i16) -> Expression {
        self.expression_operable
            .is_short(Self::get_type(), operand, true)
    }

    fn is_not_i32(&self, operand: i32) -> Expression {
        self.expression_operable
            .is_i32(Self::get_type(), operand, true)
    }

    fn is_not_long(&self, operand: i64) -> Expression {
        self.expression_operable
            .is_long(Self::get_type(), operand, true)
    }

    fn is_not_float(&self, operand: f32) -> Expression {
        self.expression_operable
            .is_float(Self::get_type(), operand, true)
    }

    fn is_not_double(&self, operand: f64) -> Expression {
        self.expression_operable
            .is_double(Self::get_type(), operand, true)
    }

    fn is_not_string(&self, operand: &str) -> Expression {
        self.expression_operable
            .is_string(Self::get_type(), operand, true)
    }

    fn is_not_expression_convertible<T>(&self, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.expression_operable
            .is_expression_convertible(Self::get_type(), operand, true)
    }

    fn avg(&self) -> Expression {
        self.expression_operable.avg(Self::get_type())
    }

    fn count(&self) -> Expression {
        self.expression_operable.count(Self::get_type())
    }

    fn group_concat(&self) -> Expression {
        self.expression_operable.group_concat(Self::get_type())
    }

    fn group_concat_string(&self, sperator: &str) -> Expression {
        self.expression_operable
            .group_concat_string(Self::get_type(), sperator)
    }

    fn max(&self) -> Expression {
        self.expression_operable.max(Self::get_type())
    }

    fn min(&self) -> Expression {
        self.expression_operable.min(Self::get_type())
    }

    fn sum(&self) -> Expression {
        self.expression_operable.sum(Self::get_type())
    }

    fn total(&self) -> Expression {
        self.expression_operable.total(Self::get_type())
    }

    fn abs(&self) -> Expression {
        self.expression_operable.abs(Self::get_type())
    }

    fn hex(&self) -> Expression {
        self.expression_operable.hex(Self::get_type())
    }

    fn length(&self) -> Expression {
        self.expression_operable.length(Self::get_type())
    }

    fn lower(&self) -> Expression {
        self.expression_operable.lower(Self::get_type())
    }

    fn upper(&self) -> Expression {
        self.expression_operable.upper(Self::get_type())
    }

    fn round(&self) -> Expression {
        self.expression_operable.round(Self::get_type())
    }

    fn match_info(&self) -> Expression {
        self.expression_operable.match_info(Self::get_type())
    }

    fn offsets(&self) -> Expression {
        self.expression_operable.offsets(Self::get_type())
    }

    fn snippet(&self) -> Expression {
        self.expression_operable.snippet(Self::get_type())
    }

    fn bm25(&self) -> Expression {
        self.expression_operable.bm25(Self::get_type())
    }

    fn highlight(&self) -> Expression {
        self.expression_operable.highlight(Self::get_type())
    }

    fn substring_match_info(&self) -> Expression {
        self.expression_operable
            .substring_match_info(Self::get_type())
    }
}

impl ResultColumnConvertibleTrait for Expression {}

impl Expression {
    pub fn new() -> Self {
        Expression {
            expression_operable: ExpressionOperable::new(),
        }
    }

    // 通用的私有方法来处理 when/then/else 的值设置
    fn set_expression_value(
        &self,
        set_func: unsafe extern "C" fn(*mut c_void, c_int, *mut c_void, c_double, *const c_char),
        cpp_type: c_int,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    ) {
        unsafe {
            set_func(
                self.get_cpp_obj(),
                cpp_type,
                int_value,
                double_value,
                string_value,
            );
        }
    }

    pub fn new_with_literal_value(value: LiteralValue) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(&value), CppObject::get(&value))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_bind_parameter(bind_parameter: BindParameter) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(
                Identifier::get_cpp_type(&bind_parameter),
                CppObject::get(&bind_parameter),
            )
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_column(column: &Column) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(column), CppObject::get(column))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_statement_select(select: &StatementSelect) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(select), CppObject::get(select))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn argument_with_expression_convertible<T>(self, arg: &T) -> Self
    where
        T: IdentifierStaticTrait
            + IdentifierConvertibleTrait
            + ExpressionConvertibleTrait
            + CppObjectTrait,
    {
        self.argument(
            Identifier::get_cpp_type(arg),
            CppObject::get(arg),
            0f64,
            None,
        );
        self
    }

    fn argument(
        &self,
        type_i: i32,
        int_value: *mut c_void,
        double_value: f64,
        string_value: Option<&str>,
    ) {
        let cpp_obj = self.get_cpp_obj();
        let _cstring; // 用于保持CString的生命周期
        let mut cstr: *const c_char = null();
        if let Some(string_val) = string_value {
            _cstring = string_val.to_cstring();
            cstr = _cstring.as_ptr();
        }
        unsafe {
            WCDBRustExpression_argument(cpp_obj, type_i, int_value, double_value as c_double, cstr);
        }
    }

    pub(crate) fn get_expression_operable(&self) -> &ExpressionOperable {
        &self.expression_operable
    }

    pub(crate) fn function(func_name: &str) -> Expression {
        let c_str = func_name.to_cstring();
        let cpp_obj = unsafe { WCDBRustExpression_createWithFunction(c_str.as_ptr()) };
        ExpressionOperable::create_expression(cpp_obj)
    }

    pub(crate) fn argument_expression_convertible_trait(
        mut self,
        cpp_type: i32,
        arg_cpp_object: *mut c_void,
    ) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_argument(cpp_obj, cpp_type, arg_cpp_object, 0 as c_double, null());
        };
        self
    }

    pub(crate) fn argument_int(mut self, arg: i32) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_argument(
                cpp_obj,
                CPPType::Int as c_int,
                arg as *mut c_void,
                0 as c_double,
                null(),
            );
        };
        self
    }

    pub(crate) fn argument_long(mut self, arg: i64) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_argument(
                cpp_obj,
                CPPType::Int as c_int,
                arg as *mut c_void,
                0 as c_double,
                null(),
            );
        };
        self
    }

    pub(crate) fn argument_float(mut self, arg: f32) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_argument(
                cpp_obj,
                CPPType::Double as c_int,
                0 as *mut c_void,
                arg as c_double,
                null(),
            );
        };
        self
    }

    pub(crate) fn argument_double(mut self, arg: f64) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_argument(
                cpp_obj,
                CPPType::Double as c_int,
                0 as *mut c_void,
                arg as c_double,
                null(),
            );
        };
        self
    }

    pub(crate) fn argument_string(mut self, arg: &str) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        let cstr = arg.to_cstring();
        if !arg.is_empty() {
            unsafe {
                WCDBRustExpression_argument(
                    cpp_obj,
                    CPPType::String as c_int,
                    0 as *mut c_void,
                    0 as c_double,
                    cstr.as_ptr(),
                );
            };
        } else {
            unsafe {
                WCDBRustExpression_argument(
                    cpp_obj,
                    CPPType::String as c_int,
                    0 as *mut c_void,
                    0 as c_double,
                    null(),
                );
            };
        }
        self
    }

    pub fn escape(mut self, content: &str) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        let cstr = content.to_cstring();
        unsafe {
            WCDBRustExpression_escapeWith(cpp_obj, cstr.as_ptr());
        }
        self
    }

    pub fn exists(select: &StatementSelect) -> Expression {
        let mut exp = Expression::new();
        let cpp_obj = unsafe { WCDBRustExpression_createWithExistStatement(select.get_cpp_obj()) };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn not_exists(select: &StatementSelect) -> Expression {
        let mut exp = Expression::new();
        let cpp_obj =
            unsafe { WCDBRustExpression_createWithNotExistStatement(select.get_cpp_obj()) };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn cast(column_name: &str) -> Expression {
        let mut exp = Expression::new();
        let cstr = column_name.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpression_cast(CPPType::String as c_int, 0 as *mut c_void, cstr.as_ptr())
        };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn cast_with_expression_convertible<T>(expression: &T) -> Expression
    where
        T: IdentifierStaticTrait
            + IdentifierConvertibleTrait
            + ExpressionConvertibleTrait
            + CppObjectTrait,
    {
        let mut exp = Expression::new();
        let cpp_obj = unsafe {
            WCDBRustExpression_cast(
                Identifier::get_cpp_type(expression),
                CppObject::get(expression),
                null(),
            )
        };
        exp.set_cpp_obj(cpp_obj);
        exp
    }
    pub fn distinct(mut self) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_distinct(cpp_obj);
        }
        self
    }

    pub fn invoke(self) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_invoke(cpp_obj);
        }
        self
    }

    pub fn invoke_all(self) -> Expression {
        let cpp_obj = self.get_cpp_obj();
        unsafe {
            WCDBRustExpression_invokeAll(cpp_obj);
        }
        self
    }

    pub fn as_with_column_type(&self, column_type: ColumnType) -> &Self {
        unsafe { WCDBRustExpression_as(self.get_cpp_obj(), column_type as c_int) };
        &self
    }

    pub fn as_(&self, alias: &str) -> ResultColumn {
        let cstr = alias.to_cstring();
        let cpp_obj = unsafe { WCDBRustExpression_configAlias(self.get_cpp_obj(), cstr.as_ptr()) };
        ResultColumn::new_with_cpp_obj(cpp_obj)
    }

    pub fn _case() -> Expression {
        let mut exp = Expression::new();
        let cpp_obj = unsafe {
            WCDBRustExpression_caseWithExp(CPPType::Invalid as c_int, 0 as *mut c_void, null())
        };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn _cast_with_column_name(column_name: &str) -> Expression {
        if column_name.is_empty() {
            return Expression::_case();
        }
        let mut exp = Expression::new();
        let cstr = column_name.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpression_caseWithExp(
                CPPType::String as c_int,
                0 as *mut c_void,
                cstr.as_ptr(),
            )
        };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn _cast_with_expression_convertible<T>(expression: &T) -> Expression
    where
        T: IdentifierStaticTrait
            + IdentifierConvertibleTrait
            + ExpressionConvertibleTrait
            + CppObjectTrait,
    {
        let mut exp = Expression::new();
        let cpp_obj = unsafe {
            WCDBRustExpression_caseWithExp(
                Identifier::get_cpp_type(expression),
                CppObject::get(expression),
                null(),
            )
        };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn when_with_bool(self, arg: bool) -> Expression {
        let int_val = if arg { 1 } else { 0 };
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Bool as c_int,
            int_val as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn when_with_i8(self, arg: i8) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn when_with_i16(self, arg: i16) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn when_with_i32(self, arg: i32) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn when_with_i64(self, arg: i64) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn when_with_f32(self, arg: f32) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Double as c_int,
            0 as *mut c_void,
            arg as c_double,
            null(),
        );
        self
    }
    pub fn when_with_f64(self, arg: f64) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            CPPType::Double as c_int,
            0 as *mut c_void,
            arg as c_double,
            null(),
        );
        self
    }
    pub fn when_with_string(self, arg: &str) -> Expression {
        if arg.is_empty() {
            self.set_expression_value(
                WCDBRustExpression_setWithWhenExp,
                CPPType::Null as c_int,
                0 as *mut c_void,
                0 as c_double,
                null(),
            );
        } else {
            let cstr = arg.to_cstring();
            self.set_expression_value(
                WCDBRustExpression_setWithWhenExp,
                CPPType::String as c_int,
                0 as *mut c_void,
                0 as c_double,
                cstr.as_ptr(),
            );
        }
        self
    }
    pub fn when_with_expression_convertible<T>(self, arg: &T) -> Expression
    where
        T: IdentifierStaticTrait
            + IdentifierConvertibleTrait
            + ExpressionConvertibleTrait
            + CppObjectTrait,
    {
        self.set_expression_value(
            WCDBRustExpression_setWithWhenExp,
            Identifier::get_cpp_type(arg),
            CppObject::get(arg),
            0 as c_double,
            null(),
        );
        self
    }

    pub fn then_with_bool(self, arg: bool) -> Expression {
        let int_val = if arg { 1 } else { 0 };
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Bool as c_int,
            int_val as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn then_with_i8(self, arg: i8) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn then_with_i16(self, arg: i16) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn then_with_i32(self, arg: i32) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn then_with_i64(self, arg: i64) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn then_with_f32(self, arg: f32) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Double as c_int,
            0 as *mut c_void,
            arg as c_double,
            null(),
        );
        self
    }
    pub fn then_with_f64(self, arg: f64) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithThenExp,
            CPPType::Double as c_int,
            0 as *mut c_void,
            arg as c_double,
            null(),
        );
        self
    }
    pub fn then_with_string(self, arg: &str) -> Expression {
        if arg.is_empty() {
            self.set_expression_value(
                WCDBRustExpression_setWithThenExp,
                CPPType::Null as c_int,
                0 as *mut c_void,
                0 as c_double,
                null(),
            );
        } else {
            let cstr = arg.to_cstring();
            self.set_expression_value(
                WCDBRustExpression_setWithThenExp,
                CPPType::String as c_int,
                0 as *mut c_void,
                0 as c_double,
                cstr.as_ptr(),
            );
        }
        self
    }

    pub fn else_with_bool(self, arg: bool) -> Expression {
        let int_val = if arg { 1 } else { 0 };
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Bool as c_int,
            int_val as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn else_with_i8(self, arg: i8) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn else_with_i16(self, arg: i16) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn else_with_i32(self, arg: i32) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn else_with_i64(self, arg: i64) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Int as c_int,
            arg as *mut c_void,
            0 as c_double,
            null(),
        );
        self
    }
    pub fn else_with_f32(self, arg: f32) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Double as c_int,
            0 as *mut c_void,
            arg as c_double,
            null(),
        );
        self
    }
    pub fn else_with_f64(self, arg: f64) -> Expression {
        self.set_expression_value(
            WCDBRustExpression_setWithElseExp,
            CPPType::Double as c_int,
            0 as *mut c_void,
            arg as c_double,
            null(),
        );
        self
    }
    pub fn else_with_string(self, arg: &str) -> Expression {
        if arg.is_empty() {
            self.set_expression_value(
                WCDBRustExpression_setWithElseExp,
                CPPType::Null as c_int,
                0 as *mut c_void,
                0 as c_double,
                null(),
            );
        } else {
            let cstr = arg.to_cstring();
            self.set_expression_value(
                WCDBRustExpression_setWithElseExp,
                CPPType::String as c_int,
                0 as *mut c_void,
                0 as c_double,
                cstr.as_ptr(),
            );
        }
        self
    }
    pub fn window_function(func_name: &str) -> Expression {
        let mut exp = Expression::new();
        let cstr = func_name.to_cstring();
        let cpp_obj = unsafe { WCDBRustExpression_createWithWindowFunction(cstr.as_ptr()) };
        exp.set_cpp_obj(cpp_obj);
        exp
    }

    pub fn filter(self, condition: &Expression) -> Expression {
        unsafe { WCDBRustExpression_filter(self.get_cpp_obj(), condition.get_cpp_obj()) };
        self
    }

    pub fn over_with_window_def(self, window_def: &WindowDef) -> Expression {
        unsafe { WCDBRustExpression_overWindowDef(self.get_cpp_obj(), CppObject::get(window_def)) };
        self
    }

    pub fn over_with_string(self, window: &str) -> Expression {
        let cstr = window.to_cstring();
        unsafe { WCDBRustExpression_overWindow(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }
}
