use proc_macro::TokenStream;
use quote::quote;
use syn::spanned::Spanned;
use syn::Data::Struct;
use syn::{parse_macro_input, DeriveInput, Ident};

#[proc_macro_derive(TableBinding, attributes(WCDBTableCoding, WCDBField))]
pub fn table_binding(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    let table_name_string = input.ident.to_string();
    let db_table_name = Ident::new(&format!("Db{}", table_name_string), table_name_string.span());
    let field_vec = extract_fields(&input);

    let ret = quote! {
        pub struct #db_table_name {
            #(#field_vec),*
        }
    };
    ret.into()
}

fn extract_fields(input: &DeriveInput) -> Vec<proc_macro2::TokenStream> {
    let table_name = &input.ident;
    match &input.data {
        Struct(data) => data
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
