use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(version = "FTS5", tokenizer = "wcdb_pinyin"))]
pub struct PinyinObject {
    #[WCDBField]
    pub content: String,
}

impl PinyinObject {
    pub fn new() -> Self {
        Self {
            content: String::new(),
        }
    }
}
