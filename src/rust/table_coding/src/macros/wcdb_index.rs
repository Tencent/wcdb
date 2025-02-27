use darling::{FromField, FromMeta};
use proc_macro2::Ident;
use syn::Type;

#[derive(Debug, FromMeta)]
// #[darling(attributes(WCDBIndex))]
pub struct WCDBIndex {
    ident: Option<Ident>,
    ty: Type,
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
