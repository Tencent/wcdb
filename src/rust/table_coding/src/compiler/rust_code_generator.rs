use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::compiler::resolved_info::table_config_info::TableConfigInfo;
use crate::field_orm_info::FIELD_ORM_INFO_MAP;
use crate::macros::wcdb_field::WCDBField;
use crate::macros::wcdb_table::WCDBTable;
use darling::ast::Data;
use proc_macro2::{Ident, Span};
use quote::quote;
use std::collections::HashMap;
use syn::spanned::Spanned;
use syn::Type;

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

pub struct RustCodeGenerator {
    package_name: String,
    class_name: String,
    orm_class_name: String, //DB+StructName
    table_constraint_info: Option<TableConfigInfo>,
    all_column_info: Vec<ColumnInfo>,
}

impl RustCodeGenerator {
    pub(crate) fn new() -> Self {
        RustCodeGenerator {
            package_name: "".to_string(),
            class_name: "".to_string(),
            orm_class_name: "".to_string(),
            table_constraint_info: None,
            all_column_info: vec![],
        }
    }

    pub(crate) fn set_package_name(&mut self, package_name: String) {
        self.package_name = package_name;
    }

    pub(crate) fn set_class_name(&mut self, class_name: String) {
        self.class_name = class_name;
    }

    pub(crate) fn set_orm_class_name(&mut self, orm_class_name: String) {
        self.orm_class_name = orm_class_name;
    }

    pub(crate) fn set_table_constraint_info(
        &mut self,
        table_constraint_info: Option<TableConfigInfo>,
    ) {
        self.table_constraint_info = table_constraint_info;
    }

    pub(crate) fn set_all_column_info(&mut self, data: &Data<(), WCDBField>) {
        match data {
            Data::Enum(_) => {}
            Data::Struct(fields) => {
                for field in &fields.fields {
                    self.all_column_info.push(ColumnInfo::resolve(&field));
                }
            }
        }
    }

    pub(crate) fn generate_fields(&self) -> syn::Result<proc_macro2::TokenStream> {
        let mut token_stream = proc_macro2::TokenStream::new();
        let class_name = &self.class_name;
        for column_info in &self.all_column_info {
            let name = &column_info.property_name();
            // name: id ， class_name: ProcessorTest
            let stream_tmp: proc_macro2::TokenStream = quote! {
                pub(crate) #name: Field<#class_name>,
            }
            .into();
            token_stream.extend(stream_tmp);
        }
        Ok(token_stream)
    }

    pub(crate) fn generate_singleton(
        &self,
        table: &WCDBTable,
    ) -> syn::Result<proc_macro2::TokenStream> {
        let db_table_ident = table.get_db_table();
        let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
        let binding_ident = Ident::new(&binding, Span::call_site());
        let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
        let instance_ident = Ident::new(&instance, Span::call_site());

        let generate_table_config = self.generate_table_config(&binding_ident)?;
        let columns_statements = self.generate_columns(&binding_ident)?;

        Ok(quote! {
            pub static #binding_ident: once_cell::sync::Lazy<wcdb_core::orm::binding::Binding> = once_cell::sync::Lazy::new(|| {
                wcdb_core::orm::binding::Binding::new()
            });
            pub static #instance_ident: once_cell::sync::Lazy<#db_table_ident> = once_cell::sync::Lazy::new(|| {
                let mut instance = #db_table_ident::default();
                let instance_raw = unsafe { &instance as *const #db_table_ident };

                #columns_statements
                #generate_table_config
                instance
            });
        })
    }

    fn generate_columns(&self, binding_ident: &Ident) -> syn::Result<proc_macro2::TokenStream> {
        if self.all_column_info.is_empty() {
            return Ok(quote::quote! {}.into());
        }

        let mut token_stream = proc_macro2::TokenStream::new();
        let mut field_id: usize = 1;
        for column_info in &self.all_column_info {
            let property_name = column_info.property_name();
            let mut column_name = column_info.column_name();
            if column_name.is_empty() {
                column_name = property_name.clone();
            }

            // multiPrimary1
            let column_name_ident = Ident::new(&*column_name, Span::call_site());
            // Ident 等于 property_name
            let field_ident = Ident::new(&*property_name, Span::call_site());
            // multi_primary1_def
            let field_def_ident = Ident::new(
                &format!("{}Def", field_ident.to_string()),
                Span::call_site(),
            );

            let is_primary_key = column_info.is_primary();
            let is_auto_increment = column_info.is_auto_increment();
            let column_type_ident = Ident::new(&*column_info.property_type(), Span::call_site());

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

            token_stream.extend(quote! {
                let column_constraint = wcdb_core::winq::column_constraint::ColumnConstraint::new();
            });

            if is_primary_key {
                token_stream.extend(quote! {
                    column_constraint.primary_key();
                });
                if is_auto_increment {
                    token_stream.extend(quote! {
                        column_constraint.auto_increment();
                    });
                }
            }

            match column_info.default_value() {
                None => {}
                Some(default) => {
                    if column_info.property_type() == "Integer" {
                        let int_value = default.i32_value();
                        token_stream.extend(quote::quote! {
                            column_constraint.default_to(#int_value);
                        });
                    } else if column_info.property_type() == "Float" {
                        let double_value = default.f64_value();
                        token_stream.extend(quote::quote! {
                            column_constraint.default_to(#double_value);
                        });
                    } else {
                        let text_value = default.text_value();
                        token_stream.extend(quote::quote! {
                            column_constraint.default_to(#text_value);
                        });
                    }
                }
            }

            if column_info.is_unique() {
                token_stream.extend(quote! {
                    column_constraint.unique();
                });
            }

            if column_info.is_not_null() {
                token_stream.extend(quote! {
                    column_constraint.not_null();
                });
            }

            if column_info.is_not_indexed() {
                token_stream.extend(quote! {
                    column_constraint.un_index();
                });
            }

            token_stream.extend(quote! {
                #field_def_ident.constraint(column_constraint);
            });

            token_stream.extend(quote! {
                instance.#field_ident = unsafe { Box::into_raw(field) };
                #binding_ident.add_column_def(#field_def_ident);
            });

            if column_info.enable_auto_increment_for_existing_table() {
                token_stream.extend(quote! {
                    #binding_ident.enable_auto_increment_for_existing_table();
                });
            }

            if !column_info.has_index() {
                continue;
            }
            let mut index_name = column_info.index_name();
            let mut is_full_name: bool = true;
            if index_name.is_empty() {
                is_full_name = false;
                index_name = format!("{}{}{}", "_", column_name.clone().as_str(), "_index");
            }
            let index_is_unique = column_info.index_is_unique();
            let index_name_ident = Ident::new(&*index_name.clone(), Span::call_site());
            let property_name_ident = Ident::new(&*property_name.clone(), Span::call_site());
            token_stream.extend(quote::quote! {
                let create_index = wcdb_core::winq::statement_create_index::StatementCreateIndex::new();
                create_index.if_not_exist();
                if #index_is_unique {
                    create_index.unique();
                }
                let mut column_names: Vec<String> = Vec::new();
                 column_names.push(stringify!(#property_name_ident).to_string());
                create_index.indexed_by_column_names(&column_names);
                #binding_ident.add_index(stringify!(#index_name_ident), #is_full_name, create_index);
            });
        }
        Ok(token_stream)
    }

    fn generate_table_config(
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

                        if !module_info.external_table().is_empty() {
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

    pub(crate) fn generate_binding_type(
        &self,
        table_ident: &&Ident,
    ) -> syn::Result<proc_macro2::TokenStream> {
        Ok(quote::quote! {
            fn binding_type(&self) -> std::any::TypeId {
                std::any::TypeId::of::<#table_ident>()
            }
        })
    }

    pub(crate) fn generate_binding_fields(
        &self,
        table_ident: &&Ident,
        field_ident_vec: &Vec<&Ident>,
    ) -> syn::Result<proc_macro2::TokenStream> {
        Ok(quote::quote! {
            fn all_binding_fields(&self) -> Vec<&wcdb_core::orm::field::Field<#table_ident>> {
                unsafe { vec![
                    #(&*self.#field_ident_vec,)*
                ] }
            }
        })
    }

    pub(crate) fn generate_base_binding(
        &self,
        binding_ident: &Ident,
    ) -> syn::Result<proc_macro2::TokenStream> {
        Ok(quote::quote! {
            fn base_binding(&self) -> &wcdb_core::orm::binding::Binding {
                &*#binding_ident
            }
        })
    }

    pub(crate) fn generate_extract_object(
        &self,
        table_ident: &&Ident,
        field_ident_vec: &Vec<&Ident>,
        field_type_vec: &Vec<&Type>,
    ) -> syn::Result<proc_macro2::TokenStream> {
        let field_get_type_vec: Vec<_> = get_field_info_vec!(field_type_vec, field_getter);
        let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();

        Ok(quote::quote! {
             fn extract_object(
                &self,
                fields: &Vec<&wcdb_core::orm::field::Field<#table_ident>>,
                prepared_statement: &wcdb_core::core::prepared_statement::PreparedStatement,
            ) -> #table_ident {
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
            }
        })
    }

    pub(crate) fn generate_bind_object(
        &self,
        table_ident: &&Ident,
        field_ident_vec: &Vec<&Ident>,
        field_type_vec: &Vec<&Type>,
    ) -> syn::Result<proc_macro2::TokenStream> {
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
        Ok(quote::quote! {
             fn bind_field(
                &self,
                object: &#table_ident,
                field: &wcdb_core::orm::field::Field<#table_ident>,
                index: usize,
                prepared_statement: &std::sync::Arc<wcdb_core::core::prepared_statement::PreparedStatement>,
            ) {
                match field.get_field_id() {
                    #(
                        #field_id_vec => prepared_statement.#field_bind_type_vec(object.#field_ident_vec #as_ref_vec, index),
                    )*
                    _ => panic!("Invalid id {} of field {} in {}",
                        field.get_field_id(),
                        wcdb_core::winq::identifier::IdentifierTrait::get_description(field),
                        stringify!(#table_ident)
                    ),
                }
            }
        })
    }

    pub(crate) fn generate_auto_increment_config(
        &self,
        table_ident: &&Ident,
        auto_increment_field_opt: &Option<&WCDBField>,
    ) -> syn::Result<proc_macro2::TokenStream> {
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
}
