use crate::winq::column::ColumnTrait;

/// 支持 String, &str, Column
pub enum StringColumnTraitParam<'a> {
    String(String),
    Column(&'a dyn ColumnTrait),
}

impl<'a> From<String> for StringColumnTraitParam<'a> {
    fn from(value: String) -> Self {
        StringColumnTraitParam::String(value)
    }
}

impl<'a> From<&str> for StringColumnTraitParam<'a> {
    fn from(value: &str) -> Self {
        StringColumnTraitParam::String(value.to_string())
    }
}

impl<'a, T: ColumnTrait> From<&'a T> for StringColumnTraitParam<'a> {
    fn from(value: &'a T) -> Self {
        StringColumnTraitParam::Column(value)
    }
}
