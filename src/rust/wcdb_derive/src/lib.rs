#![feature(proc_macro_quote)]

mod compiler;
mod macros;

use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use crate::compiler::rust_code_generator::RustCodeGenerator;
use crate::compiler::rust_field_orm_info::RUST_FIELD_ORM_INFO_MAP;
use crate::macros::wcdb_field::WCDBField;
use crate::macros::wcdb_table::WCDBTable;
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
    check_field_element(&table, &all_column_info);
    check_field_default(&all_column_info);
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

fn check_field_element(table: &WCDBTable, all_column_info: &Vec<ColumnInfo>) {
    let column_type_vec: Vec<&String> = RUST_FIELD_ORM_INFO_MAP.keys().collect();
    let mut primary_key_count = 0;
    for column_info in all_column_info {
        let has_contain = column_type_vec
            .iter()
            .any(|x| *x == column_info.property_type().as_str());
        if !has_contain {
            panic!(
                "The type {} of field {} in {} is Unsupported!",
                column_info.property_type(),
                column_info.property_name(),
                table.get_struct_name()
            );
        }

        let field_key = column_info.property_name();
        let field_key = field_key.as_str();
        if column_info.is_primary() {
            primary_key_count += 1;
            if primary_key_count > 1 {
                panic!("#[WCDBField] can only configure one primary key for \"{}\". If multiple primary keys are required, configure multiPrimaries in #[WCDBTableCoding]. ", field_key)
            }
            if column_info.is_auto_increment() {
                let field_orm_info = column_info.get_field_orm_info();
                if field_orm_info.column_type != "Integer" {
                    panic!(
                        "#[WCDBField] Auto-increment field must be integer for \"{}\".",
                        field_key
                    );
                }
            }
            if column_info.has_index() {
                panic!("Restricted to primary key, so no @WCDBIndex configuration is required.field_key:  \"{}\".", field_key);
            }
        } else if column_info.is_auto_increment() {
            panic!(
                "#[WCDBField] Auto-increment field must be primary key for \"{}\".",
                field_key
            );
        }

        let field_orm_info = column_info.get_field_orm_info();
        if column_info.is_not_null() && field_orm_info.nullable {
            panic!(
                "#[WCDBField] Not null field cannot support \"{}: {}\".",
                field_key,
                column_info.property_type(),
            );
        }
    }
}

fn check_field_default(all_column_info: &Vec<ColumnInfo>) {
    for column_info in all_column_info {
        let mut value_count = 0;
        let mut type_miss_match = false;
        let field_orm_info = column_info.get_field_orm_info();
        let column_type = field_orm_info.column_type.clone();
        let default_opt = column_info.default_value();
        if default_opt.is_none() {
            continue;
        }
        let default = default_opt.clone().unwrap();
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
            panic!(
                "Only one default value can be configured for a field. \"{}\".",
                column_info.property_name()
            );
        } else if type_miss_match {
            if column_type != "BLOB" {
                panic!(
                    "Assigning a default value to BLOB is unsupported. \"{}\".",
                    column_info.property_name()
                );
            } else {
                panic!("Default value should be a \"{}\".", column_type);
            }
        }
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
