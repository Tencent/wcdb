use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::identifier::{CPPType, Identifier};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_void};

/// 支持 String, &str, Column
pub enum StringColumnParam<'a> {
    String(String),
    Column(&'a Column),
}

impl<'a> From<String> for StringColumnParam<'a> {
    fn from(value: String) -> Self {
        StringColumnParam::String(value)
    }
}

impl<'a> From<&str> for StringColumnParam<'a> {
    fn from(value: &str) -> Self {
        StringColumnParam::String(value.to_string())
    }
}

impl<'a> From<&'a Column> for StringColumnParam<'a> {
    fn from(value: &'a Column) -> Self {
        StringColumnParam::Column(value)
    }
}
