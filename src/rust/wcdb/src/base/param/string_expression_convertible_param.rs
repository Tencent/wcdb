use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::{c_char, c_void};

/// 支持 String, &str, &dyn ExpressionConvertibleTrait
pub enum StringExpressionConvertibleParam<'a> {
    String(String),
    ExpressionConvertible(&'a dyn ExpressionConvertibleTrait),
}

impl StringExpressionConvertibleParam<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, *const c_char) {
        match self {
            StringExpressionConvertibleParam::String(str) => {
                (CPPType::String, 0 as *mut c_void, str.to_cstring().as_ptr())
            }
            StringExpressionConvertibleParam::ExpressionConvertible(exp) => (
                Identifier::get_cpp_type(exp),
                CppObject::get(exp),
                std::ptr::null(),
            ),
        }
    }
}

impl<'a> From<String> for StringExpressionConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringExpressionConvertibleParam::String(value)
    }
}

impl<'a> From<&str> for StringExpressionConvertibleParam<'a> {
    fn from(value: &str) -> Self {
        StringExpressionConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: ExpressionConvertibleTrait> From<&'a T> for StringExpressionConvertibleParam<'a> {
    fn from(value: &'a T) -> Self {
        StringExpressionConvertibleParam::ExpressionConvertible(value)
    }
}
