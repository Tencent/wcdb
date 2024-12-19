use table_coding::WCDBTableCoding;
use wcdb_core::core::database::Database;

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

fn main() {
    let db = Database::new("/Users/zhanglei/Downloads/test.db");
    db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE);
}
