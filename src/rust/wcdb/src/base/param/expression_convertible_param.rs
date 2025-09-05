use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::{c_char, c_double, c_void};

/// 支持 bool, i8, i16, i32, i64, f32, f64, String, &str, Option<&dyn ExpressionConvertibleTrait>
pub enum ExpressionConvertibleParam<'a> {
    Bool(bool),
    Int(i64),
    Double(f64),
    String(String),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl ExpressionConvertibleParam<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, c_double, *const c_char) {
        match self {
            ExpressionConvertibleParam::Bool(value) => {
                let value = if value { 1 } else { 0 };
                (
                    CPPType::Bool,
                    value as *mut c_void,
                    0 as c_double,
                    std::ptr::null_mut(),
                )
            }
            ExpressionConvertibleParam::Int(value) => (
                CPPType::Int,
                value as *mut c_void,
                0 as c_double,
                std::ptr::null_mut(),
            ),
            ExpressionConvertibleParam::Double(value) => (
                CPPType::Double,
                0 as *mut c_void,
                value as c_double,
                std::ptr::null_mut(),
            ),
            ExpressionConvertibleParam::String(value) => (
                CPPType::String,
                0 as *mut c_void,
                0 as c_double,
                value.as_str().to_cstring().as_ptr(),
            ),
            ExpressionConvertibleParam::ExpressionConvertible(obj_opt) => {
                let (cpp_type, cpp_obj) = match obj_opt {
                    None => (CPPType::Null, 0 as *mut c_void),
                    Some(obj) => (Identifier::get_cpp_type(obj), CppObject::get(obj)),
                };
                (cpp_type, cpp_obj, 0 as c_double, std::ptr::null_mut())
            }
        }
    }
}

impl<'a> From<bool> for ExpressionConvertibleParam<'a> {
    fn from(value: bool) -> Self {
        ExpressionConvertibleParam::Bool(value)
    }
}

impl<'a> From<i8> for ExpressionConvertibleParam<'a> {
    fn from(value: i8) -> Self {
        ExpressionConvertibleParam::Int(value as i64)
    }
}

impl<'a> From<i16> for ExpressionConvertibleParam<'a> {
    fn from(value: i16) -> Self {
        ExpressionConvertibleParam::Int(value as i64)
    }
}

impl<'a> From<i32> for ExpressionConvertibleParam<'a> {
    fn from(value: i32) -> Self {
        ExpressionConvertibleParam::Int(value as i64)
    }
}

impl<'a> From<i64> for ExpressionConvertibleParam<'a> {
    fn from(value: i64) -> Self {
        ExpressionConvertibleParam::Int(value)
    }
}

impl<'a> From<f32> for ExpressionConvertibleParam<'a> {
    fn from(value: f32) -> Self {
        ExpressionConvertibleParam::Double(value as f64)
    }
}

impl<'a> From<f64> for ExpressionConvertibleParam<'a> {
    fn from(value: f64) -> Self {
        ExpressionConvertibleParam::Double(value)
    }
}

impl<'a> From<String> for ExpressionConvertibleParam<'a> {
    fn from(value: String) -> Self {
        ExpressionConvertibleParam::String(value)
    }
}

impl<'a> From<&'a str> for ExpressionConvertibleParam<'a> {
    fn from(value: &'a str) -> Self {
        ExpressionConvertibleParam::String(value.to_string())
    }
}

impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for ExpressionConvertibleParam<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        ExpressionConvertibleParam::ExpressionConvertible(value)
    }
}
