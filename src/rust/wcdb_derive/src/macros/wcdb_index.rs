use darling::FromMeta;

#[derive(FromMeta, Debug)]
pub struct WCDBIndex {
    #[darling(default)]
    name: String,
    #[darling(default)]
    is_unique: bool,
}

impl WCDBIndex {
    pub fn name(&self) -> String {
        self.name.clone()
    }

    pub fn is_unique(&self) -> bool {
        self.is_unique
    }
}
