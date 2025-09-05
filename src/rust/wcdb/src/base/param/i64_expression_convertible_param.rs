use crate::winq::expression_convertible::ExpressionConvertibleTrait;

/// 支持 i64, Option<&dyn ExpressionConvertibleTrait>
pub enum I64ExpressionConvertibleParam<'a> {
    I64(i64),

    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl<'a> From<i64> for I64ExpressionConvertibleParam<'a> {
    fn from(value: i64) -> Self {
        I64ExpressionConvertibleParam::I64(value)
    }
}
impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for I64ExpressionConvertibleParam<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        I64ExpressionConvertibleParam::ExpressionConvertible(value)
    }
}
