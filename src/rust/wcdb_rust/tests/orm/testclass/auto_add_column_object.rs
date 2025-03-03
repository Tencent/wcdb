use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
pub struct AutoAddColumnObject {
    #[WCDBField(column_name = "primaryValue", is_primary = true)]
    primary_value: i32,
    #[WCDBField(column_name = "uniqueValue", is_unique = true)]
    unique_value: i32,
    #[WCDBField(column_name = "insertValue")]
    insert_value: i32,
    #[WCDBField(column_name = "updateValue")]
    update_value: i32,
    #[WCDBField(column_name = "selectValue")]
    select_value: i32,
    #[WCDBField(column_name = "multiSelectValue")]
    multi_select_value: i32,
    #[WCDBField(column_name = "deleteValue")]
    delete_value: i32,
    #[WCDBField(column_name = "indexValue")]
    index_value: i32,
}

impl AutoAddColumnObject {
    pub fn new() -> Self {
        Self {
            primary_value: 0,
            unique_value: 0,
            insert_value: 0,
            update_value: 0,
            select_value: 0,
            multi_select_value: 0,
            delete_value: 0,
            index_value: 0,
        }
    }
}
