use crate::default_value_info::DefaultValueInfo;

#[derive(Debug)]
pub struct ColumnInfo {
    pub property_name: String,
    // property_type: String,
    // nullable : bool,
    pub column_name: String,
    // is_primary: bool,
    // is_auto_increment: bool,
    // enable_auto_increment_for_existing_table:bool,
    // default_value: Option<DefaultValueInfo>,
    // is_unique: bool,
    // is_not_null: bool,
    // is_not_indexed: bool,
    // has_index: bool,
    // index_name: String,
    // index_is_unique: bool,
}

impl ColumnInfo {
    pub fn new() -> Self {
        ColumnInfo {
            property_name: String::new(),
            // property_type: String::new(),
            // nullable: false,
            column_name: String::new(),
            // is_primary: false,
            // is_auto_increment: false,
            // enable_auto_increment_for_existing_table: false,
            // default_value: None,
            // is_unique: false,
            // is_not_null: false,
            // is_not_indexed: false,
            // has_index: false,
            // index_name: String::new(),
            // index_is_unique: false,
        }
    }
}
