use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::multi_indexes_info::MultiIndexesInfo;
use crate::compiler::resolved_info::multi_primary_info::MultiPrimaryInfo;
use crate::compiler::resolved_info::multi_unique_info::MultiUniqueInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use proc_macro2::{Ident, Span};
use quote::quote;
use std::collections::HashMap;
use std::fmt::format;
use syn::DeriveInput;
use wcdb_core::winq::column::Column;
use wcdb_core::winq::table_constraint::TableConstraint;

pub struct RustCodeGenerator {
    package_name: String,
    class_name: String,
    orm_class_name: String, //DB+StructName
    table_constraint_info: Option<TableConfigInfo>,
    all_column_info: Vec<ColumnInfo>,
}

impl RustCodeGenerator {
    pub fn new() -> Self {
        RustCodeGenerator {
            package_name: "".to_string(),
            class_name: "".to_string(),
            orm_class_name: "".to_string(),
            table_constraint_info: None,
            all_column_info: vec![],
        }
    }

    pub fn set_package_name(&mut self, package_name: String) {
        self.package_name = package_name;
    }

    pub fn set_class_name(&mut self, class_name: String) {
        self.class_name = class_name;
    }

    pub fn set_orm_class_name(&mut self, orm_class_name: String) {
        self.orm_class_name = orm_class_name;
    }

    pub fn set_table_constraint_info(&mut self, table_constraint_info: Option<TableConfigInfo>) {
        self.table_constraint_info = table_constraint_info;
    }

    pub fn set_all_column_info(&mut self, all_column_info: Vec<ColumnInfo>) {
        self.all_column_info = all_column_info;
    }

    pub fn generate_fields(&self) -> syn::Result<proc_macro2::TokenStream> {
        let mut token_stream = proc_macro2::TokenStream::new();
        let class_name = &self.class_name;
        for column_info in &self.all_column_info {
            let name = &column_info.property_name();
            // name: id ， class_name: ProcessorTest
            let stream_tmp: proc_macro2::TokenStream = quote! {
                pub #name: Field<#class_name>,
            }
            .into();
            token_stream.extend(stream_tmp);
        }
        Ok(token_stream)
    }

    pub fn generate_table_config(
        &self,
        binding_ident: &Ident,
    ) -> syn::Result<proc_macro2::TokenStream> {
        let mut all_columns_map: HashMap<String, ColumnInfo> = HashMap::new();
        for column_info in &self.all_column_info {
            let key;
            if column_info.column_name().is_empty() {
                key = column_info.property_name();
            } else {
                key = column_info.column_name();
            }
            all_columns_map.insert(key, column_info.clone());
        }

        let mut token_stream = proc_macro2::TokenStream::new();

        match &self.table_constraint_info {
            None => {}
            Some(table_config_info) => {
                match table_config_info.multi_indexes() {
                    None => {}
                    Some(multi_indexes) => {
                        for multi_index in multi_indexes {
                            let index_name_ident: Ident = multi_index.get_index_name_ident();
                            let index_column_name_ident_vec: Vec<Ident> =
                                multi_index.get_index_column_name_ident_vec(&all_columns_map);
                            let mut is_full_name = true;
                            if multi_index.name().is_empty() {
                                is_full_name = false;
                            }
                            token_stream.extend(quote! {
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
                    }
                }

                match table_config_info.multi_primaries() {
                    None => {}
                    Some(multi_primaries) => {
                        for primaries in multi_primaries {
                            let ident_vec: Vec<Ident> =
                                primaries.columns_ident_vec(&all_columns_map);
                            token_stream.extend(quote::quote! {
                                let table_constraint = wcdb_core::winq::table_constraint::TableConstraint::new();
                                table_constraint.primary_key();
                                table_constraint.indexed_by(
                                    unsafe {vec![
                                        #(
                                            (*instance.#ident_vec).get_column(),
                                        )*
                                    ]}
                                );
                                #binding_ident.add_table_constraint(table_constraint);
                            });
                        }
                    }
                }

                match table_config_info.multi_unique() {
                    None => {}
                    Some(multi_unique_vec) => {
                        for uniques in multi_unique_vec {
                            let ident_vec: Vec<Ident> = uniques.columns_ident_vec(&all_columns_map);
                            token_stream.extend(quote::quote! {
                                let table_constraint = wcdb_core::winq::table_constraint::TableConstraint::new();
                                table_constraint.unique();
                                table_constraint.indexed_by(
                                    unsafe {vec![
                                        #(
                                            (*instance.#ident_vec).get_column(),
                                        )*
                                    ]}
                                );
                                #binding_ident.add_table_constraint(table_constraint);
                            });
                        }
                    }
                }

                if table_config_info.is_without_row_id() {
                    token_stream.extend(quote::quote! {
                        #binding_ident.config_without_row_id();
                    });
                }

                match table_config_info.fts_module() {
                    None => {
                        return Ok(token_stream);
                    }
                    Some(module_info) => {
                        if module_info.fts_version().is_empty() {
                            return Ok(token_stream);
                        }

                        let version = module_info.fts_version();
                        token_stream.extend(quote::quote! {
                            #binding_ident.config_virtual_module(#version.as_str());
                        });

                        let parameter = module_info.tokenizer_parameters().join(" ");
                        let tokenizer =
                            format!("{}{}{}", "tokenize = ", module_info.tokenizer(), parameter);
                        token_stream.extend(quote::quote! {
                            #binding_ident.config_virtual_module_argument(#tokenizer.as_str());
                        });

                        if (!module_info.external_table().is_empty()) {
                            let content =
                                format!("{}{}{}", "content='", module_info.external_table(), "'");
                            token_stream.extend(quote::quote! {
                                #binding_ident.config_virtual_module_argument(#content.as_str());
                            });
                        }
                    }
                }
            }
        }
        Ok(token_stream.clone())
    }
}
