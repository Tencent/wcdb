use darling::FromMeta;
use syn::LitStr;

#[derive(Debug, Clone, FromMeta)]
pub struct MultiPrimary {
    #[darling(default)]
    columns: Vec<LitStr>,
}

impl MultiPrimary {
    pub fn new() -> Self {
        MultiPrimary { columns: vec![] }
    }

    pub fn columns(&self) -> &Vec<LitStr> {
        &self.columns
    }
}
