use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier};
use crate::winq::schema::Schema;
use std::ffi::{c_void, CString};

/// 支持 String, &str, Option<&Schema>
pub enum StringSchemaParam<'a> {
    String(String),
    Schema(Option<&'a Schema>),
}

impl StringSchemaParam<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringSchemaParam::String(str) => {
                (CPPType::String, 0 as *mut c_void, Some(str.to_cstring()))
            }
            StringSchemaParam::Schema(schema_opt) => match schema_opt {
                None => (CPPType::Null, 0 as *mut c_void, None),
                Some(sc) => (Identifier::get_cpp_type(sc), CppObject::get(sc), None),
            },
        }
    }
}

impl<'a> From<String> for StringSchemaParam<'a> {
    fn from(value: String) -> Self {
        StringSchemaParam::String(value)
    }
}

impl<'a> From<&str> for StringSchemaParam<'a> {
    fn from(value: &str) -> Self {
        StringSchemaParam::String(value.to_string())
    }
}

impl<'a> From<Option<&'a Schema>> for StringSchemaParam<'a> {
    fn from(value: Option<&'a Schema>) -> Self {
        StringSchemaParam::Schema(value)
    }
}
