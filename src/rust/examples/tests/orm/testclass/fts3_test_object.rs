use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(version = "FTS3", tokenizer = "wcdb_one_or_binary", tokenizer_parameters = ["skip_stemming"]))]
pub struct Fts3TestObject {
    #[WCDBField]
    id: i64,
    #[WCDBField]
    pub(crate) content: String,
}

impl Fts3TestObject {
    pub(crate) fn new(id: i64, content: &str) -> Self {
        Self {
            id,
            content: String::from(content),
        }
    }
}
