use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_double, c_int, c_long, c_void};

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

    pub fn or<T>(&mut self, operand: T) -> Self
    where
        T: ExpressionConvertibleTrait,
    {
        self.binary_operate_with_expression_convertible(operand, BinaryOperatorType::Or, false)
    }

    pub fn eq_long(&self, operand: i64) -> Self {
        self.binary_operate_long(operand, BinaryOperatorType::Equal, false)
    }

    pub fn eq_text(&self, operand: &str) -> Self {
        self.binary_operate_text(operand, BinaryOperatorType::Equal, false)
    }

    fn binary_operate_long(
        &self,
        operand: i64,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                Identifier::get_cpp_type(self),
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
        operand: &str,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Self {
        let c_operand = operand.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                Identifier::get_cpp_type(self),
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
        &mut self,
        operand: T,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Self
    where
        T: ExpressionConvertibleTrait,
    {
        let cpp_obj = unsafe {
            let operand_option = Option::Some(operand);
            WCDBRustExpressionOperable_binaryOperate(
                Identifier::get_cpp_type(self),
                CppObject::get(self),
                Identifier::get_cpp_type_by_identifier_convertible(&operand_option),
                CppObject::get_by_cpp_object_convertible_trait(&operand_option),
                0.0,
                std::ptr::null(),
                binary_operator_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn binary_operate_with_long(
        &self,
        operand: i64,
        binary_operator_type: BinaryOperatorType,
        is_not: bool,
    ) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                Identifier::get_cpp_type(self),
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

    fn create_expression(cpp_obj: *mut c_void) -> Self {
        ExpressionOperable::new_with_obj(cpp_obj)
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
