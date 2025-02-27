use darling::FromMeta;
use syn::LitStr;

#[derive(Debug, FromMeta, Clone)]
pub struct MultiUnique {
    columns: Vec<LitStr>,
}
