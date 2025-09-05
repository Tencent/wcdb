use crate::winq::qualified_table::QualifiedTable;

/// 支持 String, &str, &QualifiedTable
pub enum StringQualifiedTableParam<'a> {
    String(String),
    QualifiedTable(&'a QualifiedTable),
}

impl<'a> From<String> for StringQualifiedTableParam<'a> {
    fn from(value: String) -> Self {
        StringQualifiedTableParam::String(value)
    }
}

impl<'a> From<&str> for StringQualifiedTableParam<'a> {
    fn from(value: &str) -> Self {
        StringQualifiedTableParam::String(value.to_string())
    }
}

impl<'a> From<&'a QualifiedTable> for StringQualifiedTableParam<'a> {
    fn from(value: &'a QualifiedTable) -> Self {
        StringQualifiedTableParam::QualifiedTable(value)
    }
}
