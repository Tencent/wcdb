use once_cell::sync::Lazy;
use std::collections::HashMap;

pub struct FieldORMInfo {
    pub(crate) column_type: String,
    nullable: bool,
    pub(crate) field_setter: String,
    pub(crate) field_getter: String,
}

impl FieldORMInfo {
    pub fn new(column_type: &str, nullable: bool, field_setter: &str, field_getter: &str) -> Self {
        Self {
            column_type: column_type.to_string(),
            nullable,
            field_setter: field_setter.to_string(),
            field_getter: field_getter.to_string(),
        }
    }
}

pub static FIELD_ORM_INFO_MAP: Lazy<HashMap<String, FieldORMInfo>> = Lazy::new(|| {
    let mut all_info = HashMap::new();
    all_info.insert(
        "bool".to_string(),
        FieldORMInfo::new("Integer", false, "bind_bool", "get_bool"),
    );
    all_info.insert(
        "i8".to_string(),
        FieldORMInfo::new("Integer", false, "bind_i8", "get_i8"),
    );
    all_info.insert(
        "i16".to_string(),
        FieldORMInfo::new("Integer", false, "bind_i16", "get_i16"),
    );
    all_info.insert(
        "i32".to_string(),
        FieldORMInfo::new("Integer", false, "bind_i32", "get_i32"),
    );
    all_info.insert(
        "i64".to_string(),
        FieldORMInfo::new("Integer", false, "bind_i64", "get_i64"),
    );
    all_info.insert(
        "f32".to_string(),
        FieldORMInfo::new("Float", false, "bind_f32", "get_f32"),
    );
    all_info.insert(
        "f64".to_string(),
        FieldORMInfo::new("Float", false, "bind_f64", "get_f64"),
    );
    all_info.insert(
        "String".to_string(),
        FieldORMInfo::new("Text", false, "bind_text", "get_text"),
    );
    all_info.insert(
        "Option<String>".to_string(),
        FieldORMInfo::new("Text", true, "bind_text", "get_text"),
    );
    all_info
});
