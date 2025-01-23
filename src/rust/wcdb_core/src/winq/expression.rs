use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::column::Column;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::{BinaryOperatorType, ExpressionOperable};
use crate::winq::expression_operable_trait::ExpressionOperableTrait;
use crate::winq::identifier::{
    get_cpp_type, CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait,
};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::literal_value::LiteralValue;
use crate::winq::statement_select::StatementSelect;
use std::ffi::{c_int, c_long, c_void};

extern "C" {
    pub fn WCDBRustExpression_create(value_type: c_int, cpp_obj: *mut c_void) -> *mut c_void;
    // pub fn WCDBRustExpression_argument(
    //     cpp_obj: *mut c_void,
    //     type_i: c_int,
    //     int_value: c_long,
    //     double_value: c_double,
    //     string_value: *const c_char,
    // );
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

    fn between_expr_expr<T>(&self, begin: &T, eng: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        todo!()
    }

    fn between_expr_long<T>(&self, begin: &T, end: i64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        todo!()
    }
}

impl Expression {
    pub fn new() -> Self {
        Expression {
            expression_operable: ExpressionOperable::new(),
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

    pub fn new_with_column(column: Column) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(&column), CppObject::get(&column))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_statement_select(select: StatementSelect) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(&select), CppObject::get(&select))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    // pub fn argument_i64(self, value: i64) -> Self {
    //     self.argument(CPPType::Int, value, 0.0, "".to_string());
    //     self
    // }
    //
    // // todo qixinbing: 怎么用？
    // fn argument(&self, type_i: CPPType, int_value: i64, double_value: f64, string_value: String) {
    //     let c_str = CString::new(string_value).unwrap_or_default();
    //     unsafe {
    //         WCDBRustExpression_argument(
    //             self.get_cpp_obj(),
    //             type_i as i32,
    //             int_value,
    //             double_value,
    //             c_str.as_ptr(),
    //         );
    //     }
    // }

    pub(crate) fn get_expression_operable(&self) -> &ExpressionOperable {
        &self.expression_operable
    }
}
