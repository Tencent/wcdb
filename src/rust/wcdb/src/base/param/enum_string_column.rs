use crate::winq::column::ColumnTrait;

/// 支持 String, &str, Column
pub enum StringColumn<'a> {
    String(String),
    Column(&'a dyn ColumnTrait),
}

impl<'a> From<String> for StringColumn<'a> {
    fn from(value: String) -> Self {
        StringColumn::String(value)
    }
}

impl<'a> From<&str> for StringColumn<'a> {
    fn from(value: &str) -> Self {
        StringColumn::String(value.to_string())
    }
}

impl<'a, T: ColumnTrait> From<&'a T> for StringColumn<'a> {
    fn from(value: &'a T) -> Self {
        StringColumn::Column(value)
    }
}
