use crate::base::cpp_object::CppObject;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::{CPPType, Identifier};
use std::ffi::{c_void, CString};

/// support:
/// ```text
/// String, &str
/// &ExpressionConvertibleTrait     Option<&ExpressionConvertibleTrait>
/// &Column                         Option<&Column>
/// ```
pub enum StringExpression<'a> {
    String(String),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl StringExpression<'_> {
    pub(crate) fn get_params(self) -> (CPPType, *mut c_void, Option<CString>) {
        match self {
            StringExpression::String(str) => {
                (CPPType::String, 0 as *mut c_void, Some(str.to_cstring()))
            }
            StringExpression::ExpressionConvertible(exp_opt) => match exp_opt {
                None => (CPPType::Null, 0 as *mut c_void, None),
                Some(exp) => (Identifier::get_cpp_type(exp), CppObject::get(exp), None),
            },
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

impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for StringExpression<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        StringExpression::ExpressionConvertible(value)
    }
}

impl<'a> From<&'a dyn ExpressionConvertibleTrait> for StringExpression<'a> {
    fn from(value: &'a dyn ExpressionConvertibleTrait) -> Self {
        StringExpression::ExpressionConvertible(Some(value))
    }
}

impl<'a> From<Option<&'a ExpressionOperable>> for StringExpression<'a> {
    fn from(value: Option<&'a ExpressionOperable>) -> Self {
        let v = value.map(|x| x as &dyn ExpressionConvertibleTrait).into();
        StringExpression::ExpressionConvertible(v)
    }
}

impl<'a> From<&'a ExpressionOperable> for StringExpression<'a> {
    fn from(value: &'a ExpressionOperable) -> Self {
        let v = value as &dyn ExpressionConvertibleTrait;
        StringExpression::ExpressionConvertible(Some(v))
    }
}

impl<'a> From<Option<&'a Expression>> for StringExpression<'a> {
    fn from(value: Option<&'a Expression>) -> Self {
        let v = value.map(|x| x as &dyn ExpressionConvertibleTrait).into();
        StringExpression::ExpressionConvertible(v)
    }
}

impl<'a> From<&'a Expression> for StringExpression<'a> {
    fn from(value: &'a Expression) -> Self {
        StringExpression::ExpressionConvertible(Some(value))
    }
}

impl<'a> From<Option<&'a Column>> for StringExpression<'a> {
    fn from(value: Option<&'a Column>) -> Self {
        value.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<&'a Column> for StringExpression<'a> {
    fn from(value: &'a Column) -> Self {
        StringExpression::ExpressionConvertible(Some(value))
    }
}
