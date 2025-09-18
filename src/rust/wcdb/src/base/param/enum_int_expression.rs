use crate::base::cpp_object::CppObject;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::c_longlong;

/// 支持 i8, i16, i32, i64, Option<&dyn ExpressionConvertibleTrait>
pub enum IntExpression<'a> {
    Int(i64),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl IntExpression<'_> {
    pub(crate) fn get_params(self) -> (CPPType, c_longlong) {
        match self {
            IntExpression::Int(value) => (CPPType::Int, value),
            IntExpression::ExpressionConvertible(value_opt) => match value_opt {
                None => (CPPType::Null, 0),
                Some(value) => (
                    Identifier::get_cpp_type(value),
                    CppObject::get(value) as c_longlong,
                ),
            },
        }
    }
}

/// 宏定义：为所有整数类型实现 From trait
macro_rules! impl_from_int_types {
    ($($int_type:ty),* $(,)?) => {
        $(
            impl<'a> From<$int_type> for IntExpression<'a> {
                fn from(value: $int_type) -> Self {
                    IntExpression::Int(value as i64)
                }
            }
        )*
    };
}

/// 使用宏为所有整数类型实现 From trait
impl_from_int_types!(i8, i16, i32, i64);

impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for IntExpression<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        IntExpression::ExpressionConvertible(value)
    }
}

impl<'a> From<&'a dyn ExpressionConvertibleTrait> for IntExpression<'a> {
    fn from(value: &'a dyn ExpressionConvertibleTrait) -> Self {
        IntExpression::ExpressionConvertible(Some(value))
    }
}
