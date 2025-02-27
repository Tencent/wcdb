use darling::FromField;
use proc_macro2::Ident;
use syn::Type;

#[derive(Debug, FromField)]
#[darling(attributes(WCDBField))]
pub struct WCDBField {
    pub ident: Option<Ident>,
    pub ty: Type,
    #[darling(default)]
    pub column_name: String,
    #[darling(default)]
    pub is_primary: bool,
    #[darling(default)]
    pub is_auto_increment: bool,
    #[darling(default)]
    pub enable_auto_increment_for_existing_table: bool,
    #[darling(default)]
    pub is_unique: bool,
    #[darling(default)]
    pub is_not_null: bool,
    #[darling(default)]
    pub is_not_indexed: bool,
}
