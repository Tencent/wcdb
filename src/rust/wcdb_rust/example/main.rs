use std::env;
use std::time::SystemTime;
use table_coding::WCDBTableCoding;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::winq::column::Column;
use wcdb_core::winq::expression::Expression;
use wcdb_core::winq::identifier::IdentifierTrait;

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_indexes(name = "specifiedNameIndex", columns = ["multiIndex1", "multiIndex2", "multiIndex3"]),
    multi_indexes(columns = ["multiIndex1", "multiIndex2"]),
    multi_primaries = ["multiPrimary1", "multiPrimary2", "multiPrimary3"],
    multi_unique = ["multiUnique1", "multiUnique2", "multiUnique3"],
)]
pub struct TableMessage {
    #[WCDBField]
    multi_primary1: i32,
    #[WCDBField]
    multi_primary2: i32,
    #[WCDBField(column_name = "multiPrimary3")]
    multi_primary: i32,
    #[WCDBField]
    multi_unique1: i32,
    #[WCDBField]
    multi_unique2: i32,
    #[WCDBField(column_name = "multiUnique3")]
    multi_unique: i32,
    #[WCDBField]
    multi_index1: i32,
    #[WCDBField]
    multi_index2: i32,
    #[WCDBField(column_name = "multiIndex3")]
    multi_index: i32,
}

impl TableMessage {
    pub fn new() -> Self {
        Self {
            multi_primary1: 1,
            multi_primary2: 2,
            multi_primary: 3,
            multi_unique1: 11,
            multi_unique2: 12,
            multi_unique: 13,
            multi_index1: 21,
            multi_index2: 22,
            multi_index: 23,
        }
    }
}

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
    // db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE)
    //     .unwrap();
    db.create_table("rct_message_box", &*DBTABLEMESSAGEBOX_INSTANCE)
        .unwrap();

    // insert_object_to_rct_message(&db);
    // insert_objects_to_rct_message(&db);
    // delete_objects_from_rct_message(&db);
    // delete_objects_by_expression_from_rct_message(&db);
    // update_object_to_rct_message(&db);
    // get_all_object_from_rct_message(&db);
    insert_object_to_rct_message_box(&db);
    get_all_object_from_rct_message_box(&db);
    // get_first_object_from_rct_message_box(&db);
}

fn insert_object_to_rct_message_box(db: &Database) {
    // 插入一条记录
    let mut record = TableMessageBox::new();
    let cur_ts = current_timestamp_ms();
    record.item_i64 = cur_ts;
    db.insert_object(record, DbTableMessageBox::all_fields(), "rct_message_box")
        .unwrap();

    println!("insert_object_to_rct_message_box cur_ts : {:?}", cur_ts);

    // 通过时间戳再获取一次该数据
    let expression = Expression::new_with_column(Column::new("item_i64")).eq_long(cur_ts);
    let desc = expression.get_description();
    println!("expression_desc: {:?}", desc);
    let first_object_ret = db.get_first_object_by_expression::<TableMessageBox>(
        DbTableMessageBox::all_fields(),
        "rct_message_box",
        expression,
    );
    match first_object_ret {
        Ok(obj) => {
            println!("first_object_ret: {:?}", obj.item_i64);
            assert_eq!(obj.item_i64, cur_ts);
        }
        Err(_) => {}
    }
}

fn get_all_object_from_rct_message_box(db: &Database) {
    let all_objects_ret =
        db.get_all_objects::<TableMessageBox>(DbTableMessageBox::all_fields(), "rct_message_box");
    match all_objects_ret {
        Ok(obj_vec) => {
            println!("obj_vec = ")
        }
        Err(_) => {}
    }
}

fn get_first_object_from_rct_message_box(db: &Database) {
    let first_object_ret =
        db.get_first_object::<TableMessageBox>(DbTableMessageBox::all_fields(), "rct_message_box");
    match first_object_ret {
        Ok(obj) => {
            println!("first_object_ret: {:?}", obj.item_i64);
        }
        Err(_) => {}
    };
}

/// 插入单条数据
fn insert_object_to_rct_message(db: &Database) {
    let record = TableMessage::new();
    db.insert_object(record, DbTableMessage::all_fields(), "rct_message")
        .unwrap();
}

/// 插入批量数据
fn insert_objects_to_rct_message(db: &Database) {
    let mut record1 = TableMessage::new();
    record1.multi_unique = 111;
    let mut record2 = TableMessage::new();
    record2.multi_unique = 222;
    let msg_vec = vec![record1, record2];
    db.insert_objects(msg_vec, DbTableMessage::all_fields(), "rct_message")
        .unwrap()
}

fn delete_objects_from_rct_message(db: &Database) {
    db.delete_objects("rct_message").unwrap();
}

fn delete_objects_by_expression_from_rct_message(db: &Database) {
    db.delete_objects_by_expression("rct_message", Expression::new())
        .unwrap();
}

fn update_object_to_rct_message(db: &Database, i: i32) {
    let mut record1 = TableMessage::new();
    record1.multi_unique = i;
    record1.multi_index1 = 999;
    db.update_object(record1, DbTableMessage::all_fields(), "rct_message")
        .unwrap();
}

fn get_all_object_from_rct_message(db: &Database) {
    let all_objects_ret =
        db.get_all_objects::<TableMessage>(DbTableMessage::all_fields(), "rct_message");
    match all_objects_ret {
        Ok(obj_vec) => for obj in obj_vec {},
        Err(_) => {}
    }
}

fn get_current_username() -> String {
    let user_opt = env::var("USER");
    user_opt.unwrap_or_else(|_| "zhanglei".to_string())
}

fn current_timestamp_ms() -> i64 {
    SystemTime::now()
        .duration_since(SystemTime::UNIX_EPOCH)
        .unwrap()
        .as_millis() as i64
}
