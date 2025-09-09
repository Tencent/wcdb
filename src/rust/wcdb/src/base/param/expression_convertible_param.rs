use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::{CPPType, Identifier};
use libc::c_longlong;
use std::ffi::{c_char, c_double, c_void};

/// 支持 bool, i8, i16, i32, i64, f32, f64, String, &str, Option<&dyn ExpressionConvertibleTrait>
pub enum ExpressionConvertibleParam<'a> {
    Bool(bool),
    I64(i64),
    F64(f64),
    String(String),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl ExpressionConvertibleParam<'_> {
    pub(crate) fn get_params(self) -> (CPPType, c_longlong, c_double, *const c_char) {
        match self {
            ExpressionConvertibleParam::Bool(value) => {
                let value = if value { 1 } else { 0 };
                (
                    CPPType::Bool,
                    value as c_longlong,
                    0 as c_double,
                    std::ptr::null_mut(),
                )
            }
            ExpressionConvertibleParam::I64(value) => (
                CPPType::Int,
                value as c_longlong,
                0 as c_double,
                std::ptr::null_mut(),
            ),
            ExpressionConvertibleParam::F64(value) => (
                CPPType::Double,
                0 as c_longlong,
                value as c_double,
                std::ptr::null_mut(),
            ),
            ExpressionConvertibleParam::String(value) => (
                CPPType::String,
                0 as c_longlong,
                0 as c_double,
                value.as_str().to_cstring().as_ptr(),
            ),
            ExpressionConvertibleParam::ExpressionConvertible(obj_opt) => {
                let (cpp_type, cpp_obj) = match obj_opt {
                    None => (CPPType::Null, 0 as *mut c_void),
                    Some(obj) => (Identifier::get_cpp_type(obj), CppObject::get(obj)),
                };
                (
                    cpp_type,
                    cpp_obj as c_longlong,
                    0 as c_double,
                    std::ptr::null_mut(),
                )
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
        ExpressionConvertibleParam::I64(value as i64)
    }
}

impl<'a> From<i16> for ExpressionConvertibleParam<'a> {
    fn from(value: i16) -> Self {
        ExpressionConvertibleParam::I64(value as i64)
    }
}

impl<'a> From<i32> for ExpressionConvertibleParam<'a> {
    fn from(value: i32) -> Self {
        ExpressionConvertibleParam::I64(value as i64)
    }
}

impl<'a> From<i64> for ExpressionConvertibleParam<'a> {
    fn from(value: i64) -> Self {
        ExpressionConvertibleParam::I64(value)
    }
}

impl<'a> From<f32> for ExpressionConvertibleParam<'a> {
    fn from(value: f32) -> Self {
        ExpressionConvertibleParam::F64(value as f64)
    }
}

impl<'a> From<f64> for ExpressionConvertibleParam<'a> {
    fn from(value: f64) -> Self {
        ExpressionConvertibleParam::F64(value)
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

impl<'a> From<Option<&'a ExpressionOperable>> for ExpressionConvertibleParam<'a> {
    fn from(v: Option<&'a ExpressionOperable>) -> Self {
        v.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<Option<&'a Expression>> for ExpressionConvertibleParam<'a> {
    fn from(value: Option<&'a Expression>) -> Self {
        value.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<&'a Expression> for ExpressionConvertibleParam<'a> {
    fn from(value: &'a Expression) -> Self {
        ExpressionConvertibleParam::ExpressionConvertible(Some(value))
    }
}

impl<'a> From<Option<&'a Column>> for ExpressionConvertibleParam<'a> {
    fn from(value: Option<&'a Column>) -> Self {
        value.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<&'a Column> for ExpressionConvertibleParam<'a> {
    fn from(value: &'a Column) -> Self {
        ExpressionConvertibleParam::ExpressionConvertible(Some(value))
    }
}
