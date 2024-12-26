use table_coding::WCDBTableCoding;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;

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
    let db = Database::new("/Users/zhanglei/Downloads/test.db");
    db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE);
    let record = TableMessage::new();
    db.insert_object(record, DbTableMessage::all_fields(), "rct_message").unwrap();
}
