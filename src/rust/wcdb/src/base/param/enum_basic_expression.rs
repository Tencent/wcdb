use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::{CPPType, Identifier};
use libc::c_longlong;
use std::ffi::{c_double, c_void, CString};

/// 支持 bool, i8, i16, i32, i64, f32, f64, String, &str, Option<&dyn ExpressionConvertibleTrait>
pub enum BasicExpression<'a> {
    Bool(bool),
    Int(i64),
    Float(f64),
    String(String),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl BasicExpression<'_> {
    pub(crate) fn get_params(self) -> (CPPType, c_longlong, c_double, Option<CString>) {
        match self {
            BasicExpression::Bool(value) => {
                let value = if value { 1 } else { 0 };
                (CPPType::Bool, value as c_longlong, 0 as c_double, None)
            }
            BasicExpression::Int(value) => (CPPType::Int, value as c_longlong, 0 as c_double, None),
            BasicExpression::Float(value) => {
                (CPPType::Double, 0 as c_longlong, value as c_double, None)
            }
            BasicExpression::String(value) => {
                let cstr = value.as_str().to_cstring();
                (CPPType::String, 0 as c_longlong, 0 as c_double, Some(cstr))
            }
            BasicExpression::ExpressionConvertible(obj_opt) => {
                let (cpp_type, cpp_obj) = match obj_opt {
                    None => (CPPType::Null, 0 as *mut c_void),
                    Some(obj) => (Identifier::get_cpp_type(obj), CppObject::get(obj)),
                };
                (cpp_type, cpp_obj as c_longlong, 0 as c_double, None)
            }
        }
    }
}

impl<'a> From<bool> for BasicExpression<'a> {
    fn from(value: bool) -> Self {
        BasicExpression::Bool(value)
    }
}

/// 宏定义：为所有整数类型实现 From trait
macro_rules! impl_from_int_types {
    ($($int_type:ty),* $(,)?) => {
        $(
            impl<'a> From<$int_type> for BasicExpression<'a> {
                fn from(value: $int_type) -> Self {
                    BasicExpression::Int(value as i64)
                }
            }
        )*
    };
}

/// 使用宏为所有整数类型实现 From trait
impl_from_int_types!(i8, i16, i32, i64);

/// 宏定义：为所有浮点类型实现 From trait
macro_rules! impl_from_float_types {
    ($($float_type:ty),* $(,)?) => {
        $(
            impl<'a> From<$float_type> for BasicExpression<'a> {
                fn from(value: $float_type) -> Self {
                    BasicExpression::Float(value as f64)
                }
            }
        )*
    };
}

/// 使用宏为所有浮点类型实现 From trait
impl_from_float_types!(f32, f64);

impl<'a> From<String> for BasicExpression<'a> {
    fn from(value: String) -> Self {
        BasicExpression::String(value)
    }
}

impl<'a> From<&'a str> for BasicExpression<'a> {
    fn from(value: &'a str) -> Self {
        BasicExpression::String(value.to_string())
    }
}

impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for BasicExpression<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        BasicExpression::ExpressionConvertible(value)
    }
}

impl<'a> From<Option<&'a ExpressionOperable>> for BasicExpression<'a> {
    fn from(v: Option<&'a ExpressionOperable>) -> Self {
        v.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<Option<&'a Expression>> for BasicExpression<'a> {
    fn from(value: Option<&'a Expression>) -> Self {
        value.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<&'a Expression> for BasicExpression<'a> {
    fn from(value: &'a Expression) -> Self {
        BasicExpression::ExpressionConvertible(Some(value))
    }
}

impl<'a> From<Option<&'a Column>> for BasicExpression<'a> {
    fn from(value: Option<&'a Column>) -> Self {
        value.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<&'a Column> for BasicExpression<'a> {
    fn from(value: &'a Column) -> Self {
        BasicExpression::ExpressionConvertible(Some(value))
    }
}
