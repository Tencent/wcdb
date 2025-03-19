#![feature(proc_macro_quote)]

mod compiler;
mod field_orm_info;
mod macros;

use crate::compiler::resolved_info::default_value_info::DefaultValueInfo;
use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use crate::compiler::rust_code_generator::RustCodeGenerator;
use crate::field_orm_info::FIELD_ORM_INFO_MAP;
use crate::macros::wcdb_field::WCDBField;
use crate::macros::wcdb_table::WCDBTable;
use darling::ast::Data;
use darling::{FromDeriveInput, FromField, FromMeta};
use proc_macro::TokenStream;
use proc_macro2::Span;
use quote::ToTokens;
use std::any::Any;
use std::fmt::Debug;
use syn::parse::Parse;
use syn::spanned::Spanned;
use syn::{parse_macro_input, DeriveInput, Ident};

#[proc_macro_derive(WCDBTableCoding, attributes(WCDBTable, WCDBField))]
pub fn process(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    check_class_element(&input);
    let table = WCDBTable::from_derive_input(&input).unwrap();
    match create_orm_file(&table) {
        Ok(quote) => quote.into(),
        Err(e) => e.to_compile_error().into(),
    }
}

fn create_orm_file(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = table.ident();
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    let field_ident_vec = table.get_field_ident_vec();

    check_field_element(table);

    let mut code_gen = RustCodeGenerator::new();
    code_gen.set_class_name(table_ident.to_string());
    code_gen.set_orm_class_name(db_table_ident.to_string());
    code_gen.set_table_constraint_info(Option::from(TableConfigInfo::resolve(
        table,
        Some(FTSModuleInfo::new()), //TODO dengxudong fts module
    )));
    code_gen.set_all_column_info(table.data());
    code_gen.check_column_in_table_constraint();

    let singleton_statements = code_gen.generate_singleton(&table)?;
    let generate_binding_type = code_gen.generate_binding_type(&table_ident)?;
    let generate_binding_fields =
        code_gen.generate_binding_fields(&table_ident, &field_ident_vec)?;
    let generate_base_binding = code_gen.generate_base_binding(&binding_ident)?;
    let generate_extract_object = code_gen.generate_extract_object(&table_ident)?;
    let generate_bind_object = code_gen.generate_bind_object(&table_ident)?;

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

fn check_class_element(input: &DeriveInput) {
    let is_struct = match &input.data {
        syn::Data::Struct(_) => true,
        syn::Data::Enum(_) => false,
        _ => false,
    };
    if !is_struct {
        panic!("@WCDBTableCoding is only valid for structure");
    }

    let vis_str = input.vis.to_token_stream().to_string();
    if vis_str != "pub" {
        panic!(
            "The visibility of the structure with @WCDBTableCoding macro definition must be pub"
        );
    }
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
                        panic!("#[WCDBField] can only configure one primary key for \"{}\". If multiple primary keys are required, configure multiPrimaries in #[WCDBTableCoding]. ", field_key.unwrap())
                    }

                    if field.is_auto_increment() {
                        if !field.is_integer() {
                            panic!("#[WCDBField] Auto-increment field must be integer for \"{}\".", field_key.unwrap());
                        }
                    }

                    match field.attr() {
                        None => {}
                        Some(attr) => {
                            match attr.index() {
                                None => {}
                                Some(_) => {
                                    panic!("Restricted to primary key, so no @WCDBIndex configuration is required.field_key:  \"{}\".", field_key.unwrap());
                                }
                            }
                        }
                    }
                } else if field.is_auto_increment() {
                    panic!("#[WCDBField] Auto-increment field must be primary key for \"{}\".", field_key.unwrap());
                }
                for field in &fields.fields {
                    let mut value_count = 0;
                    let mut type_miss_match = false;
                    let property_type = WCDBField::get_property_type(&field.ty()).unwrap_or(String::from("None"));
                    let field_orm_info_opt = FIELD_ORM_INFO_MAP.get(property_type.as_str());
                    assert!(field_orm_info_opt.is_some());
                    let column_type = field_orm_info_opt.unwrap().column_type.clone();

                    let default_opt = DefaultValueInfo::resolve(&field.attr());
                    if default_opt.is_none() {
                        continue;
                    }
                    let default = default_opt.unwrap();
                    if default.i32_value() != 0 {
                        value_count = value_count + 1;
                        if column_type != "Integer" {
                            type_miss_match = true;
                        }
                    }
                    if default.f64_value() != 0f64 {
                        value_count = value_count + 1;
                        if column_type != "Float" {
                            type_miss_match = true;
                        }
                    }
                    if !default.text_value().is_empty() {
                        value_count = value_count + 1;
                        if column_type != "Text" {
                            type_miss_match = true;
                        }
                    }
                    if value_count > 1 {
                        panic!("Only one default value can be configured for a field. \"{}\".", field_key.unwrap());
                    } else if type_miss_match {
                        if column_type != "BLOB" {
                            panic!("Assigning a default value to BLOB is unsupported. \"{}\".", field_key.unwrap());
                        }else {
                            panic!("Default value should be a \"{}\".", column_type);
                        }
                    }
                }
            })
            .collect(),
        _ => panic!("WCDBTable only works on structs"),
    }
}
