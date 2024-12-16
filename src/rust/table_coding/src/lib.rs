use proc_macro::TokenStream;
use quote::{quote, ToTokens};
use std::fmt::Debug;
use syn::parse::Parse;
use syn::spanned::Spanned;
use syn::{parse_macro_input, Data, DeriveInput, Ident, LitBool};

#[proc_macro_derive(WCDBTableCoding, attributes(WCDBTableCodingParam, WCDBField))]
pub fn wcdb_table_coding(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    let table_name = input.ident.to_string();
    let db_table = Ident::new(&format!("Db{}", table_name), input.ident.span());
    let field_vec = extract_fields(&input);

    let global_singleton = generate_singleton(&input, &db_table);

    let ret = quote! {
        #global_singleton

        pub struct #db_table {
            #(#field_vec),*
        }

        unsafe impl Send for #db_table {}
        unsafe impl Sync for #db_table {}
    };
    ret.into()
}

fn extract_fields(input: &DeriveInput) -> Vec<proc_macro2::TokenStream> {
    let table_name = &input.ident;
    match &input.data {
        Data::Struct(data) => data
            .fields
            .iter()
            .filter_map(|field| {
                let has_wcdb_field = field
                    .attrs
                    .iter()
                    .any(|attr| attr.path().is_ident("WCDBField"));
                if has_wcdb_field {
                    let field_name = field.ident.as_ref().unwrap();
                    Some(quote! {
                        pub #field_name: wcdb_core::orm::field::Field<#table_name>
                    })
                } else {
                    None
                }
            })
            .collect::<Vec<_>>(),
        _ => {
            let err = syn::Error::new_spanned(input, "TableBinding only used for struct!");
            vec![err.to_compile_error()]
        }
    }
}

fn generate_singleton(input: &DeriveInput, db_table: &Ident) -> proc_macro2::TokenStream {
    let binding = Ident::new(
        &format!("{}_BINDING", db_table.to_string().to_uppercase()),
        input.ident.span(),
    );
    let instance = Ident::new(
        &format!("{}_INSTANCE", db_table.to_string().to_uppercase()),
        input.ident.span(),
    );
    quote! {
        static #binding: once_cell::sync::Lazy<wcdb_core::orm::binding::Binding> = once_cell::sync::Lazy::new(|| {
            wcdb_core::orm::binding::Binding::new()
        });
        // static #instance: once_cell::sync::Lazy<#db_table> = once_cell::sync::Lazy::new(|| {
        //
        // });
    }
}

fn get_attr<T: std::str::FromStr>(
    input: &DeriveInput,
    field_name: &str,
    attr_name: &str,
) -> Option<T> {
    if let Data::Struct(data_struct) = &input.data {
        for field in &data_struct.fields {
            if let Some(ident) = &field.ident {
                if ident == field_name {
                    for attr in &field.attrs {
                        if attr.meta.path().is_ident("WCDBField") {
                            println!("{:#?}", attr.meta);
                            let meta_list = attr.meta.require_list().unwrap();
                            println!("Tokens: {:#?}", meta_list.tokens);
                            match meta_list.parse_nested_meta(|meta| {
                                println!("{:#?}", meta.path);
                                if meta.path.is_ident(attr_name) {
                                    let value = meta.value().unwrap();
                                    let s: LitBool = value.parse().unwrap();
                                    println!("113 {:#?}", s);
                                }
                                Ok(())
                            }) {
                                Ok(_) => {
                                    println!("3333");
                                }
                                Err(e) => {
                                    println!("{:?}", e);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    None
}
