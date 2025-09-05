use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;

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
