use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding, Clone)]
#[WCDBTable(
    multi_primaries(columns = ["multiPrimary1", "multiPrimary2", "multiPrimary3"]),
    multi_unique(columns = ["multiUnique1", "multiUnique2", "multiUnique3"]),
    multi_indexes(name = "specifiedNameIndex", columns = ["multiIndex1", "multiIndex2", "multiIndex3"]),
    multi_indexes(columns = ["multiIndex1", "multiIndex2"])
)]
pub struct TableConstraintObject {
    #[WCDBField(column_name = "multiPrimary1")]
    multi_primary1: i32,
    #[WCDBField(column_name = "multiPrimary2")]
    multi_primary2: i32,
    #[WCDBField(column_name = "multiPrimary3")]
    multi_primary: i32,
    #[WCDBField(column_name = "multiUnique1")]
    multi_unique1: i32,
    #[WCDBField(column_name = "multiUnique2")]
    multi_unique2: i32,
    #[WCDBField(column_name = "multiUnique3")]
    multi_unique: i32,
    #[WCDBField(column_name = "multiIndex1")]
    multi_index1: i32,
    #[WCDBField(column_name = "multiIndex2")]
    multi_index2: i32,
    #[WCDBField(column_name = "multiIndex3")]
    multi_index: i32,
}
