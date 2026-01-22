use crate::winq::qualified_table::QualifiedTable;

/// 支持 String, &str, &QualifiedTable
pub enum StringQualifiedTable<'a> {
    String(String),
    QualifiedTable(&'a QualifiedTable),
}

impl<'a> From<String> for StringQualifiedTable<'a> {
    fn from(value: String) -> Self {
        StringQualifiedTable::String(value)
    }
}

impl<'a> From<&str> for StringQualifiedTable<'a> {
    fn from(value: &str) -> Self {
        StringQualifiedTable::String(value.to_string())
    }
}

impl<'a> From<&'a QualifiedTable> for StringQualifiedTable<'a> {
    fn from(value: &'a QualifiedTable) -> Self {
        StringQualifiedTable::QualifiedTable(value)
    }
}
