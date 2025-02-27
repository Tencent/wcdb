use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::multi_indexes_info::MultiIndexesInfo;
use crate::compiler::resolved_info::multi_primary_info::MultiPrimaryInfo;
use crate::compiler::resolved_info::multi_unique_info::MultiUniqueInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use proc_macro::quote;
use proc_macro2::Ident;
use std::collections::HashMap;
use std::fmt::format;
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
        println!("bugtags.generate_fields: {0}", token_stream.to_string());
        Ok(token_stream)
    }

    pub fn generate_table_config(
        &self,
        binding_ident: &Ident,
    ) -> syn::Result<proc_macro2::TokenStream> {
        println!(
            "generate_table_config start.struct name : {0}",
            binding_ident.to_string()
        );
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
            None => {
                println!("generate_table_config: table_constraint_info is none")
            }
            Some(table_config_info) => {
                // todo dengxudong 将 multi_indexes 逻辑挪过来
                // table_config_info.multi_indexes()

                match table_config_info.multi_primaries() {
                    None => {
                        println!("generate_table_config: multi_primaries is none")
                    }
                    Some(multi_primaries) => {
                        for primaries in multi_primaries {
                            let stream: proc_macro2::TokenStream = quote! {
                                let mut indexed_columns: Vec<wcdb_core::winq::column::Column> = Vec::new();
                                for column in #primaries.columns() {
                                    indexed_columns.push(wcdb_core::winq::column::Column::new(
                                        #all_columns_map.get(column).unwrap().property_name(),
                                    ));
                                }
                                let table_constraint = wcdb_core::winq::table_constraint::TableConstraint::new();
                                table_constraint.primary_key().indexed_by(indexed_columns);
                                #binding_ident.add_table_constraint(table_constraint);
                            }
                            .into();
                            token_stream.extend(stream);
                        }
                    }
                }

                match table_config_info.multi_unique() {
                    None => {
                        println!("generate_table_config: multi_unique is none")
                    }
                    Some(multi_unique_vec) => {
                        for uniques in multi_unique_vec {
                            let stream: proc_macro2::TokenStream = quote! {
                                let mut indexed_columns: Vec<wcdb_core::winq::column::Column> = Vec::new();
                                for column in #uniques.columns() {
                                    indexed_columns.push(wcdb_core::winq::column::Column::new(
                                        #all_columns_map.get(column).unwrap().property_name(),
                                    ));
                                }
                                let table_constraint = wcdb_core::winq::table_constraint::TableConstraint::new();
                                table_constraint.unique().indexed_by(indexed_columns);
                                #binding_ident.add_table_constraint(table_constraint);
                            }
                            .into();
                            token_stream.extend(stream);
                        }
                    }
                }

                if table_config_info.is_without_row_id() {
                    let stream: proc_macro2::TokenStream = quote! {
                        #binding_ident.config_without_row_id();
                    }
                    .into();
                    token_stream.extend(stream);
                }

                match table_config_info.fts_module() {
                    None => {
                        println!("generate_table_config: fts_module is none");
                        return Ok(token_stream);
                    }
                    Some(module_info) => {
                        if module_info.fts_version().is_empty() {
                            println!("generate_table_config: fts_version is empty");
                            return Ok(token_stream);
                        }

                        let version = module_info.fts_version();
                        let stream: proc_macro2::TokenStream = quote! {
                            #binding_ident.config_virtual_module(#version.as_str());
                        }
                        .into();
                        token_stream.extend(stream);

                        let parameter = module_info.tokenizer_parameters().join(" ");
                        let tokenizer =
                            format!("{}{}{}", "tokenize = ", module_info.tokenizer(), parameter);
                        let stream: proc_macro2::TokenStream = quote! {
                            #binding_ident.config_virtual_module_argument(#tokenizer.as_str());
                        }
                        .into();
                        token_stream.extend(stream);

                        if (!module_info.external_table().is_empty()) {
                            let content =
                                format!("{}{}{}", "content='", module_info.external_table(), "'");
                            let stream: proc_macro2::TokenStream = quote! {
                                #binding_ident.config_virtual_module_argument(#content.as_str());
                            }
                            .into();
                            token_stream.extend(stream);
                        }
                    }
                }
            }
        }

        if token_stream.is_empty() {
            let stream: proc_macro2::TokenStream = quote! {
                // 内容为空
            }
            .into();
            token_stream.extend(stream);
        }
        println!(
            "generate_table_config-> content{:?}",
            token_stream.to_string()
        );
        Ok(token_stream.clone())
    }
}
