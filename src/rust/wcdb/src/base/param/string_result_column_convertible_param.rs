use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;

/// 支持 String, &str, &dyn ResultColumnConvertibleTrait
pub enum StringResultColumnConvertibleParam<'a> {
    String(String),
    ResultColumn(&'a dyn ResultColumnConvertibleTrait),
}

impl<'a> From<String> for StringResultColumnConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringResultColumnConvertibleParam::String(value)
    }
}

impl<'a> From<&str> for StringResultColumnConvertibleParam<'a> {
    fn from(value: &str) -> Self {
        StringResultColumnConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: ResultColumnConvertibleTrait> From<&'a T> for StringResultColumnConvertibleParam<'a> {
    fn from(value: &'a T) -> Self {
        StringResultColumnConvertibleParam::ResultColumn(value)
    }
}
