use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::column::ColumnTrait;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::{c_void, CString};

/// support:
/// ```text
/// String, &str
/// &ColumnTrait
/// ```
pub enum StringColumn<'a> {
    String(String),
    Column(&'a dyn ColumnTrait),
}

impl StringColumn<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringColumn::String(value) => {
                let cstr = value.as_str().to_cstring();
                (CPPType::String, 0 as *mut c_void, Some(cstr))
            }
            StringColumn::Column(column) => (
                Identifier::get_cpp_type(column),
                CppObject::get(column),
                None,
            ),
        }
    }
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
