#![feature(proc_macro_quote)]

mod compiler;
mod field_orm_info;
mod macros;

use crate::field_orm_info::FIELD_ORM_INFO_MAP;
use crate::macros::wcdb_field::WCDBField;
use crate::macros::wcdb_table::WCDBTable;
use darling::ast::Data;
use darling::{FromDeriveInput, FromField, FromMeta};
use proc_macro::TokenStream;
use proc_macro2::Span;
use quote::{quote, ToTokens};
use std::fmt::Debug;
use syn::parse::Parse;
use syn::spanned::Spanned;
use syn::{parse_macro_input, DeriveInput, Ident};

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
    let table_ident = table.ident();
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    let field_ident_vec = table.get_field_ident_vec();

    check_field_element(table);
    let singleton_statements = generate_singleton(&table)?;
    let extract_object_statements = generate_extract_object(&table)?;
    let bind_field_statements = generate_bind_field(&table)?;
    let auto_increment_statements = generate_auto_increment(&table)?;

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
                fields: &Vec<&wcdb_core::orm::field::Field<#table_ident>>,
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

            #auto_increment_statements
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

macro_rules! match_field_info {
    ($field_type_string:expr, $field:expr, $getter_name:ident) => {
        match FIELD_ORM_INFO_MAP.get(&$field_type_string) {
            Some(value) => value.$getter_name.clone(),
            None => {
                return Err(syn::Error::new(
                    $field.span(),
                    "Unsupported field type for bind_field",
                ))
            }
        }
    };
}
macro_rules! get_field_info_vec {
    ($field_type_vec:expr, $field_getter:ident) => {
        $field_type_vec
            .iter()
            .map(|field| {
                let field_type_string = WCDBField::get_field_type_string(field)?;
                let type_string = match_field_info!(field_type_string, field, $field_getter);
                Ok(Ident::new(&type_string, Span::call_site()))
            })
            .collect::<syn::Result<Vec<_>>>()?
    };
}

fn check_field_element(table: &WCDBTable) {
    let mut primary_key_count = 0;
    match &table.data() {
        Data::Struct(fields) => fields
            .iter()
            .map(|field| {
                let field_key = field.ident().span().source_text();
                if field.is_primary() {
                    primary_key_count += 1;
                    if primary_key_count > 1 {
                        panic!("#[WCDBField] can only configure one primary key for \"{}\". If multiple primary keys are required, configure multiPrimaries in #[WCDBTableCoding]. ",field_key.unwrap())
                    }

                    if field.is_auto_increment() {
                        if !field.is_integer() {
                            panic!("#[WCDBField] Auto-increment field must be integer for \"{}\".", field_key.unwrap());
                        }
                    }
                    // todo qixinbing check  @WCDBIndex
                }else if field.is_auto_increment() {
                    panic!("#[WCDBField] Auto-increment field must be primary key for \"{}\".", field_key.unwrap());
                }
            })
            .collect(),
        _ => panic!("WCDBTable only works on structs"),
    }
}

fn generate_singleton(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());

    let columns_statements = generate_columns(table)?;
    let table_config_statements = generate_table_config(table, &binding_ident)?;

    Ok(quote! {
        pub static #binding_ident: once_cell::sync::Lazy<wcdb_core::orm::binding::Binding> = once_cell::sync::Lazy::new(|| {
            wcdb_core::orm::binding::Binding::new()
        });
        pub static #instance_ident: once_cell::sync::Lazy<#db_table_ident> = once_cell::sync::Lazy::new(|| {
            let mut instance = #db_table_ident::default();
            let instance_raw = unsafe { &instance as *const #db_table_ident };

            #columns_statements

            #table_config_statements

            instance
        });
    })
}

fn generate_extract_object(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident();
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_get_type_vec: Vec<_> = get_field_info_vec!(field_type_vec, field_getter);
    let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();
    Ok(quote! {
        let mut new_one = #table_ident::default();
        let mut index = 0;
        for field in fields {
            match field.get_field_id() {
                #(
                    #field_id_vec => new_one.#field_ident_vec = prepared_statement.#field_get_type_vec(index),
                )*
                _ => panic!("Unknown field id"),
            }
            index += 1;
        }
        new_one
    })
}

fn generate_bind_field(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident();
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();
    let field_bind_type_vec: Vec<_> = field_type_vec
        .iter()
        .map(|field| {
            let field_type_string = WCDBField::get_field_type_string(field)?;
            let bind_type_string = match_field_info!(field_type_string, field, field_setter);
            Ok(Ident::new(&bind_type_string, Span::call_site()))
        })
        .collect::<syn::Result<Vec<_>>>()?;
    let as_ref_vec: Vec<_> = field_bind_type_vec
        .iter()
        .map(|bind_type| {
            if &bind_type.to_string() == "bind_text" {
                quote!(.as_ref())
            } else {
                quote!()
            }
        })
        .collect();
    Ok(quote! {
        match field.get_field_id() {
            #(
                #field_id_vec => prepared_statement.#field_bind_type_vec(object.#field_ident_vec #as_ref_vec, index),
            )*
            _ => panic!(
                "Invalid id {} of field {} in {}",
                field.get_field_id(),
                wcdb_core::winq::identifier::IdentifierTrait::get_description(field),
                stringify!(#table_ident)
            ),
        }
    })
}

fn generate_auto_increment(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident();
    let auto_increment_field_opt = table.get_auto_increment_ident_field();
    match auto_increment_field_opt {
        None => Ok(quote! {
            fn is_auto_increment(&self, object: &#table_ident) -> bool {
                false
            }

            fn set_last_insert_row_id(&self, object: &mut #table_ident, last_insert_row_id: i64) {

            }
        }),
        Some(field) => {
            let field_ident = field.ident().clone().unwrap();
            let field_type_ident = field.get_field_type_ident();
            Ok(quote! {
                fn is_auto_increment(&self, object: &#table_ident) -> bool {
                    object.#field_ident == 0
                }

                fn set_last_insert_row_id(&self, object: &mut #table_ident, last_insert_row_id: i64) {
                    object.#field_ident = last_insert_row_id as #field_type_ident
                }
            })
        }
    }
}

fn generate_enable_auto_increment_for_existing_table(
    table: &WCDBTable,
    binding_ident: &Ident,
) -> syn::Result<proc_macro2::TokenStream> {
    let enable_auto_increment_for_existing_table =
        table.get_enable_auto_increment_for_existing_table();
    let enable_auto_increment_for_existing_table_statements =
        if enable_auto_increment_for_existing_table {
            quote! {
                #binding_ident.enable_auto_increment_for_existing_table();
            }
        } else {
            quote! {}
        };
    Ok(enable_auto_increment_for_existing_table_statements)
}

fn generate_columns(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());

    let field_vec = table.get_field_vec();
    let columns_statements = if field_vec.is_empty() {
        quote! {}
    } else {
        let mut token_stream = proc_macro2::TokenStream::new();
        let mut field_id: usize = 1;
        for field in field_vec {
            let column_name_ident = field.get_column_name_ident();
            let field_ident = field.get_field_ident();
            let field_def_ident = Ident::new(
                &format!("{}_def", field_ident.to_string()),
                Span::call_site(),
            );

            let is_primary_key = field.is_primary();
            let is_auto_increment = field.is_auto_increment();
            let column_type_ident = field.get_column_type_ident()?;

            token_stream.extend(quote! {
                let field = Box::new(wcdb_core::orm::field::Field::new(
                    stringify!(#column_name_ident),
                    instance_raw,
                    #field_id,
                    #is_auto_increment,
                    #is_primary_key
                ));
            });

            field_id += 1;

            token_stream.extend(quote! {
                let #field_def_ident = wcdb_core::winq::column_def::ColumnDef::new_with_column_type(
                    &field.get_column(),
                    wcdb_core::winq::column_type::ColumnType::#column_type_ident
                );
            });

            if is_primary_key {
                token_stream.extend(quote! {
                    let column_constraint = wcdb_core::winq::column_constraint::ColumnConstraint::new();
                    column_constraint.primary_key();
                });
                if is_auto_increment {
                    token_stream.extend(quote! {
                        column_constraint.auto_increment();
                    });
                }
                token_stream.extend(quote! {
                    #field_def_ident.constraint(column_constraint);
                })
            }

            token_stream.extend(quote! {
                instance.#field_ident = unsafe { Box::into_raw(field) };
                #binding_ident.add_column_def(#field_def_ident);
            });

            if table.get_enable_auto_increment_for_existing_table() {
                token_stream.extend(quote! {
                    #binding_ident.enable_auto_increment_for_existing_table();
                });
            }
        }
        token_stream
    };

    Ok(columns_statements)
}

fn generate_table_config(
    table: &WCDBTable,
    binding_ident: &Ident,
) -> syn::Result<proc_macro2::TokenStream> {
    let all_field_info_vec = table.get_all_column_info_vec();

    let multi_index_vec = table.get_multi_index_vec();
    let multi_index_statements = if multi_index_vec.is_empty() {
        quote! {}
    } else {
        let mut code = proc_macro2::TokenStream::new();

        for multi_index in multi_index_vec {
            let index_name_ident: Ident = multi_index.get_index_name_ident();
            let index_column_name_ident_vec: Vec<Ident> =
                multi_index.get_index_column_name_ident_vec(&all_field_info_vec);
            let is_full_name = multi_index.get_is_full_name();

            code.extend(quote! {
                let create_index = wcdb_core::winq::statement_create_index::StatementCreateIndex::new();
                create_index.if_not_exist();
                create_index.indexed_by(
                    unsafe {vec![
                        #(
                            (*instance.#index_column_name_ident_vec).get_column(),
                        )*
                    ]});
                #binding_ident.add_index(stringify!(#index_name_ident), #is_full_name, create_index);
            });
        }
        code
    };

    Ok(quote! {
        #multi_index_statements
    })
}
