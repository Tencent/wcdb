use darling::ast::Data;
use darling::{FromDeriveInput, FromField, FromMeta};
use proc_macro::TokenStream;
use proc_macro2::Span;
use quote::{quote, ToTokens};
use std::fmt::Debug;
use syn::parse::Parse;
use syn::spanned::Spanned;
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
            Data::Struct(fields) => fields
                .iter()
                .map(|field| field.ident.as_ref().unwrap())
                .collect(),
            _ => unreachable!("WCDBTable only works on structs"),
        }
    }

    fn get_field_type_vec(&self) -> Vec<&Type> {
        match &self.data {
            Data::Struct(fields) => fields.iter().map(|field| &field.ty).collect(),
            _ => unreachable!("WCDBTable only works on structs"),
        }
    }
}

fn get_type_string(ty: &Type) -> syn::Result<String> {
    if let Type::Path(type_path) = ty {
        if type_path.path.is_ident("i32") {
            Ok("get_int".to_string())
        } else {
            Err(syn::Error::new(ty.span(), "Unsupported field type"))
        }
    } else {
        Err(syn::Error::new(
            ty.span(),
            "WCDBTable's field type only works on Path",
        ))
    }
}

fn bind_type_string(ty: &Type) -> syn::Result<String> {
    if let Type::Path(type_path) = ty {
        if type_path.path.is_ident("i32") {
            Ok("bind_int".to_string())
        } else if type_path.path.is_ident("i64") {
            Ok("bind_integer".to_string())
        } else {
            Err(syn::Error::new(ty.span(), "Unsupported field type"))
        }
    } else {
        Err(syn::Error::new(
            ty.span(),
            "WCDBTable's field type only works on Path",
        ))
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
    match do_expand(&table) {
        Ok(quote) => quote.into(),
        Err(e) => e.to_compile_error().into(),
    }
}

fn do_expand(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident;
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    let field_ident_vec = table.get_field_ident_vec();

    let singleton_statements = generate_singleton(&table)?;
    let extract_object_statements = generate_extract_object(&table)?;
    let bind_field_statements = generate_bind_field(&table)?;

    Ok(quote! {
        #singleton_statements

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

        impl Drop for #db_table_ident {
            fn drop(&mut self) {
                unsafe {
                    #(
                        if !self.#field_ident_vec.is_null() {
                            Box::from_raw(self.#field_ident_vec as *mut wcdb_core::orm::field::Field<#table_ident>);
                        }
                    )*
                }
            }
        }

        unsafe impl Send for #db_table_ident {}
        unsafe impl Sync for #db_table_ident {}

        impl wcdb_core::orm::table_binding::TableBinding<#table_ident> for #db_table_ident {
            fn binding_type(&self) -> std::any::TypeId {
                std::any::TypeId::of::<#table_ident>()
            }

            fn all_binding_fields(&self) -> Vec<&wcdb_core::orm::field::Field<#table_ident>> {
                unsafe { vec![
                    #(&*self.#field_ident_vec,)*
                ] }
            }

            fn base_binding(&self) -> &wcdb_core::orm::binding::Binding {
                &*#binding_ident
            }

            fn extract_object(
                &self,
                fields: &Vec<wcdb_core::orm::field::Field<#table_ident>>,
                prepared_statement: &wcdb_core::core::prepared_statement::PreparedStatement,
            ) -> #table_ident {
                #extract_object_statements
            }

            fn bind_field(
                &self,
                object: &#table_ident,
                field: &wcdb_core::orm::field::Field<#table_ident>,
                index: usize,
                prepared_statement: &std::sync::Arc<wcdb_core::core::prepared_statement::PreparedStatement>,
            ) {
                #bind_field_statements
            }

            fn is_auto_increment(&self, object: &#table_ident) -> bool {
                false
            }

            fn set_last_insert_row_id(&self, object: &mut #table_ident, last_insert_row_id: i64) {}
        }

        impl #db_table_ident {
            pub fn all_fields() -> Vec<&'static wcdb_core::orm::field::Field<#table_ident>> {
                unsafe { vec![
                    #(&*#instance_ident.#field_ident_vec,)*
                ] }
            }
        }
    })
}

fn generate_singleton(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let db_table_ident = table.get_db_table();
    let field_ident_vec = table.get_field_ident_vec();
    let field_ident_def_vec: Vec<Ident> = field_ident_vec
        .iter()
        .map(|ident| Ident::new(&format!("{}_def", ident.to_string()), Span::call_site()))
        .collect();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    let field_id_vec: Vec<usize> = (1..=field_ident_vec.len()).collect();
    Ok(quote! {
        static #binding_ident: once_cell::sync::Lazy<wcdb_core::orm::binding::Binding> = once_cell::sync::Lazy::new(|| {
            wcdb_core::orm::binding::Binding::new()
        });
        static #instance_ident: once_cell::sync::Lazy<#db_table_ident> = once_cell::sync::Lazy::new(|| {
            let mut instance = #db_table_ident::default();
            let instance_raw = unsafe { &instance as *const #db_table_ident };
            #(
                let field = Box::new(wcdb_core::orm::field::Field::new(stringify!(#field_ident_vec), instance_raw, #field_id_vec, false, false));
                let #field_ident_def_vec = wcdb_core::winq::column_def::ColumnDef::new_with_column_type(&field.get_column(), wcdb_core::winq::column_type::ColumnType::Integer);
                instance.#field_ident_vec = unsafe { Box::into_raw(field) };
                #binding_ident.add_column_def(#field_ident_def_vec);
            )*
            instance
        });
    })
}

fn generate_extract_object(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident;
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_get_type_vec: Vec<_> = field_type_vec
        .iter()
        .map(|field| {
            let type_string = get_type_string(field)?;
            Ok(Ident::new(&type_string, Span::call_site()))
        })
        .collect::<syn::Result<Vec<_>>>()?;
    let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();
    Ok(quote! {
        let mut new_one = #table_ident::default();
        let mut index = 0;
        for field in fields {
            match field.get_field_id() {
                #(
                    #field_id_vec => new_one.#field_ident_vec = prepared_statement.#field_get_type_vec(index),
                )*
                _ => unreachable!("Unknown field id"),
            }
            index += 1;
        }
        new_one
    })
}

fn generate_bind_field(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_bind_type_vec: Vec<_> = field_type_vec
        .iter()
        .map(|field| {
            let bind_type_string = bind_type_string(field)?;
            Ok(Ident::new(&bind_type_string, Span::call_site()))
        })
        .collect::<syn::Result<Vec<_>>>()?;
    let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();
    Ok(quote! {
        match field.get_field_id() {
            #(
                #field_id_vec => prepared_statement.#field_bind_type_vec(object.#field_ident_vec, index),
            )*
            _ => unreachable!("Unknown field id"),
        }
    })
}
