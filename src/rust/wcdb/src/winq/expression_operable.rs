use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_void};

extern "C" {
    fn WCDBRustExpressionOperable_nullOperate(
        operand_type: c_int,
        operand: *mut c_void,
        is_not: bool,
    ) -> *mut c_void;

    fn WCDBRustExpressionOperable_binaryOperate(
        left_type: c_int,
        left: *mut c_void,
        right_type: c_int,
        right_long: i64,
        right_double: c_double,
        right_string: *const c_char,
        operator_type: c_int,
        is_not: bool,
    ) -> *mut c_void;

    fn WCDBRustExpressionOperable_betweenOperate(
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

    fn WCDBRustExpressionOperable_in(
        operand_type: c_int,
        operand: *mut c_void,
        cpp_type: c_int,
        long_array: *const i64,
        double_array: *const c_double,
        string_array: *const *const c_char,
        array_length: c_int,
        is_not: bool,
    ) -> *mut c_void;

    fn WCDBRustExpressionOperable_inTable(
        cpp_type: c_int,
        operand: *mut c_void,
        table: *const c_char,
        is_not: bool,
    ) -> *mut c_void;

    fn WCDBRustExpressionOperable_collate(
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

impl CppObjectConvertibleTrait for ExpressionOperable {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierConvertibleTrait for ExpressionOperable {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl IdentifierTrait for ExpressionOperable {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl ExpressionConvertibleTrait for ExpressionOperable {}

pub trait OperateParam {
    /// 对应 C++ 入参 type, long, double, string
    fn get_params(&self) -> (CPPType, i64, f64, *const c_char);
}

impl<T: ExpressionConvertibleTrait> OperateParam for &T {
    fn get_params(&self) -> (CPPType, i64, f64, *const c_char) {
        (
            self.as_identifier().get_type(),
            self.as_cpp_object().get_cpp_obj() as i64,
            0.0,
            std::ptr::null_mut(),
        )
    }
}

impl OperateParam for bool {
    fn get_params(&self) -> (CPPType, i64, f64, *const c_char) {
        (
            CPPType::Int,
            if *self { 1 } else { 0 } as i64,
            0.0,
            std::ptr::null(),
        )
    }
}

macro_rules! impl_binary_operate_param_for_int {
    ($($t:ty),*) => {
        $(
            impl OperateParam for $t {
                fn get_params(&self) -> (CPPType, i64, f64, *const c_char) {
                    (CPPType::Int, *self as i64, 0.0, std::ptr::null())
                }
            }
        )*
    };
}

impl_binary_operate_param_for_int!(i8, i16, i32, i64);

macro_rules! impl_binary_operate_param_for_float {
    ($($t:ty),*) => {
        $(
            impl OperateParam for $t {
                fn get_params(&self) -> (CPPType, i64, f64, *const c_char) {
                    (CPPType::Double, 0, *self as f64, std::ptr::null())
                }
            }
        )*
    };
}

impl_binary_operate_param_for_float!(f32, f64);

impl OperateParam for &str {
    fn get_params(&self) -> (CPPType, i64, f64, *const c_char) {
        (
            CPPType::String,
            0,
            0.0,
            self.to_cstring().as_ptr(),
        )
    }
}

pub trait ExpressionOperableTrait {
    fn is_null(&self) -> Expression;

    fn not_null(&self) -> Expression;

    fn or<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression;

    fn and<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression;

    fn multiply<T: OperateParam>(&self, operand: T) -> Expression;

    fn divide<T: OperateParam>(&self, operand: T) -> Expression;

    fn r#mod<T: OperateParam>(&self, operand: T) -> Expression;

    fn add<T: OperateParam>(&self, operand: T) -> Expression;

    fn minus<T: OperateParam>(&self, operand: T) -> Expression;

    fn left_shift<T: OperateParam>(&self, operand: T) -> Expression;

    fn right_shift<T: OperateParam>(&self, operand: T) -> Expression;

    fn bit_and<T: OperateParam>(&self, operand: T) -> Expression;

    fn bit_or<T: OperateParam>(&self, operand: T) -> Expression;

    fn lt<T: OperateParam>(&self, operand: T) -> Expression;

    fn le<T: OperateParam>(&self, operand: T) -> Expression;

    fn gt<T: OperateParam>(&self, operand: T) -> Expression;

    fn ge<T: OperateParam>(&self, operand: T) -> Expression;

    fn eq<T: OperateParam>(&self, operand: T) -> Expression;

    fn not_eq<T: OperateParam>(&self, operand: T) -> Expression;

    fn concat<T: OperateParam>(&self, operand: T) -> Expression;

    fn between<T: OperateParam>(&self, begin: T, end: T) -> Expression;

    fn not_between<T: OperateParam>(&self, begin: T, end: T) -> Expression;

    fn r#in<T: OperateParam>(&self, operands: &[T]) -> Expression;

    fn not_in<T: OperateParam>(&self, operands: &[T]) -> Expression;

    fn in_table(&self, table: &str) -> Expression;

    fn not_in_table(&self, table: &str) -> Expression;

    fn collate(&self, collation: &str) -> Expression;

    fn like(&self, content: &str) -> Expression;

    fn not_like(&self, content: &str) -> Expression;

    fn glob(&self, content: &str) -> Expression;

    fn not_glob(&self, content: &str) -> Expression;

    fn r#match(&self, content: &str) -> Expression;

    fn not_match(&self, content: &str) -> Expression;

    fn regexp(&self, content: &str) -> Expression;

    fn not_regexp(&self, content: &str) -> Expression;

    fn is(&self, operand: bool) -> Expression;

    fn is_not(&self, operand: bool) -> Expression;

    fn avg(&self) -> Expression;

    fn count(&self) -> Expression;

    fn group_concat(&self) -> Expression;

    fn group_concat_string(&self, separator: &str) -> Expression;

    fn max(&self) -> Expression;

    fn min(&self) -> Expression;

    fn sum(&self) -> Expression;

    fn total(&self) -> Expression;

    fn abs(&self) -> Expression;

    fn hex(&self) -> Expression;

    fn length(&self) -> Expression;

    fn lower(&self) -> Expression;

    fn upper(&self) -> Expression;

    fn round(&self) -> Expression;

    fn match_info(&self) -> Expression;

    fn offsets(&self) -> Expression;

    fn snippet(&self) -> Expression;

    fn bm25(&self) -> Expression;

    fn highlight(&self) -> Expression;

    fn substring_match_info(&self) -> Expression;
}

impl ExpressionOperableTrait for ExpressionOperable {
    fn is_null(&self) -> Expression {
        self.null_operate(false)
    }

    fn not_null(&self) -> Expression {
        self.null_operate(true)
    }

    fn or<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression {
        self.binary_operate(&operand, BinaryOperatorType::Or, false)
    }

    fn and<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression {
        self.binary_operate(&operand, BinaryOperatorType::And, false)
    }

    fn multiply<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Multiply, false)
    }

    fn divide<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Divide, false)
    }

    fn r#mod<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Modulo, false)
    }

    fn add<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Plus, false)
    }

    fn minus<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Minus, false)
    }

    fn left_shift<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::LeftShift, false)
    }

    fn right_shift<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::RightShift, false)
    }

    fn bit_and<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::BitwiseAnd, false)
    }

    fn bit_or<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::BitwiseOr, false)
    }

    fn lt<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Less, false)
    }

    fn le<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::LessOrEqual, false)
    }

    fn gt<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Greater, false)
    }

    fn ge<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::GreaterOrEqual, false)
    }

    fn eq<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Equal, false)
    }

    fn not_eq<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::NotEqual, false)
    }

    fn concat<T: OperateParam>(&self, operand: T) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Concatenate, false)
    }

    fn between<T: OperateParam>(&self, begin: T, end: T) -> Expression {
        self.between_operate(begin, end, false)
    }

    fn not_between<T: OperateParam>(&self, begin: T, end: T) -> Expression {
        self.between_operate(begin, end, true)
    }

    fn r#in<T: OperateParam>(&self, operands: &[T]) -> Expression {
        todo!("qixinbing")
        // self.r#in(operands)
    }

    fn not_in<T: OperateParam>(&self, operands: &[T]) -> Expression {
        todo!("qixinbing")
        // self.not_in(operands)
    }

    fn in_table(&self, table: &str) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_inTable(
                self.get_type() as i32,
                self.get_cpp_obj(),
                table.to_cstring().as_ptr(),
                true,
            )
        };
        Expression::new(Some(cpp_obj))
    }

    fn not_in_table(&self, table: &str) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_inTable(
                self.get_type() as i32,
                self.get_cpp_obj(),
                table.to_cstring().as_ptr(),
                false,
            )
        };
        Expression::new(Some(cpp_obj))
    }

    fn collate(&self, collation: &str) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_collate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                collation.to_cstring().as_ptr(),
            )
        };
        Expression::new(Some(cpp_obj))
    }

    // pub fn substr_int(&self, start: i32, length: i32) -> Expression {
    //     Expression::function("SUBSTR")
    //         .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    //         .argument_int(start)
    //         .argument_int(length)
    // }

    // pub fn substr_long(&self, start: i64, length: i64) -> Expression {
    //     Expression::function("SUBSTR")
    //         .argument_expression_convertible_trait(left_cpp_type, CppObject::get(self))
    //         .argument_long(start)
    //         .argument_long(length)
    // }

    fn like(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::Like, false)
    }

    fn not_like(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::Like, true)
    }

    fn glob(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::GLOB, false)
    }

    fn not_glob(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::GLOB, true)
    }

    fn r#match(&self, content: &str) -> Expression {
        self.binary_operate(
            content,
            BinaryOperatorType::Match,
            false,
        )
    }

    fn not_match(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::Match, true)
    }

    fn regexp(&self, content: &str) -> Expression {
        self.binary_operate(
            content,
            BinaryOperatorType::RegExp,
            false,
        )
    }

    fn not_regexp(&self, content: &str) -> Expression {
        self.binary_operate(
            content,
            BinaryOperatorType::RegExp,
            true,
        )
    }

    fn is(&self, operand: bool) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Is, false)
    }

    fn is_not(&self, operand: bool) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Is, true)
    }

    fn avg(&self) -> Expression {
        Expression::function("AVG").argument(self)
    }

    fn count(&self) -> Expression {
        Expression::function("COUNT").argument(self)
    }

    fn group_concat(&self) -> Expression {
        Expression::function("GROUP_CONCAT").argument(self)
    }

    fn group_concat_string(&self, separator: &str) -> Expression {
        Expression::function("GROUP_CONCAT")
            .argument(self)
            .argument(separator)
    }

    fn max(&self) -> Expression {
        Expression::function("MAX").argument(self)
    }

    fn min(&self) -> Expression {
        Expression::function("MIN").argument(self)
    }

    fn sum(&self) -> Expression {
        Expression::function("SUM").argument(self)
    }

    fn total(&self) -> Expression {
        Expression::function("TOTAL").argument(self)
    }

    fn abs(&self) -> Expression {
        Expression::function("ABS").argument(self)
    }

    fn hex(&self) -> Expression {
        Expression::function("HEX").argument(self)
    }

    fn length(&self) -> Expression {
        Expression::function("LENGTH").argument(self)
    }

    fn lower(&self) -> Expression {
        Expression::function("LOWER").argument(self)
    }

    fn upper(&self) -> Expression {
        Expression::function("UPPER").argument(self)
    }

    fn round(&self) -> Expression {
        Expression::function("ROUND").argument(self)
    }

    fn match_info(&self) -> Expression {
        Expression::function("matchInfo").argument(self)
    }

    fn offsets(&self) -> Expression {
        Expression::function("offsets").argument(self)
    }

    fn snippet(&self) -> Expression {
        Expression::function("snippet").argument(self)
    }

    fn bm25(&self) -> Expression {
        Expression::function("bm25").argument(self)
    }

    fn highlight(&self) -> Expression {
        Expression::function("highlight").argument(self)
    }

    fn substring_match_info(&self) -> Expression {
        Expression::function("substring_match_info").argument(self)
    }
}

impl ExpressionOperable {
    pub(crate) fn new(cpp_type: CPPType, cpp_obj_opt: Option<*mut c_void>) -> Self {
        ExpressionOperable {
            identifier: Identifier::new(cpp_type, cpp_obj_opt),
        }
    }

    fn null_operate(&self, is_not: bool) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_nullOperate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                is_not,
            )
        };
        Expression::new(Some(cpp_obj))
    }

    fn binary_operate<T: OperateParam>(
        &self,
        operand: T,
        operand_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression {
        let (right_type, right_long, right_double, right_cpp_obj) = operand.get_params();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                right_type as i32,
                right_long,
                right_double,
                right_cpp_obj as *const c_char,
                operand_type as i32,
                is_not,
            )
        };
        Expression::new(Some(cpp_obj))
    }

    fn between_operate<T: OperateParam>(&self, begin: T, end: T, is_not: bool) -> Expression {
        let (begin_type, begin_long, begin_double, begin_cpp_obj) = begin.get_params();
        let (end_type, end_long, end_double, end_cpp_obj) = end.get_params();
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                begin_type as i32,
                begin_long as usize as *mut c_void,
                begin_double,
                begin_cpp_obj as *const c_char,
                end_type as i32,
                end_long as usize as *mut c_void,
                end_double,
                end_cpp_obj as *const c_char,
                is_not,
            )
        };
        Expression::new(Some(cpp_obj))
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