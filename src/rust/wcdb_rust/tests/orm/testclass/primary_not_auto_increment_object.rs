use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
pub struct PrimaryNotAutoIncrementObject {
    #[WCDBField(is_primary = true)]
    pub id: i32,
}

impl PrimaryNotAutoIncrementObject {
    pub fn new() -> Self {
        PrimaryNotAutoIncrementObject { id: 0 }
    }
}
