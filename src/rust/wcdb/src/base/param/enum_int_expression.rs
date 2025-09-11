use crate::winq::expression_convertible::ExpressionConvertibleTrait;

/// 支持 i8, i16, i32, i64, Option<&dyn ExpressionConvertibleTrait>
pub enum IntExpression<'a> {
    Int(i64),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
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
