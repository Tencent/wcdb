use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;

/// 支持 String, &str, &dyn IndexedColumnConvertibleTrait
pub enum StringIndexedColumnConvertibleParam<'a> {
    String(String),
    IndexedColumnConvertible(&'a dyn IndexedColumnConvertibleTrait),
}

impl<'a> From<String> for StringIndexedColumnConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringIndexedColumnConvertibleParam::String(value)
    }
}

impl<'a> From<&'a str> for StringIndexedColumnConvertibleParam<'a> {
    fn from(value: &'a str) -> Self {
        StringIndexedColumnConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: IndexedColumnConvertibleTrait> From<&'a T> for StringIndexedColumnConvertibleParam<'a> {
    fn from(value: &'a T) -> Self {
        StringIndexedColumnConvertibleParam::IndexedColumnConvertible(value)
    }
}
