use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;

/// 支持 String, &str, &dyn TableOrSubqueryConvertibleTrait
pub enum StringTableOrSubquery<'a> {
    String(String),
    TableOrSubquery(&'a dyn TableOrSubqueryConvertibleTrait),
}

impl<'a> From<String> for StringTableOrSubquery<'a> {
    fn from(value: String) -> Self {
        StringTableOrSubquery::String(value)
    }
}

impl<'a> From<&str> for StringTableOrSubquery<'a> {
    fn from(value: &str) -> Self {
        StringTableOrSubquery::String(value.to_string())
    }
}

impl<'a, T: TableOrSubqueryConvertibleTrait + 'a> From<&'a T> for StringTableOrSubquery<'a> {
    fn from(value: &'a T) -> Self {
        StringTableOrSubquery::TableOrSubquery(value)
    }
}
