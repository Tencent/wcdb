use darling::ast::Data;
use darling::{FromDeriveInput, FromField, FromMeta};
use proc_macro::TokenStream;
use proc_macro2::Span;
use quote::{quote, ToTokens};
use std::fmt::Debug;
use syn::parse::Parse;
use syn::{parse_macro_input, DeriveInput, Generics, Ident, LitStr, Type};

#[derive(Debug, FromDeriveInput)]
#[darling(attributes(WCDBTable))]
struct WCDBTable {
    ident: Ident,
    generics: Generics,
    data: Data<(), WCDBField>,
    #[darling(default, multiple)]
    multi_indexes: Vec<MultiIndexes>,
    #[darling(default)]
    multi_primaries: Vec<LitStr>,
    #[darling(default)]
    multi_unique: Vec<LitStr>,
    #[darling(default)]
    is_without_row_id: bool,
    // #[darling(default)]
    // fts_module: ???,
}

impl WCDBTable {
    fn get_db_table(&self) -> Ident {
        Ident::new(&format!("Db{}", self.ident), Span::call_site())
    }

    fn get_field_ident_vec(&self) -> Vec<&Ident> {
        match &self.data {
            Data::Struct(fields) => fields.iter().map(|field| field.ident.as_ref().unwrap()).collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    fn get_field_type_vec(&self) -> Vec<&Type> {
        match &self.data {
            Data::Struct(fields) => fields.iter().map(|field| &field.ty).collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }
}

#[derive(Debug, FromMeta)]
struct MultiIndexes {
    name: Option<LitStr>,
    columns: Vec<LitStr>,
}

#[derive(Debug, FromField)]
#[darling(attributes(WCDBField))]
struct WCDBField {
    ident: Option<Ident>,
    ty: Type,
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

#[proc_macro_derive(WCDBTableCoding, attributes(WCDBTable, WCDBField))]
pub fn wcdb_table_coding(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    let table = WCDBTable::from_derive_input(&input).unwrap();
    let table_ident = &table.ident;
    let db_table_ident = table.get_db_table();
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();

    let global_singleton = generate_singleton(&table);

    let quote = quote! {
        #global_singleton

        impl Default for #table_ident {
            fn default() -> Self {
                Self {
                    #(#field_ident_vec: Default::default()),*
                }
            }
        }

        pub struct #db_table_ident {
            #(pub #field_ident_vec: *const wcdb_core::orm::field::Field<#table_ident>),*
        }

        impl Default for #db_table_ident {
            fn default() -> Self {
                Self {
                    #(#field_ident_vec: std::ptr::null()),*
                }
            }
        }

        unsafe impl Send for #db_table_ident {}
        unsafe impl Sync for #db_table_ident {}
    };
    quote.into()
}

fn generate_singleton(table: &WCDBTable) -> proc_macro2::TokenStream {
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    quote! {
        static #binding_ident: once_cell::sync::Lazy<wcdb_core::orm::binding::Binding> = once_cell::sync::Lazy::new(|| {
            wcdb_core::orm::binding::Binding::new()
        });
        static #instance_ident: once_cell::sync::Lazy<#db_table_ident> = once_cell::sync::Lazy::new(|| {
            let mut instance = #db_table_ident::default();
            let instance_raw = unsafe { &instance as *const #db_table_ident };
            // #(
            // let field = Box::new(Field::new("#field_name_vec", instance_raw, 1, false, false));
            // let multi_primary1_def = ColumnDef::new_with_column_type(&field.get_column(), ColumnType::Integer);
            // instance.multi_primary1 = unsafe { Box::into_raw(field) };
            // #binding_ident.add_column_def(multi_primary1_def);
            // )*
            instance
        });
    }
}

#[cfg(test)]
mod tests {
    use crate::MultiIndexes;
    use darling::FromMeta;
    use syn::Expr;

    #[test]
    fn test_case() {
        let expr: Expr = syn::parse_str(
            r#"
            (name = "specifiedNameIndex", columns = ["multiIndex1", "multiIndex2", "multiIndex3"])
        "#,
        ).unwrap();

        match MultiIndexes::from_expr(&expr) {
            Ok(multi_indexes) => println!("{:?}", multi_indexes),
            Err(err) => eprintln!("Error: {}", err),
        }
    }
}
