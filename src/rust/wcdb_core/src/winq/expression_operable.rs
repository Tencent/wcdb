use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_long, c_void};
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
