use table_coding::WCDBTableCoding;

#[derive(PartialEq, Debug, WCDBTableCoding)]
#[WCDBTable]
pub struct TestObject {
    #[WCDBField(is_primary = true, is_auto_increment = true)]
    id: i32,
    #[WCDBField]
    content: String,
}
impl TestObject {
    pub fn new(content: String) -> TestObject {
        TestObject {
            id: 0,
            content: content.clone(),
        }
    }

    pub fn create_object(id: i32, content: String) -> TestObject {
        TestObject {
            id,
            content: content.clone(),
        }
    }

    pub fn create_auto_increment_object(content: String) -> TestObject {
        TestObject {
            id: 0,
            content: content.clone(),
        }
    }

    pub fn id(&self) -> i32 {
        self.id
    }

    pub fn content(&self) -> &str {
        &self.content
    }
}
