use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, Clone)]
pub struct PrimaryEnableAutoIncrementObject {
    #[WCDBField(
        is_primary = true,
        is_auto_increment = true,
        enable_auto_increment_for_existing_table = true
    )]
    pub(crate) id: i32,
}

impl PrimaryEnableAutoIncrementObject {
    pub fn new() -> Self {
        PrimaryEnableAutoIncrementObject { id: 0 }
    }
}
