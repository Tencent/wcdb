use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(version = "FTS5", tokenizer = "wcdb_verbatim", tokenizer_parameters = ["skip_stemming", "chinese_traditional_to_simplified"], external_table = "contentTable"))]
pub struct Fts5TestObject {
    #[WCDBField]
    pub(crate) id: i32,
    #[WCDBField]
    pub(crate) content: String,
}

impl Fts5TestObject {
    pub(crate) fn new(id: i32, content: &str) -> Self {
        Self {
            id,
            content: String::from(content),
        }
    }
}
