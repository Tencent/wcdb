use std::env;
use table_coding::WCDBTableCoding;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::orm::field::Field;
use wcdb_core::winq::expression::Expression;

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

fn main() {
    let db = Database::new("./target/tmp/test.db");
    db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE)
        .unwrap();

    insert_object_to_rct_message(&db);
    insert_objects_to_rct_message(&db);
    // delete_objects_from_rct_message(&db);
    // delete_objects_by_expression_from_rct_message(&db);
    // update_object_to_rct_message(&db);
    get_all_object_from_rct_message(&db);
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

fn update_object_to_rct_message(db: &Database) {
    let mut record1 = TableMessage::new();
    record1.multi_unique = 111;
    record1.multi_index1 = 999;
    db.update_object(record1, DbTableMessage::all_fields(), "rct_message")
        .unwrap();
}

fn get_all_object_from_rct_message(db: &Database) {
    // let fields: Vec<&Field<TableMessage>> = DbTableMessage::all_fields();
    let mut field_vec: Vec<Field<TableMessage>> = vec![];

    let field = Field::new("multi_primary1", &*DBTABLEMESSAGE_INSTANCE, 1, false, false);
    field_vec.push(field);

    let field = Field::new("multi_primary2", &*DBTABLEMESSAGE_INSTANCE, 2, false, false);
    field_vec.push(field);

    let field = Field::new("multi_primary", &*DBTABLEMESSAGE_INSTANCE, 3, false, false);
    field_vec.push(field);

    let field = Field::new("multi_unique1", &*DBTABLEMESSAGE_INSTANCE, 4, false, false);
    field_vec.push(field);

    let field = Field::new("multi_unique2", &*DBTABLEMESSAGE_INSTANCE, 5, false, false);
    field_vec.push(field);

    let field = Field::new("multi_unique", &*DBTABLEMESSAGE_INSTANCE, 6, false, false);
    field_vec.push(field);

    let field = Field::new("multi_index1", &*DBTABLEMESSAGE_INSTANCE, 7, false, false);
    field_vec.push(field);

    let field = Field::new("multi_index2", &*DBTABLEMESSAGE_INSTANCE, 8, false, false);
    field_vec.push(field);

    let field = Field::new("multi_index", &*DBTABLEMESSAGE_INSTANCE, 9, false, false);
    field_vec.push(field);

    let all_objects_ret = db.get_all_objects::<TableMessage>(field_vec, "rct_message");
    match all_objects_ret {
        Ok(obj_vec) => for obj in obj_vec {},
        Err(_) => {}
    }
}

fn get_current_username() -> String {
    let user_opt = env::var("USER");
    user_opt.unwrap_or_else(|_| "zhanglei".to_string())
}
