use darling::FromField;
use proc_macro2::Ident;
use syn::Type;

#[derive(Debug, FromField)]
#[darling(attributes(WCDBField))]
pub struct WCDBField {
    ident: Option<Ident>,
    ty: Type,
    #[darling(default)]
    column_name: String,
    #[darling(default)]
    is_primary: bool,
    #[darling(default)]
    is_auto_increment: bool,
    #[darling(default)]
    enable_auto_increment_for_existing_table: bool,
    #[darling(default)]
    is_unique: bool,
    #[darling(default)]
    is_not_null: bool,
    #[darling(default)]
    is_not_indexed: bool,
}

impl WCDBField {
    pub fn ident(&self) -> &Option<Ident> {
        &self.ident
    }

    pub fn ty(&self) -> &Type {
        &self.ty
    }

    pub fn column_name(&self) -> String {
        self.column_name.clone()
    }

    pub fn is_primary(&self) -> bool {
        self.is_primary
    }

    pub fn is_auto_increment(&self) -> bool {
        self.is_auto_increment
    }

    pub fn enable_auto_increment_for_existing_table(&self) -> bool {
        self.enable_auto_increment_for_existing_table
    }

    pub fn is_unique(&self) -> bool {
        self.is_unique
    }

    pub fn is_not_null(&self) -> bool {
        self.is_not_null
    }

    pub fn is_not_indexed(&self) -> bool {
        self.is_not_indexed
    }
}
