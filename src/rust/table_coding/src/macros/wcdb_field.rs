use crate::compiler::rust_field_orm_info::RUST_FIELD_ORM_INFO_MAP;
use crate::macros::field_attr::FieldAttr;
use darling::FromField;
use proc_macro2::{Ident, Span};
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
    attr: Option<FieldAttr>,
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

    pub fn attr(&self) -> &Option<FieldAttr> {
        &self.attr
    }
}

impl WCDBField {
    pub fn is_integer(&self) -> bool {
        let column_type_string = WCDBField::get_field_type_string(&self.ty).unwrap();
        let field_info_opt = RUST_FIELD_ORM_INFO_MAP.get(column_type_string.as_str());
        match field_info_opt {
            None => false,
            Some(field_orm_info) => field_orm_info.column_type == "Integer",
        }
    }
}

impl WCDBField {
    pub fn get_field_type_string(field: &Type) -> syn::Result<String> {
        match field {
            Type::Path(type_path) => {
                if let Some(segment) = type_path.path.segments.first() {
                    let mut type_name = String::new();
                    // 解析 Option<String> | Option<i64>
                    if segment.ident == "Option" {
                        type_name.push_str("Option");

                        if let syn::PathArguments::AngleBracketed(args) = &segment.arguments {
                            let generics: Vec<String> = args
                                .args
                                .iter()
                                .filter_map(|arg| {
                                    // 提取基础类型参数（如 String/i64）
                                    if let GenericArgument::Type(Type::Path(ty)) = arg {
                                        ty.path.segments.last().map(|s| s.ident.to_string())
                                    } else {
                                        None
                                    }
                                })
                                .collect();

                            if !generics.is_empty() {
                                type_name.push('<');
                                type_name.push_str(&generics.join(", "));
                                type_name.push('>');
                            }
                        }
                    }
                    if !type_name.is_empty() {
                        return Ok(type_name);
                    }
                }
                Ok(type_path.path.segments[0].ident.to_string())
            }
            _ => Err(syn::Error::new(
                field.span(),
                "WCDBTable's field type only works on Path",
            )),
        }
    }

    pub fn get_property_type(field: &Type) -> syn::Result<String> {
        let column_type_string = WCDBField::get_field_type_string(field)?;
        Ok(column_type_string)
    }
}
