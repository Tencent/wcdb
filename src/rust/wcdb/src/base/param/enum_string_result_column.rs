use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;

/// 支持 String, &str, &dyn ResultColumnConvertibleTrait
pub enum StringResultColumn<'a> {
    String(String),
    ResultColumn(&'a dyn ResultColumnConvertibleTrait),
}

impl<'a> From<String> for StringResultColumn<'a> {
    fn from(value: String) -> Self {
        StringResultColumn::String(value)
    }
}

impl<'a> From<&str> for StringResultColumn<'a> {
    fn from(value: &str) -> Self {
        StringResultColumn::String(value.to_string())
    }
}

impl<'a, T: ResultColumnConvertibleTrait> From<&'a T> for StringResultColumn<'a> {
    fn from(value: &'a T) -> Self {
        StringResultColumn::ResultColumn(value)
    }
}
