#![feature(proc_macro_quote)]

mod compiler;
mod field_orm_info;
mod macros;

use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use crate::compiler::rust_code_generator::RustCodeGenerator;
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
    let field_type_vec = table.get_field_type_vec();

    check_field_element(table);

    let mut code_gen = RustCodeGenerator::new();
    code_gen.set_class_name(table_ident.to_string());
    code_gen.set_orm_class_name(db_table_ident.to_string());
    code_gen.set_table_constraint_info(Option::from(TableConfigInfo::resolve(
        table,
        Some(FTSModuleInfo::new()), //TODO dengxudong fts module
    )));
    match table.data() {
        Data::Enum(_) => {}
        Data::Struct(fields) => {
            let mut all_column_info: Vec<ColumnInfo> = vec![];
            for field in &fields.fields {
                all_column_info.push(ColumnInfo::resolve(&field));
            }
            code_gen.set_all_column_info(all_column_info);
        }
    }

    let singleton_statements = code_gen.generate_singleton(&table)?;
    let generate_binding_type = code_gen.generate_binding_type(&table_ident)?;
    let generate_binding_fields =
        code_gen.generate_binding_fields(&table_ident, &field_ident_vec)?;
    let generate_base_binding = code_gen.generate_base_binding(&binding_ident)?;
    let generate_extract_object =
        code_gen.generate_extract_object(&table_ident, &field_ident_vec, &field_type_vec)?;
    let generate_bind_object =
        code_gen.generate_bind_object(&table_ident, &field_ident_vec, &field_type_vec)?;

    let auto_increment_field_opt = table.get_auto_increment_ident_field();
    let generate_auto_increment_config =
        code_gen.generate_auto_increment_config(&table_ident, &auto_increment_field_opt)?;

    Ok(quote::quote! {
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
            #generate_binding_type

            #generate_binding_fields

            #generate_base_binding

            #generate_extract_object

            #generate_bind_object

            #generate_auto_increment_config
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
