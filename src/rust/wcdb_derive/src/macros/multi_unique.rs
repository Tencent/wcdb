use darling::FromMeta;
use syn::LitStr;

#[derive(Debug, Clone, FromMeta)]
pub struct MultiUnique {
    #[darling(default)]
    columns: Vec<LitStr>,
}

impl MultiUnique {
    pub fn new() -> Self {
        MultiUnique { columns: vec![] }
    }

    pub fn columns(&self) -> &Vec<LitStr> {
        &self.columns
    }
}
