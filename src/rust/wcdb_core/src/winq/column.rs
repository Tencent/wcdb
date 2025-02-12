use crate::base::cpp_object::CppObjectTrait;
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::value::Value;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::expression_operable_trait::ExpressionOperableTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::ordering_term::{Order, OrderingTerm};
use std::ffi::{c_char, c_void, CString};
use std::ptr::null_mut;

extern "C" {
    pub fn WCDBRustColumn_createWithName(name: *const c_char, binding: *mut c_void) -> *mut c_void;
}

pub struct Column {
    expression_operable: ExpressionOperable,
}

impl CppObjectTrait for Column {
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

impl IdentifierTrait for Column {
    fn get_description(&self) -> String {
        self.expression_operable.get_description()
    }
}

impl IdentifierStaticTrait for Column {
    fn get_type() -> i32 {
        CPPType::Column as i32
    }
}

impl IdentifierConvertibleTrait for Column {
    fn as_identifier(&self) -> &Identifier {
        self.expression_operable.as_identifier()
    }
}

impl CppObjectConvertibleTrait for Column {
    fn as_cpp_object(&self) -> *mut c_void {
        self.expression_operable.get_cpp_obj()
    }
}

impl ExpressionConvertibleTrait for Column {}

impl IndexedColumnConvertibleTrait for Column {}

impl ExpressionOperableTrait for Column {
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

    fn collate(&self, collation: &str) -> Expression {
        self.expression_operable
            .collate(Self::get_type(), collation)
    }

    fn substr_int(&self, start: i32, length: i32) -> Expression {
        self.expression_operable
            .substr_int(Self::get_type(), start, length)
    }

    fn substr_long(&self, start: i64, length: i64) -> Expression {
        self.expression_operable
            .substr_long(Self::get_type(), start, length)
    }
}

impl Column {
    pub fn new(name: &str) -> Column {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), null_mut()) };
        Column {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_binding(name: &str, binding_raw: *mut c_void) -> Self {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), binding_raw) };
        Column {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn order(&self, order: Order) -> OrderingTerm {
        return OrderingTerm::new(&self.expression_operable).order(order);
    }
}
