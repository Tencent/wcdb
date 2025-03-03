use darling::FromMeta;

#[derive(Debug, FromMeta)]
pub struct WCDBIndex {
    #[darling(default)]
    name: String,
    #[darling(default)]
    is_unique: bool,
}

impl WCDBIndex {
    // pub fn new() -> Self {
    //     WCDBIndex { ident: None, ty: (), name: "".to_string(), is_unique: false }
    // }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn is_unique(&self) -> bool {
        self.is_unique
    }
}
