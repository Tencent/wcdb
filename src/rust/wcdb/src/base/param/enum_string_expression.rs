use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::{c_void, CString};

/// 支持 String, &str, &dyn ExpressionConvertibleTrait
pub enum StringExpression<'a> {
    String(String),
    ExpressionConvertible(&'a dyn ExpressionConvertibleTrait),
}

impl StringExpression<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringExpression::String(str) => {
                (CPPType::String, 0 as *mut c_void, Some(str.to_cstring()))
            }
            StringExpression::ExpressionConvertible(exp) => {
                (Identifier::get_cpp_type(exp), CppObject::get(exp), None)
            }
        }
    }
}

impl<'a> From<String> for StringExpression<'a> {
    fn from(value: String) -> Self {
        StringExpression::String(value)
    }
}

impl<'a> From<&str> for StringExpression<'a> {
    fn from(value: &str) -> Self {
        StringExpression::String(value.to_string())
    }
}

impl<'a, T: ExpressionConvertibleTrait> From<&'a T> for StringExpression<'a> {
    fn from(value: &'a T) -> Self {
        StringExpression::ExpressionConvertible(value)
    }
}
