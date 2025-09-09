use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::expression_convertible_param::ExpressionConvertibleParam;
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

    fn WCDBRustExpressionOperable_collateOperate(
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

pub trait ExpressionOperableTrait {
    fn is_null(&self) -> Expression;

    fn not_null(&self) -> Expression;

    fn or<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression;

    fn and<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression;

    fn multiply<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn divide<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn r#mod<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn add<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn minus<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn left_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn right_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn bit_and<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn bit_or<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn lt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn le<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn gt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn ge<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn not_eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn concat<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn between<'a, T, V>(&self, begin: T, end: V) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
        V: Into<ExpressionConvertibleParam<'a>>;

    fn not_between<'a, T, V>(&self, begin: T, end: V) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
        V: Into<ExpressionConvertibleParam<'a>>;

    fn r#in<'a, I, S>(&self, operands: I) -> Expression
    where
        I: IntoIterator<Item = S>,
        S: Into<ExpressionConvertibleParam<'a>>;

    fn not_in<'a, I, S>(&self, operands: I) -> Expression
    where
        I: IntoIterator<Item = S>,
        S: Into<ExpressionConvertibleParam<'a>>;

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

    fn is<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

    fn is_not<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>;

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

    fn or<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::Or, false)
    }

    fn and<'a>(&self, operand: Option<&'a dyn ExpressionConvertibleTrait>) -> Expression {
        self.binary_operate(operand, BinaryOperatorType::And, false)
    }

    fn multiply<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Multiply, false)
    }

    fn divide<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Divide, false)
    }

    fn r#mod<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Modulo, false)
    }

    fn add<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Plus, false)
    }

    fn minus<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Minus, false)
    }

    fn left_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::LeftShift, false)
    }

    fn right_shift<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::RightShift, false)
    }

    fn bit_and<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::BitwiseAnd, false)
    }

    fn bit_or<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::BitwiseOr, false)
    }

    fn lt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Less, false)
    }

    fn le<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::LessOrEqual, false)
    }

    fn gt<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Greater, false)
    }

    fn ge<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::GreaterOrEqual, false)
    }

    fn eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Equal, false)
    }

    fn not_eq<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::NotEqual, false)
    }

    fn concat<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Concatenate, false)
    }

    fn between<'a, T, V>(&self, begin: T, end: V) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
        V: Into<ExpressionConvertibleParam<'a>>,
    {
        self.between_operate(begin, end, false)
    }

    fn not_between<'a, T, V>(&self, begin: T, end: V) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
        V: Into<ExpressionConvertibleParam<'a>>,
    {
        self.between_operate(begin, end, true)
    }

    fn r#in<'a, I, S>(&self, operands: I) -> Expression
    where
        I: IntoIterator<Item = S>,
        S: Into<ExpressionConvertibleParam<'a>>,
    {
        todo!("qixinbing")
        // self.r#in(operands)
    }

    fn not_in<'a, I, S>(&self, operands: I) -> Expression
    where
        I: IntoIterator<Item = S>,
        S: Into<ExpressionConvertibleParam<'a>>,
    {
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
        Self::create_expression(cpp_obj)
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
        Self::create_expression(cpp_obj)
    }

    fn collate(&self, collation: &str) -> Expression {
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_collateOperate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                collation.to_cstring().as_ptr(),
            )
        };
        Self::create_expression(cpp_obj)
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
        self.binary_operate(content, BinaryOperatorType::Match, false)
    }

    fn not_match(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::Match, true)
    }

    fn regexp(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::RegExp, false)
    }

    fn not_regexp(&self, content: &str) -> Expression {
        self.binary_operate(content, BinaryOperatorType::RegExp, true)
    }

    fn is<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Is, false)
    }

    fn is_not<'a, T>(&self, operand: T) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        self.binary_operate(operand, BinaryOperatorType::Is, true)
    }

    fn avg(&self) -> Expression {
        let exp = Expression::function("AVG");
        exp.argument(Some(self));
        exp
    }

    fn count(&self) -> Expression {
        let exp = Expression::function("COUNT");
        exp.argument(Some(self));
        exp
    }

    fn group_concat(&self) -> Expression {
        let exp = Expression::function("GROUP_CONCAT");
        exp.argument(Some(self));
        exp
    }

    fn group_concat_string(&self, separator: &str) -> Expression {
        let exp = Expression::function("GROUP_CONCAT");
        exp.argument(Some(self)).argument(separator);
        exp
    }

    fn max(&self) -> Expression {
        let exp = Expression::function("MAX");
        exp.argument(Some(self));
        exp
    }

    fn min(&self) -> Expression {
        let exp = Expression::function("MIN");
        exp.argument(Some(self));
        exp
    }

    fn sum(&self) -> Expression {
        let exp = Expression::function("SUM");
        exp.argument(Some(self));
        exp
    }

    fn total(&self) -> Expression {
        let exp = Expression::function("TOTAL");
        exp.argument(Some(self));
        exp
    }

    fn abs(&self) -> Expression {
        let exp = Expression::function("ABS");
        exp.argument(Some(self));
        exp
    }

    fn hex(&self) -> Expression {
        let exp = Expression::function("HEX");
        exp.argument(Some(self));
        exp
    }

    fn length(&self) -> Expression {
        let exp = Expression::function("LENGTH");
        exp.argument(Some(self));
        exp
    }

    fn lower(&self) -> Expression {
        let exp = Expression::function("LOWER");
        exp.argument(Some(self));
        exp
    }

    fn upper(&self) -> Expression {
        let exp = Expression::function("UPPER");
        exp.argument(Some(self));
        exp
    }

    fn round(&self) -> Expression {
        let exp = Expression::function("ROUND");
        exp.argument(Some(self));
        exp
    }

    fn match_info(&self) -> Expression {
        let exp = Expression::function("matchInfo");
        exp.argument(Some(self));
        exp
    }

    fn offsets(&self) -> Expression {
        let exp = Expression::function("offsets");
        exp.argument(Some(self));
        exp
    }

    fn snippet(&self) -> Expression {
        let exp = Expression::function("snippet");
        exp.argument(Some(self));
        exp
    }

    fn bm25(&self) -> Expression {
        let exp = Expression::function("bm25");
        exp.argument(Some(self));
        exp
    }

    fn highlight(&self) -> Expression {
        let exp = Expression::function("highlight");
        exp.argument(Some(self));
        exp
    }

    fn substring_match_info(&self) -> Expression {
        let exp = Expression::function("substring_match_info");
        exp.argument(Some(self));
        exp
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
        Self::create_expression(cpp_obj)
    }

    pub(crate) fn create_expression(cpp_obj: *mut c_void) -> Expression {
        let mut expression = Expression::new_empty();
        expression.set_cpp_obj(cpp_obj);
        expression
    }

    fn binary_operate<'a, T>(
        &self,
        operand: T,
        operand_type: BinaryOperatorType,
        is_not: bool,
    ) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        let (right_type, right_long, right_double, right_cstr_opt) = operand.into().get_params();
        let right_string_ptr = match right_cstr_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_binaryOperate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                right_type as i32,
                right_long,
                right_double,
                right_string_ptr,
                operand_type as i32,
                is_not,
            )
        };
        Self::create_expression(cpp_obj)
    }

    fn between_operate<'a, T, V>(&self, begin: T, end: V, is_not: bool) -> Expression
    where
        T: Into<ExpressionConvertibleParam<'a>>,
        V: Into<ExpressionConvertibleParam<'a>>,
    {
        let (begin_type, begin_long, begin_double, begin_cstr_opt) = begin.into().get_params();
        let (end_type, end_long, end_double, end_cstr_opt) = end.into().get_params();
        let begin_string_ptr = match begin_cstr_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        let end_string_ptr = match end_cstr_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        let cpp_obj = unsafe {
            WCDBRustExpressionOperable_betweenOperate(
                self.get_type() as i32,
                self.get_cpp_obj(),
                begin_type as i32,
                begin_long as usize as *mut c_void,
                begin_double,
                begin_string_ptr,
                end_type as i32,
                end_long as usize as *mut c_void,
                end_double,
                end_string_ptr,
                is_not,
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
