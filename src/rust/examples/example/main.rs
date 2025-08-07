use wcdb::base::wcdb_exception::WCDBException;
use wcdb::core::database::{Database, PerformanceInfo};
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::winq::identifier::IdentifierTrait;
use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_indexes(name = "specifiedNameIndex", columns = ["item_i32", "message_id"]),
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
    #[WCDBField]
    item_float: f32,
    #[WCDBField]
    item_double: f64,
    // todo qixinbing-需要支持 blob 类型
    #[WCDBField]
    item_text: String,
    #[WCDBField]
    item_blob: Vec<u8>,
    #[WCDBField]
    item_blob_opt: Option<Vec<u8>>,
}

impl TableMessageBox {
    pub fn new() -> Self {
        Self {
            item_bool: false,
            item_byte: 1,
            item_short: 2,
            item_i32: 32,
            item_i64: 64,
            item_float: 32.1f32,
            item_double: 64.1f64,
            item_text: "hello".to_string(),
            item_blob: vec![1, 2, 3, 4, 5],
            item_blob_opt: Some(vec![6, 7, 8, 9, 10]),
        }
    }
}

fn main() {
    global_trace();
    let db = Database::new("./target/tmp/test.db");
    db.create_table("rct_message_box", &*DB_TABLE_MESSAGE_BOX_INSTANCE)
        .unwrap();
    test_func(&db);
}

fn global_trace() {
    let ret = Database::global_trace_sql(Some(
        |tag: i64, path: String, handle_id: i64, sql: String, info: String| {
            println!(
                "global_trace_sql tag: {}, path: {}, handle_id: {}, sql: {}, info: {}",
                tag, path, handle_id, sql, info
            );
        },
    ));
    assert!(ret.is_ok());

    let ret = Database::global_trace_exception(Some(|exception: WCDBException| {
        println!("global_trace_exception: {}", exception.message())
    }));
    assert!(ret.is_ok());

    let ret = Database::global_trace_performance(Some(
        |tag: i64, path: String, handle_id: i64, sql: String, info: PerformanceInfo| {
            println!(
                "global_trace_performance tag: {}, path: {}, handle_id: {}, sql: {}, info: {:?}",
                tag, path, handle_id, sql, info
            );
        },
    ));
    assert!(ret.is_ok());
}

fn test_func(db: &Database) {
    let msg_box = TableMessageBox::new();
    db.insert_object(msg_box, DbTableMessageBox::all_fields(), "rct_message_box")
        .unwrap();

    let msg_box_opt = db
        .get_first_object(DbTableMessageBox::all_fields(), "rct_message_box")
        .unwrap();
    println!("qxb test_func");
}
