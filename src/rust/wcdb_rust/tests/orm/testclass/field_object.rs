use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
pub struct FieldObject {
    #[WCDBField]
    field: i32,
    #[WCDBField(
        column_name = "differentName",
        is_primary = true,
        is_auto_increment = true
    )]
    field_with_different_name: i32,
}
