use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier};
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use std::ffi::{c_void, CString};

/// support:
/// ```text
/// String, &str
/// &IndexedColumnConvertibleTrait
/// ```
pub enum StringIndexedColumn<'a> {
    String(String),
    IndexedColumnConvertible(&'a dyn IndexedColumnConvertibleTrait),
}

impl StringIndexedColumn<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringIndexedColumn::String(value) => {
                let cstr = value.as_str().to_cstring();
                (CPPType::String, 0 as *mut c_void, Some(cstr))
            }
            StringIndexedColumn::IndexedColumnConvertible(value) => {
                (Identifier::get_cpp_type(value), CppObject::get(value), None)
            }
        }
    }
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
