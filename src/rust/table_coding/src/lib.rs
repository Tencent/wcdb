#![feature(proc_macro_quote)]

mod compiler;
mod macros;

use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::compiler::resolved_info::default_value_info::DefaultValueInfo;
use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use crate::compiler::rust_code_generator::RustCodeGenerator;
use crate::compiler::rust_field_orm_info::RUST_FIELD_ORM_INFO_MAP;
use crate::macros::wcdb_field::WCDBField;
use crate::macros::wcdb_table::WCDBTable;
use darling::ast::Data;
use darling::{FromDeriveInput, FromField, FromMeta};
use proc_macro::TokenStream;
use quote::ToTokens;
use std::any::Any;
use std::collections::HashSet;
use std::fmt::Debug;
use syn::parse::Parse;
use syn::spanned::Spanned;
use syn::{parse_macro_input, DeriveInput};

#[proc_macro_derive(WCDBTableCoding, attributes(WCDBTable, WCDBField))]
pub fn process(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    check_class_element(&input);

    let table = WCDBTable::from_derive_input(&input).unwrap();

    check_fts_module(&table);

    let table_constraint_info = TableConfigInfo::resolve(
        &table,
        Some(FTSModuleInfo::new()), //TODO dengxudong fts module
    );
    let all_column_info = table.get_all_column_info();

    check_field_element(&table);

    check_column_in_table_constraint(&table_constraint_info, &all_column_info);

    match create_orm_file(&table, table_constraint_info, all_column_info) {
        Ok(quote) => quote.into(),
        Err(e) => e.to_compile_error().into(),
    }
}

fn create_orm_file(
    table: &WCDBTable,
    table_constraint_info: TableConfigInfo,
    all_column_info: Vec<ColumnInfo>,
) -> syn::Result<proc_macro2::TokenStream> {
    let mut code_gen = RustCodeGenerator::new();
    code_gen.set_class_name(table.get_struct_name());
    code_gen.set_orm_class_name(table.get_db_table_name());
    code_gen.set_table_constraint_info(Option::from(table_constraint_info));
    code_gen.set_all_column_info(all_column_info);

    code_gen.generate(&table)
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

fn check_fts_module(table: &WCDBTable) {
    // todo qixinbing
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
                check_field_not_null(field, &field_key);
                for field in &fields.fields {
                    let mut value_count = 0;
                    let mut type_miss_match = false;
                    let property_type = WCDBField::get_property_type(&field.ty()).unwrap_or(String::from("None"));
                    let field_orm_info_opt = RUST_FIELD_ORM_INFO_MAP.get(property_type.as_str());
                    assert!(field_orm_info_opt.is_some(), "filed not support {}",property_type.as_str());
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

fn check_column_in_table_constraint(
    table_config_info: &TableConfigInfo,
    all_column_info: &Vec<ColumnInfo>,
) {
    if table_config_info.multi_indexes_is_empty()
        && table_config_info.multi_primaries_is_empty()
        && table_config_info.multi_unique_is_empty()
    {
        return;
    }

    let mut all_columns: HashSet<String> = HashSet::new();
    for column_info in all_column_info {
        let name = if column_info.column_name().is_empty() {
            column_info.property_name()
        } else {
            column_info.column_name()
        };
        all_columns.insert(name);
    }

    if let Some(indexes_info_vec) = table_config_info.multi_indexes() {
        for indexes_info in indexes_info_vec {
            for str in indexes_info.columns() {
                if !all_columns.contains(str) {
                    panic!(
                        "Can't find column \"{}\" in class orm multi-index config.",
                        str
                    );
                }
            }
        }
    }

    if let Some(multi_primary_vec) = table_config_info.multi_primaries() {
        for primary_info in multi_primary_vec {
            for name in primary_info.columns() {
                if !all_columns.contains(name) {
                    panic!(
                        "Can't find column \"{}\" in class orm multi-primaries config.",
                        name
                    );
                }
            }
        }
    }

    if let Some(multi_unique_vec) = table_config_info.multi_unique() {
        for unique_info in multi_unique_vec {
            for name in unique_info.columns() {
                if !all_columns.contains(name) {
                    panic!(
                        "Can't find column \"{}\" in class orm multi-unique config.",
                        name
                    );
                }
            }
        }
    }
}

fn check_field_not_null(field: &WCDBField, field_key: &Option<String>) {
    let column_info = ColumnInfo::resolve(field);
    let property_type = column_info.property_type();
    let field_orm_info_opt = RUST_FIELD_ORM_INFO_MAP.get(property_type.as_str());
    assert!(
        field_orm_info_opt.is_some(),
        "filed not support {}",
        property_type.as_str()
    );
    let field_orm_info = field_orm_info_opt.unwrap();
    if column_info.is_not_null() && field_orm_info.nullable {
        panic!(
            "#[WCDBField] Not null field cannot support \"{}: {}\".",
            field_key.clone().unwrap(),
            property_type.as_str(),
        );
    }
}
