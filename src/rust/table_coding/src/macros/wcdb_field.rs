use crate::macros::wcdb_default::WCDBDefault;
use crate::macros::wcdb_index::WCDBIndex;
use darling::FromField;
use proc_macro2::Ident;
use syn::spanned::Spanned;
use syn::{GenericArgument, Type};

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
    #[darling(default)]
    default: Option<WCDBDefault>,
    #[darling(default)]
    index: Option<WCDBIndex>,
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

    pub fn default(&self) -> &Option<WCDBDefault> {
        &self.default
    }

    pub fn index(&self) -> &Option<WCDBIndex> {
        &self.index
    }
}

impl WCDBField {
    fn extract_type_path(ty: &Type) -> Vec<String> {
        let mut segments = Vec::new();
        if let Type::Path(type_path) = ty {
            for segment in &type_path.path.segments {
                segments.push(segment.ident.to_string());
                if let syn::PathArguments::AngleBracketed(args) = &segment.arguments {
                    for arg in &args.args {
                        if let GenericArgument::Type(inner_ty) = arg {
                            segments.extend(Self::extract_type_path(inner_ty));
                        }
                    }
                }
            }
        }
        segments
    }

    pub fn get_field_type_string(field: &Type) -> syn::Result<String> {
        let segments = Self::extract_type_path(field);
        let mut value = segments.iter().fold(String::new(), |acc, s| {
            if acc.is_empty() {
                s.clone()
            } else {
                format!("{}<{}", acc, s)
            }
        });
        if segments.len() > 1 {
            for _ in 0..segments.len() - 1 {
                value.push('>');
            }
        }
        Ok(value)
    }
}
