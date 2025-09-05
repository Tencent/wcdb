use crate::winq::identifier::CPPType;
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;

pub trait ExpressionConvertibleTrait: IdentifierConvertibleTrait {}

pub enum ExpressionConvertibleParam<'a> {
    Int(CPPType, i64),
    Double(CPPType, f64),
    String(String),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl<'a> From<bool> for ExpressionConvertibleParam<'a> {
    fn from(value: bool) -> Self {
        let value = if value { 1 } else { 0 };
        ExpressionConvertibleParam::Int(CPPType::Bool, value)
    }
}

impl<'a> From<i8> for ExpressionConvertibleParam<'a> {
    fn from(value: i8) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value as i64)
    }
}

impl<'a> From<i16> for ExpressionConvertibleParam<'a> {
    fn from(value: i16) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value as i64)
    }
}

impl<'a> From<i32> for ExpressionConvertibleParam<'a> {
    fn from(value: i32) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value as i64)
    }
}

impl<'a> From<i64> for ExpressionConvertibleParam<'a> {
    fn from(value: i64) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value)
    }
}

impl<'a> From<f32> for ExpressionConvertibleParam<'a> {
    fn from(value: f32) -> Self {
        ExpressionConvertibleParam::Double(CPPType::Double, value as f64)
    }
}

impl<'a> From<f64> for ExpressionConvertibleParam<'a> {
    fn from(value: f64) -> Self {
        ExpressionConvertibleParam::Double(CPPType::Double, value)
    }
}

impl<'a> From<String> for ExpressionConvertibleParam<'a> {
    fn from(value: String) -> Self {
        ExpressionConvertibleParam::String(value)
    }
}

impl<'a> From<&'a str> for ExpressionConvertibleParam<'a> {
    fn from(value: &'a str) -> Self {
        ExpressionConvertibleParam::String(value.to_string())
    }
}

impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for ExpressionConvertibleParam<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        ExpressionConvertibleParam::ExpressionConvertible(value)
    }
}
