use darling::FromMeta;
use syn::LitStr;

#[derive(Debug, FromMeta, Clone)]
pub struct MultiPrimary {
    columns: Vec<LitStr>,
}
