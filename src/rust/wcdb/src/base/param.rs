use crate::winq::column::Column;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::CPPType;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;

/// 支持 bool, i8, i16, i32, i64, f32, f64, String, &str, Option<&dyn ExpressionConvertibleTrait>
pub enum ExpressionConvertibleParam<'a> {
    Int(CPPType, i64),
    Double(CPPType, f64),
    String(String),
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl<'a> From<bool> for ExpressionConvertibleParam<'a> {
    fn from(value: bool) -> Self {
        let value = if value { 1 } else { 0 };
        ExpressionConvertibleParam::Int(CPPType::Bool, value)
    }
}

impl<'a> From<i8> for ExpressionConvertibleParam<'a> {
    fn from(value: i8) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value as i64)
    }
}

impl<'a> From<i16> for ExpressionConvertibleParam<'a> {
    fn from(value: i16) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value as i64)
    }
}

impl<'a> From<i32> for ExpressionConvertibleParam<'a> {
    fn from(value: i32) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value as i64)
    }
}

impl<'a> From<i64> for ExpressionConvertibleParam<'a> {
    fn from(value: i64) -> Self {
        ExpressionConvertibleParam::Int(CPPType::Int, value)
    }
}

impl<'a> From<f32> for ExpressionConvertibleParam<'a> {
    fn from(value: f32) -> Self {
        ExpressionConvertibleParam::Double(CPPType::Double, value as f64)
    }
}

impl<'a> From<f64> for ExpressionConvertibleParam<'a> {
    fn from(value: f64) -> Self {
        ExpressionConvertibleParam::Double(CPPType::Double, value)
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

/// 支持 String, &str, &dyn IndexedColumnConvertibleTrait
pub enum StringIndexedColumnConvertibleParam<'a> {
    String(String),
    IndexedColumnConvertible(&'a dyn IndexedColumnConvertibleTrait),
}

impl<'a> From<String> for StringIndexedColumnConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringIndexedColumnConvertibleParam::String(value)
    }
}

impl<'a> From<&'a str> for StringIndexedColumnConvertibleParam<'a> {
    fn from(value: &'a str) -> Self {
        StringIndexedColumnConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: IndexedColumnConvertibleTrait> From<&'a T> for StringIndexedColumnConvertibleParam<'a> {
    fn from(value: &'a T) -> Self {
        StringIndexedColumnConvertibleParam::IndexedColumnConvertible(value)
    }
}

/// 支持 String, &str, Column
pub enum StringColumnParam<'a> {
    String(String),
    Column(&'a Column),
}

impl<'a> From<String> for StringColumnParam<'a> {
    fn from(value: String) -> Self {
        StringColumnParam::String(value)
    }
}

impl<'a> From<&str> for StringColumnParam<'a> {
    fn from(value: &str) -> Self {
        StringColumnParam::String(value.to_string())
    }
}

impl<'a> From<&'a Column> for StringColumnParam<'a> {
    fn from(value: &'a Column) -> Self {
        StringColumnParam::Column(value)
    }
}

/// 支持 String, &str, &dyn ResultColumnConvertibleTrait
pub enum StringResultColumnConvertibleParam<'a> {
    String(String),
    ResultColumn(&'a dyn ResultColumnConvertibleTrait),
}

impl<'a> From<String> for StringResultColumnConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringResultColumnConvertibleParam::String(value)
    }
}

impl<'a> From<&str> for StringResultColumnConvertibleParam<'a> {
    fn from(value: &str) -> Self {
        StringResultColumnConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: ResultColumnConvertibleTrait> From<&'a T> for StringResultColumnConvertibleParam<'a> {
    fn from(value: &'a T) -> Self {
        StringResultColumnConvertibleParam::ResultColumn(value)
    }
}

/// 支持 String, &str, &dyn TableOrSubqueryConvertibleTrait
pub enum StringTableOrSubqueryConvertibleParam<'a> {
    String(String),
    TableOrSubquery(&'a dyn TableOrSubqueryConvertibleTrait),
}

impl<'a> From<String> for StringTableOrSubqueryConvertibleParam<'a> {
    fn from(value: String) -> Self {
        StringTableOrSubqueryConvertibleParam::String(value)
    }
}

impl<'a> From<&str> for StringTableOrSubqueryConvertibleParam<'a> {
    fn from(value: &str) -> Self {
        StringTableOrSubqueryConvertibleParam::String(value.to_string())
    }
}

impl<'a, T: TableOrSubqueryConvertibleTrait + 'a> From<&'a T>
    for StringTableOrSubqueryConvertibleParam<'a>
{
    fn from(value: &'a T) -> Self {
        StringTableOrSubqueryConvertibleParam::TableOrSubquery(value)
    }
}

/// 支持 String, &str, &dyn ExpressionConvertibleTrait
pub enum StringExpressionConvertibleParam<'a> {
    String(String),
    ExpressionConvertible(&'a dyn ExpressionConvertibleTrait),
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
