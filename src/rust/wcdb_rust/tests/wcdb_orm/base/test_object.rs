use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
pub struct TestObject {
    #[WCDBField]
    id: i32,
    // #[WCDBField] todo qixinbing
    // content: String,
}

impl TestObject {}
