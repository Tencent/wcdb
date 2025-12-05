use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(version = "FTS3", tokenizer = "mmicu"))]
pub struct MmicuTestObject {
    #[WCDBField]
    id: i64,
    #[WCDBField]
    pub(crate) content: String,
}

impl MmicuTestObject {
    pub(crate) fn new(id: i64, content: &str) -> Self {
        Self {
            id,
            content: String::from(content),
        }
    }
}
