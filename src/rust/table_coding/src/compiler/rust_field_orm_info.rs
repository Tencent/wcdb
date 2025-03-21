use once_cell::sync::Lazy;
use std::collections::HashMap;

pub struct RustFieldORMInfo {
    pub(crate) column_type: String,
    pub(crate) nullable: bool,
    pub(crate) field_setter: String,
    pub(crate) field_getter: String,
}

impl RustFieldORMInfo {
    pub fn new(column_type: &str, nullable: bool, field_setter: &str, field_getter: &str) -> Self {
        Self {
            column_type: column_type.to_string(),
            nullable,
            field_setter: field_setter.to_string(),
            field_getter: field_getter.to_string(),
        }
    }
}

pub static RUST_FIELD_ORM_INFO_MAP: Lazy<HashMap<String, RustFieldORMInfo>> = Lazy::new(|| {
    let mut all_info = HashMap::new();
    all_info.insert(
        "bool".to_string(),
        RustFieldORMInfo::new("Integer", false, "bind_bool", "get_bool"),
    );
    all_info.insert(
        "Option<bool>".to_string(),
        RustFieldORMInfo::new("Integer", true, "bind_bool_opt", "get_bool_opt"),
    );
    all_info.insert(
        "i8".to_string(),
        RustFieldORMInfo::new("Integer", false, "bind_i8", "get_i8"),
    );
    all_info.insert(
        "Option<i8>".to_string(),
        RustFieldORMInfo::new("Integer", true, "bind_i8_opt", "get_i8_opt"),
    );
    all_info.insert(
        "i16".to_string(),
        RustFieldORMInfo::new("Integer", false, "bind_i16", "get_i16"),
    );
    all_info.insert(
        "Option<i16>".to_string(),
        RustFieldORMInfo::new("Integer", true, "bind_i16_opt", "get_i16_opt"),
    );
    all_info.insert(
        "i32".to_string(),
        RustFieldORMInfo::new("Integer", false, "bind_i32", "get_i32"),
    );
    all_info.insert(
        "Option<i32>".to_string(),
        RustFieldORMInfo::new("Integer", true, "bind_i32_opt", "get_i32_opt"),
    );
    all_info.insert(
        "i64".to_string(),
        RustFieldORMInfo::new("Integer", false, "bind_i64", "get_i64"),
    );
    all_info.insert(
        "Option<i64>".to_string(),
        RustFieldORMInfo::new("Integer", true, "bind_i64_opt", "get_i64_opt"),
    );
    all_info.insert(
        "f32".to_string(),
        RustFieldORMInfo::new("Float", false, "bind_f32", "get_f32"),
    );
    all_info.insert(
        "Option<f32>".to_string(),
        RustFieldORMInfo::new("Float", true, "bind_f32_opt", "get_f32_opt"),
    );
    all_info.insert(
        "f64".to_string(),
        RustFieldORMInfo::new("Float", false, "bind_f64", "get_f64"),
    );
    all_info.insert(
        "Option<f64>".to_string(),
        RustFieldORMInfo::new("Float", true, "bind_f64_opt", "get_f64_opt"),
    );
    all_info.insert(
        "String".to_string(),
        RustFieldORMInfo::new("Text", false, "bind_text", "get_text"),
    );
    all_info.insert(
        "Option<String>".to_string(),
        RustFieldORMInfo::new("Text", true, "bind_text_opt", "get_text_opt"),
    );
    all_info
});
