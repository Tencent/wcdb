use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::bind_parameter::BindParameter;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::{c_void, CString};

/// support:
/// ```text
/// String, &str
/// &BindParameter
/// ```
pub enum StringBindParameter<'a> {
    String(String),
    BindParameter(&'a BindParameter),
}

impl StringBindParameter<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringBindParameter::String(value) => {
                (CPPType::String, 0 as *mut c_void, Some(value.to_cstring()))
            }
            StringBindParameter::BindParameter(value) => {
                (Identifier::get_cpp_type(value), CppObject::get(value), None)
            }
        }
    }
}

impl<'a> From<String> for StringBindParameter<'a> {
    fn from(value: String) -> Self {
        StringBindParameter::String(value)
    }
}

impl<'a> From<&str> for StringBindParameter<'a> {
    fn from(value: &str) -> Self {
        StringBindParameter::String(value.to_string())
    }
}

impl<'a> From<&'a BindParameter> for StringBindParameter<'a> {
    fn from(value: &'a BindParameter) -> Self {
        StringBindParameter::BindParameter(value)
    }
}
