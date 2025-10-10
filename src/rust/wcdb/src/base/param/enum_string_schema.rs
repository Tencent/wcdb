use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier};
use crate::winq::schema::Schema;
use std::ffi::{c_void, CString};

/// 支持 String, &str, Option<&Schema>
pub enum StringSchema<'a> {
    String(String),
    Schema(Option<&'a Schema>),
}

impl StringSchema<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringSchema::String(str) => {
                (CPPType::String, 0 as *mut c_void, Some(str.to_cstring()))
            }
            StringSchema::Schema(schema_opt) => match schema_opt {
                None => (CPPType::Null, 0 as *mut c_void, None),
                Some(sc) => (Identifier::get_cpp_type(sc), CppObject::get(sc), None),
            },
        }
    }
}

impl<'a> From<String> for StringSchema<'a> {
    fn from(value: String) -> Self {
        StringSchema::String(value)
    }
}

impl<'a> From<&str> for StringSchema<'a> {
    fn from(value: &str) -> Self {
        StringSchema::String(value.to_string())
    }
}

impl<'a> From<Option<&'a Schema>> for StringSchema<'a> {
    fn from(value: Option<&'a Schema>) -> Self {
        StringSchema::Schema(value)
    }
}
