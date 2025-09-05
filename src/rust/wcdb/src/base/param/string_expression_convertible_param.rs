use crate::winq::expression_convertible::ExpressionConvertibleTrait;

/// 支持 String, &str, &dyn ExpressionConvertibleTrait
pub enum StringExpressionConvertibleParam<'a> {
    String(String),
    ExpressionConvertible(&'a dyn ExpressionConvertibleTrait),
}

impl<'a> From<String> for StringExpressionConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringExpressionConvertibleParam::String(value)
    }
}

impl<'a> From<&str> for StringExpressionConvertibleParam<'a> {
    fn from(value: &str) -> Self {
        StringExpressionConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: ExpressionConvertibleTrait> From<&'a T> for StringExpressionConvertibleParam<'a> {
    fn from(value: &'a T) -> Self {
        StringExpressionConvertibleParam::ExpressionConvertible(value)
    }
}
