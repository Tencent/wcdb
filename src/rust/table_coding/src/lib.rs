use proc_macro::TokenStream;
use proc_macro2::Span;
use quote::{quote, ToTokens};
use std::fmt::Debug;
use syn::parse::Parse;
use syn::{parse_macro_input, Data, DeriveInput, Ident, Type};

#[proc_macro_derive(WCDBTableCoding, attributes(WCDBCodingParam, WCDBField))]
pub fn wcdb_table_coding(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    let fields = match &input.data {
        Data::Struct(data) => &data.fields,
        _ => panic!("WCDBTableCoding can only be used on structs"),
    };
    let table_ident = &input.ident;
    let db_table_ident = Ident::new(&format!("Db{}", table_ident), Span::call_site());
    let field_name_vec: Vec<_> = fields.iter().map(|field| &field.ident).collect();
    let field_type_vec: Vec<_> = fields.iter().map(|field| &field.ty).collect();
    let global_singleton = generate_singleton(&db_table_ident, &field_name_vec, &field_type_vec);

    let quote = quote! {
        #global_singleton
        
        impl Default for #table_ident {
            fn default() -> Self {
                Self {
                    #(#field_name_vec: Default::default()),*
                }
            }
        }

        pub struct #db_table_ident {
            #(pub #field_name_vec: *const wcdb_core::orm::field::Field<#table_ident>),*
        }

        impl Default for #db_table_ident {
            fn default() -> Self {
                Self {
                    #(#field_name_vec: std::ptr::null()),*
                }
            }
        }

        unsafe impl Send for #db_table_ident {}
        unsafe impl Sync for #db_table_ident {}
    };
    quote.into()
}

fn generate_singleton(db_table_ident: &Ident, field_name_vec: &Vec<&Option<Ident>>, field_type_vec: &Vec<&Type>) -> proc_macro2::TokenStream {
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
