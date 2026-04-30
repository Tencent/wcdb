use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(version = "FTS5", tokenizer = "wcdb_verbatim" , tokenizer_parameters = ["chinese_traditional_to_simplified"]))]
pub struct TraditionalChineseObject {
    #[WCDBField]
    pub content: String,
}
