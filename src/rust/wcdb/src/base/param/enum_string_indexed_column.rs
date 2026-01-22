use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;

/// 支持 String, &str, &dyn IndexedColumnConvertibleTrait
pub enum StringIndexedColumn<'a> {
    String(String),
    IndexedColumnConvertible(&'a dyn IndexedColumnConvertibleTrait),
}

impl<'a> From<String> for StringIndexedColumn<'a> {
    fn from(value: String) -> Self {
        StringIndexedColumn::String(value)
    }
}

impl<'a> From<&'a str> for StringIndexedColumn<'a> {
    fn from(value: &'a str) -> Self {
        StringIndexedColumn::String(value.to_string())
    }
}

impl<'a, T: IndexedColumnConvertibleTrait> From<&'a T> for StringIndexedColumn<'a> {
    fn from(value: &'a T) -> Self {
        StringIndexedColumn::IndexedColumnConvertible(value)
    }
}
