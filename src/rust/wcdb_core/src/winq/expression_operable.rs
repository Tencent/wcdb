use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable_trait::ExpressionOperableTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_long, c_void, CString};
use std::ptr::null;

extern "C" {
    pub fn WCDBRustExpressionOperable_binaryOperate(
        left_type: c_int,
        left: *mut c_void,
        right_type: c_int,
        right_long: c_long,
        right_double: c_double,
        right_string: *const c_char,
        operator_type: c_int,
        is_not: bool,
    ) -> *mut c_void;

    pub fn WCDBRustExpressionOperable_betweenOperate(
        operand_type: c_int,
        operand: *mut c_void,
        left_type: c_int,
        left_long: *mut c_void,
        left_double: c_double,
        left_string: *const c_char,
        right_type: c_int,
        right_long: *mut c_void,
        right_double: c_double,
        right_string: *const c_char,
        is_not: bool,
    ) -> *mut c_void;

    pub fn WCDBRustExpressionOperable_inOperate(
        operand_type: c_int,
        operand: *mut c_void,
        cpp_type: c_int,
        long_array: *const c_long,
        double_array: *const c_double,
        string_array: *const *const c_char,
        array_length: c_int,
        is_not: bool,
    ) -> *mut c_void;

    pub fn WCDBRustExpressionOperable_collateOperate(
        cpp_type: c_int,
        operand: *mut c_void,
        collation: *const c_char,
    ) -> *mut c_void;
}

#[derive(Debug)]
pub(crate) struct ExpressionOperable {
    identifier: Identifier,
}

impl CppObjectTrait for ExpressionOperable {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object();
    }
}

impl IdentifierTrait for ExpressionOperable {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for ExpressionOperable {
    fn get_type() -> i32 {
        CPPType::Expression as i32
    }
}

impl ExpressionOperable {
    pub fn new() -> Self {
        ExpressionOperable {
            identifier: Identifier::new(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> Self {
        ExpressionOperable {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }

    pub(crate) fn or<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Or,
            false,
        )
    }

    pub(crate) fn and<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::And,
            false,
        )
    }

    pub(crate) fn multiply_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Multiply,
            false,
        )
    }

    pub(crate) fn multiply_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Multiply, false)
    }

    pub(crate) fn multiply_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Multiply, false)
    }

    pub(crate) fn divide_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Divide,
            false,
        )
    }

    pub(crate) fn divide_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Divide, false)
    }

    pub(crate) fn divide_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Divide, false)
    }

    pub(crate) fn mod_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Modulo,
            false,
        )
    }

    pub(crate) fn mod_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Modulo, false)
    }

    pub(crate) fn mod_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Modulo, false)
    }

    pub(crate) fn add_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Plus,
            false,
        )
    }

    pub(crate) fn add_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Plus, false)
    }

    pub(crate) fn add_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Plus, false)
    }

    pub(crate) fn minus_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Minus,
            false,
        )
    }

    pub(crate) fn minus_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Minus, false)
    }

    pub(crate) fn minus_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Minus, false)
    }

    pub(crate) fn left_shift_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::LeftShift,
            false,
        )
    }

    pub(crate) fn left_shift_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::LeftShift, false)
    }

    pub(crate) fn right_shift_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::RightShift,
            false,
        )
    }

    pub(crate) fn right_shift_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand,
            BinaryOperatorType::RightShift,
            false,
        )
    }

    pub(crate) fn bit_and_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::BitwiseAnd,
            false,
        )
    }

    pub(crate) fn bit_and_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand,
            BinaryOperatorType::BitwiseAnd,
            false,
        )
    }

    pub(crate) fn bit_or_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::BitwiseOr,
            false,
        )
    }

    pub(crate) fn bit_or_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::BitwiseOr, false)
    }

    pub(crate) fn lt_expression_convertible<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Less,
            false,
        )
    }

    pub(crate) fn lt_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Less, false)
    }

    pub(crate) fn lt_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Less, false)
    }

    pub(crate) fn lt_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(left_cpp_type, operand, BinaryOperatorType::Less, false)
    }

    pub(crate) fn le_expression_convertible<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::LessOrEqual,
            false,
        )
    }

    pub(crate) fn le_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand,
            BinaryOperatorType::LessOrEqual,
            false,
        )
    }

    pub(crate) fn le_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(
            left_cpp_type,
            operand,
            BinaryOperatorType::LessOrEqual,
            false,
        )
    }

    pub(crate) fn le_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(
            left_cpp_type,
            operand,
            BinaryOperatorType::LessOrEqual,
            false,
        )
    }

    pub(crate) fn gt_expression_convertible<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Greater,
            false,
        )
    }

    pub(crate) fn gt_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Greater, false)
    }

    pub(crate) fn gt_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Greater, false)
    }

    pub(crate) fn gt_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(left_cpp_type, operand, BinaryOperatorType::Greater, false)
    }

    pub(crate) fn ge_expression_convertible<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::GreaterOrEqual,
            false,
        )
    }

    pub(crate) fn ge_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand,
            BinaryOperatorType::GreaterOrEqual,
            false,
        )
    }

    pub(crate) fn ge_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(
            left_cpp_type,
            operand,
            BinaryOperatorType::GreaterOrEqual,
            false,
        )
    }

    pub(crate) fn ge_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(
            left_cpp_type,
            operand,
            BinaryOperatorType::GreaterOrEqual,
            false,
        )
    }

    pub(crate) fn eq_expression_convertible<T>(&self, left_cpp_type: i32, operand: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Equal,
            false,
        )
    }

    pub(crate) fn eq_bool(&self, left_cpp_type: i32, operand: bool) -> Expression {
        self.binary_operate_with_bool(left_cpp_type, operand, BinaryOperatorType::Equal, false)
    }

    pub(crate) fn eq_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Equal, false)
    }

    pub(crate) fn eq_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Equal, false)
    }

    pub(crate) fn eq_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(left_cpp_type, operand, BinaryOperatorType::Equal, false)
    }

    pub(crate) fn not_eq_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::NotEqual,
            false,
        )
    }

    pub(crate) fn not_eq_bool(&self, left_cpp_type: i32, operand: bool) -> Expression {
        self.binary_operate_with_bool(left_cpp_type, operand, BinaryOperatorType::NotEqual, false)
    }

    pub(crate) fn not_eq_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::NotEqual, false)
    }

    pub(crate) fn not_eq_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::NotEqual, false)
    }

    pub(crate) fn not_eq_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(left_cpp_type, operand, BinaryOperatorType::NotEqual, false)
    }

    pub(crate) fn concat_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Concatenate,
            false,
        )
    }

    pub(crate) fn concat_long(&self, left_cpp_type: i32, operand: i64) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand,
            BinaryOperatorType::Concatenate,
            false,
        )
    }

    pub(crate) fn concat_double(&self, left_cpp_type: i32, operand: f64) -> Expression {
        self.binary_operate_with_double(
            left_cpp_type,
            operand,
            BinaryOperatorType::Concatenate,
            false,
        )
    }

    pub(crate) fn concat_string(&self, left_cpp_type: i32, operand: &str) -> Expression {
        self.binary_operate_text(
            left_cpp_type,
            operand,
            BinaryOperatorType::Concatenate,
            false,
        )
    }

    pub fn binary_operate_long(
        &self,
        left_cpp_type: i32,
        operand: i64,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                left_cpp_type,
                self.identifier.get_cpp_obj(),
                CPPType::Int as i32,
                operand,
                0.0,
                std::ptr::null(),
                binary_operator_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn binary_operate_text(
        &self,
        left_cpp_type: i32,
        operand: &str,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression {
        let c_operand = operand.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                left_cpp_type,
                self.identifier.get_cpp_obj(),
                CPPType::String as i32,
                0,
                0.0,
                c_operand.as_ptr(),
                binary_operator_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn binary_operate_with_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let operand_option = Option::Some(operand);
        let right_long = CppObject::get_by_cpp_object_convertible_trait(&operand_option);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&operand_option),
                right_long,
                0.0,
                std::ptr::null(),
                binary_operator_type as c_int,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn binary_operate_with_long(
        &self,
        left_cpp_type: i32,
        operand: i64,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as i32,
                operand,
                0.0,
                std::ptr::null(),
                binary_operator_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn binary_operate_with_double(
        &self,
        left_cpp_type: i32,
        operand: f64,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as i32,
                0,
                operand,
                std::ptr::null(),
                binary_operator_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn binary_operate_with_bool(
        &self,
        left_cpp_type: i32,
        operand: bool,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Bool as i32,
                operand as c_long,
                0.0,
                std::ptr::null(),
                binary_operator_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn create_expression(cpp_obj: *mut c_void) -> Expression {
        let mut expression = Expression::new();
        expression.set_cpp_obj(cpp_obj);
        expression
    }

    fn null_operate() -> Expression {
        // todo dengxudong
        Expression::new()
    }

    pub fn between_operate_with_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        begin: &T,
        end: &T,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let begin_option = Option::Some(begin);
        let end_option = Option::Some(end);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                std::ptr::null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_expr_long<T>(&self, left_cpp_type: i32, begin: &T, end: i64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let begin_option = Option::Some(begin);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                std::ptr::null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_expr_double<T>(&self, left_cpp_type: i32, begin: &T, end: f64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let begin_option = Option::Some(begin);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                std::ptr::null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_expr_string<T>(&self, left_cpp_type: i32, begin: &T, end: &str) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let begin_option = Option::Some(begin);
        let c_operand = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_operand.as_ptr(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_long_expr<T>(&self, left_cpp_type: i32, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let end_option = Option::Some(end);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_long_long(&self, left_cpp_type: i32, begin: i64, end: i64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_long_double(&self, left_cpp_type: i32, begin: i64, end: f64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_long_string(&self, left_cpp_type: i32, begin: i64, end: &str) -> Expression {
        let c_end = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_end.as_ptr(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_double_expr<T>(&self, left_cpp_type: i32, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let end_option = Option::Some(end);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin as c_double,
                null(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_double_long(&self, left_cpp_type: i32, begin: f64, end: i64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin,
                null(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_double_double(&self, left_cpp_type: i32, begin: f64, end: f64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin,
                null(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_double_string(&self, left_cpp_type: i32, begin: f64, end: &str) -> Expression {
        let c_end = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin,
                null(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_end.as_ptr(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_string_expr<T>(&self, left_cpp_type: i32, begin: &str, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let end_option = Option::Some(end);
        let c_begin = begin.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0 as c_double,
                c_begin.as_ptr(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_string_long(&self, left_cpp_type: i32, begin: &str, end: i64) -> Expression {
        let c_begin = begin.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_begin.as_ptr(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_string_double(&self, left_cpp_type: i32, begin: &str, end: f64) -> Expression {
        let c_begin = begin.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_begin.as_ptr(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                null(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn between_string_string(&self, left_cpp_type: i32, begin: &str, end: &str) -> Expression {
        let c_begin = begin.to_cstring();
        let c_end = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_begin.as_ptr(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_end.as_ptr(),
                false,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_expr_expr<T>(&self, left_cpp_type: i32, begin: &T, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let begin_option = Option::Some(begin);
        let end_option = Option::Some(end);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                std::ptr::null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_expr_long<T>(&self, left_cpp_type: i32, begin: &T, end: i64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let begin_option = Option::Some(begin);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                std::ptr::null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_expr_double<T>(&self, left_cpp_type: i32, begin: &T, end: f64) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let begin_option = Option::Some(begin);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                std::ptr::null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_expr_string<T>(&self, left_cpp_type: i32, begin: &T, end: &str) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let begin_cpp_obj: *mut c_void = begin.as_cpp_object();
        let begin_option = Option::Some(begin);
        let c_operand = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                Identifier::get_cpp_type_with_option(&begin_option),
                begin_cpp_obj,
                0.0,
                std::ptr::null(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_operand.as_ptr(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_long_expr<T>(&self, left_cpp_type: i32, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let end_option = Option::Some(end);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_long_long(&self, left_cpp_type: i32, begin: i64, end: i64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_long_double(&self, left_cpp_type: i32, begin: i64, end: f64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_long_string(&self, left_cpp_type: i32, begin: i64, end: &str) -> Expression {
        let c_end = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Int as c_int,
                begin as *mut c_void,
                0.0,
                null(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_end.as_ptr(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_double_expr<T>(&self, left_cpp_type: i32, begin: i64, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let end_option = Option::Some(end);
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin as c_double,
                null(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_double_long(&self, left_cpp_type: i32, begin: f64, end: i64) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin,
                null(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_double_double(
        &self,
        left_cpp_type: i32,
        begin: f64,
        end: f64,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin,
                null(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_double_string(
        &self,
        left_cpp_type: i32,
        begin: f64,
        end: &str,
    ) -> Expression {
        let c_end = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::Double as c_int,
                0 as *mut c_void,
                begin,
                null(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_end.as_ptr(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_string_expr<T>(&self, left_cpp_type: i32, begin: &str, end: &T) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        let end_cpp_obj: *mut c_void = end.as_cpp_object();
        let end_option = Option::Some(end);
        let c_begin = begin.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0 as c_double,
                c_begin.as_ptr(),
                Identifier::get_cpp_type_with_option(&end_option),
                end_cpp_obj,
                0.0,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_string_long(&self, left_cpp_type: i32, begin: &str, end: i64) -> Expression {
        let c_begin = begin.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_begin.as_ptr(),
                CPPType::Int as c_int,
                end as *mut c_void,
                0.0,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_string_double(
        &self,
        left_cpp_type: i32,
        begin: &str,
        end: f64,
    ) -> Expression {
        let c_begin = begin.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_begin.as_ptr(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                end,
                null(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn not_between_string_string(
        &self,
        left_cpp_type: i32,
        begin: &str,
        end: &str,
    ) -> Expression {
        let c_begin = begin.to_cstring();
        let c_end = end.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                left_cpp_type,
                CppObject::get(self),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_begin.as_ptr(),
                CPPType::String as c_int,
                0 as *mut c_void,
                0.0,
                c_end.as_ptr(),
                true,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn in_short(&self, left_cpp_type: i32, operands: Vec<i16>, is_not: bool) -> Expression {
        let val: Vec<i64> = operands.iter().map(|&i| i as i64).collect();
        self.in_long(left_cpp_type, val, is_not)
    }

    pub fn in_int(&self, left_cpp_type: i32, operands: Vec<i32>, is_not: bool) -> Expression {
        let val: Vec<i64> = operands.iter().map(|&i| i as i64).collect();
        self.in_long(left_cpp_type, val, is_not)
    }

    pub fn in_float(&self, left_cpp_type: i32, operands: Vec<f32>, is_not: bool) -> Expression {
        let val: Vec<f64> = operands.iter().map(|&i| i as f64).collect();
        self.in_double(left_cpp_type, val, is_not)
    }

    pub fn in_double(&self, left_cpp_type: i32, operands: Vec<f64>, is_not: bool) -> Expression {
        self.in_double_operate(left_cpp_type, operands, is_not)
    }

    pub fn in_long(&self, left_cpp_type: i32, operands: Vec<i64>, is_not: bool) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_inOperate(
                left_cpp_type as c_int,
                CppObject::get(self),
                CPPType::Int as c_int,
                operands.as_ptr(),
                null(),
                null(),
                operands.len() as c_int,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn in_long_with_cpp_type(
        &self,
        left_cpp_type: i32,
        cpp_type: i32,
        operands: Vec<i64>,
        is_not: bool,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_inOperate(
                left_cpp_type as c_int,
                CppObject::get(self),
                cpp_type as c_int,
                operands.as_ptr(),
                null(),
                null(),
                operands.len() as c_int,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn in_double_operate(
        &self,
        left_cpp_type: i32,
        operands: Vec<f64>,
        is_not: bool,
    ) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_inOperate(
                left_cpp_type as c_int,
                CppObject::get(self),
                CPPType::Double as c_int,
                null(),
                operands.as_ptr(),
                null(),
                operands.len() as c_int,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn in_string(&self, left_cpp_type: i32, operands: Vec<&str>, is_not: bool) -> Expression {
        let mut c_string_array: Vec<*const c_char> = Vec::new();
        for x in operands {
            let c_string = CString::new(x).expect("Failed to create CString");
            c_string_array.push(c_string.into_raw());
        }
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_inOperate(
                left_cpp_type as c_int,
                CppObject::get(self),
                CPPType::String as c_int,
                null(),
                null(),
                c_string_array.as_ptr(),
                c_string_array.len() as c_int,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn in_object<T>(
        &self,
        operands: Option<Vec<T>>,
        left_cpp_type: i32,
        is_not: bool,
    ) -> Expression {
        //todo dengxudong
        Expression::new()
        // match operands {
        //     None => {
        //         self.in_long(left_cpp_type, Vec::new(), is_not)
        //     }
        //     Some(val) => {
        //         let first = val.first().unwrap();
        //         let data_type: ObjectType = MultiTypeArray::get_object_type(Box::new(first));
        //         match data_type {
        //             ObjectType::Identifier => {
        //                 // let mut vector: Vec<i64> = Vec::new();
        //                 // for x in val {
        //                 //     let few = x as Identifier.get_cpp_obj();
        //                 //     vector.push(few as i64);
        //                 // }
        //                 //
        //                 // let cpp_type = crate::winq::identifier::Identifier::get_cpp_type(first);
        //                 // self.in_long_with_cpp_type(left_cpp_type, cpp_type, vector, is_not)
        //                 Expression::new()
        //             }
        //             ObjectType::Value => {
        //                 Expression::new()
        //             }
        //             ObjectType::String => {
        //                 // if val.is_empty() {
        //                 //     self.in_string(left_cpp_type, Vec::new(), is_not)
        //                 // } else {
        //                 //     let mut string_vec:Vec<&str> = Vec::new();
        //                 //     for x in val {
        //                 //         string_vec.push(x);
        //                 //     }
        //                 //     self.in_string(left_cpp_type, string_vec, is_not)
        //                 // }
        //                 Expression::new()
        //             }
        //             ObjectType::Float => {
        //                 Expression::new()
        //             }
        //             ObjectType::Bool | ObjectType::Char | ObjectType::Byte | ObjectType::Short | ObjectType::Int
        //             | ObjectType::Long | ObjectType::Double => {
        //                 Expression::new()
        //             }
        //             ObjectType::Null | ObjectType::Unknown => {
        //                 Expression::new()
        //             }
        //         }
        //     }
        // }
    }

    pub fn collate(&self, left_cpp_type: i32, collation: &str) -> Expression {
        let c_string = collation.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_collateOperate(
                left_cpp_type as c_int,
                CppObject::get(self),
                c_string.as_ptr(),
            )
        };
        Self::create_expression(cpp_obj)
    }

    pub fn substr_int(&self, left_cpp_type: i32, start: i32, length: i32) -> Expression {
        Self::create_expression(Expression::function("SUBSTR"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
            .argument_int(start)
            .argument_int(length)
    }

    pub fn substr_long(&self, left_cpp_type: i32, start: i64, length: i64) -> Expression {
        Self::create_expression(Expression::function("SUBSTR"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
            .argument_long(start)
            .argument_long(length)
    }

    pub fn like(&self, left_cpp_type: i32, content: &str, is_not: bool) -> Expression {
        self.binary_operate_text(left_cpp_type, content, BinaryOperatorType::Like, is_not)
    }

    pub fn glob(&self, left_cpp_type: i32, content: &str, is_not: bool) -> Expression {
        self.binary_operate_text(left_cpp_type, content, BinaryOperatorType::GLOB, is_not)
    }

    pub fn match_string(&self, left_cpp_type: i32, content: &str, is_not: bool) -> Expression {
        self.binary_operate_text(left_cpp_type, content, BinaryOperatorType::Match, is_not)
    }

    pub fn regexp(&self, left_cpp_type: i32, content: &str, is_not: bool) -> Expression {
        self.binary_operate_text(left_cpp_type, content, BinaryOperatorType::RegExp, is_not)
    }

    pub fn is_bool(&self, left_cpp_type: i32, operand: bool, is_not: bool) -> Expression {
        self.binary_operate_with_bool(left_cpp_type, operand, BinaryOperatorType::Is, is_not)
    }

    pub fn is_byte(&self, left_cpp_type: i32, operand: u8, is_not: bool) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand as i64,
            BinaryOperatorType::Is,
            is_not,
        )
    }

    pub fn is_short(&self, left_cpp_type: i32, operand: i16, is_not: bool) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand as i64,
            BinaryOperatorType::Is,
            is_not,
        )
    }

    pub fn is_i32(&self, left_cpp_type: i32, operand: i32, is_not: bool) -> Expression {
        self.binary_operate_with_long(
            left_cpp_type,
            operand as i64,
            BinaryOperatorType::Is,
            is_not,
        )
    }

    pub fn is_long(&self, left_cpp_type: i32, operand: i64, is_not: bool) -> Expression {
        self.binary_operate_with_long(left_cpp_type, operand, BinaryOperatorType::Is, is_not)
    }

    pub fn is_float(&self, left_cpp_type: i32, operand: f32, is_not: bool) -> Expression {
        self.binary_operate_with_double(
            left_cpp_type,
            operand as f64,
            BinaryOperatorType::Is,
            is_not,
        )
    }

    pub fn is_double(&self, left_cpp_type: i32, operand: f64, is_not: bool) -> Expression {
        self.binary_operate_with_double(left_cpp_type, operand, BinaryOperatorType::Is, is_not)
    }

    pub fn is_string(&self, left_cpp_type: i32, operand: &str, is_not: bool) -> Expression {
        self.binary_operate_text(left_cpp_type, operand, BinaryOperatorType::Is, is_not)
    }

    pub fn is_expression_convertible<T>(
        &self,
        left_cpp_type: i32,
        operand: &T,
        is_not: bool,
    ) -> Expression
    where
        T: IdentifierStaticTrait + IdentifierConvertibleTrait + ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(
            left_cpp_type,
            operand,
            BinaryOperatorType::Is,
            is_not,
        )
    }

    pub fn avg(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("AVG"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn count(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("COUNT"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn group_concat(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("GROUP_CONCAT"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn group_concat_string(&self, left_cpp_type: i32, sperator: &str) -> Expression {
        Self::create_expression(Expression::function("GROUP_CONCAT"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
            .argument_string(sperator)
    }

    pub fn max(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("MAX"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn min(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("MIN"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn sum(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("SUM"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn total(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("TOTAL"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn abs(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("ABS"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn hex(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("HEX"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn length(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("LENGTH"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn lower(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("LOWER"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn upper(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("UPPER"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn round(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("ROUND"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn match_info(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("matchInfo"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn offsets(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("offsets"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn snippet(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("snippet"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn bm25(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("bm25"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn highlight(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("highlight"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }

    pub fn substring_match_info(&self, left_cpp_type: i32) -> Expression {
        Self::create_expression(Expression::function("substring_match_info"))
            .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    }
}

pub enum BinaryOperatorType {
    Concatenate = 1,
    Multiply = 2,
    Divide = 3,
    Modulo = 4,
    Plus = 5,
    Minus = 6,
    LeftShift = 7,
    RightShift = 8,
    BitwiseAnd = 9,
    BitwiseOr = 10,
    Less = 11,
    LessOrEqual = 12,
    Greater = 13,
    GreaterOrEqual = 14,
    Equal = 15,
    NotEqual = 16,
    Is = 17,
    And = 18,
    Or = 19,
    Like = 20,
    GLOB = 21,
    RegExp = 22,
    Match = 23,
}
