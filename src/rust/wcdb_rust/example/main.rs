use table_coding::WCDBTableCoding;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::winq::identifier::IdentifierTrait;

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_indexes(name = "specifiedNameIndex", columns = ["item_i32", "item_i64"]),
)]
pub struct TableMessageBox {
    #[WCDBField]
    item_bool: bool,
    #[WCDBField]
    item_byte: i8,
    #[WCDBField]
    item_short: i16,
    #[WCDBField]
    item_i32: i32,
    #[WCDBField(column_name = "message_id")]
    item_i64: i64,
    // #[WCDBField]
    // item_float: f32,
    // #[WCDBField]
    // item_double: f64,
    // todo qixinbing->zhanglei1 需要支持 String 和 blob 类型
    #[WCDBField]
    item_text: String,
}

impl TableMessageBox {
    pub fn new() -> Self {
        Self {
            item_bool: false,
            item_byte: 1,
            item_short: 2,
            item_i32: 32,
            item_i64: 64,
            // item_float: 32.1f32,
            // item_double: 64.1f64,
            item_text: "hello".to_string(),
        }
    }
}

fn main() {
    let db = Database::new("./target/tmp/test.db");
    db.create_table("rct_message_box", &*DBTABLEMESSAGEBOX_INSTANCE)
        .unwrap();
    test_func();
}

fn test_func() {}
