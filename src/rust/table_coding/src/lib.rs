use darling::ast::Data;
use darling::{FromDeriveInput, FromField, FromMeta};
use once_cell::sync::Lazy;
use proc_macro::TokenStream;
use proc_macro2::Span;
use quote::{quote, ToTokens};
use std::collections::HashMap;
use std::fmt::Debug;
use syn::parse::Parse;
use syn::spanned::Spanned;
use syn::{parse_macro_input, DeriveInput, Generics, Ident, LitStr, Type};

#[derive(Debug, FromDeriveInput)]
#[darling(attributes(WCDBTable))]
struct WCDBTable {
    ident: Ident,
    generics: Generics,
    data: Data<(), WCDBField>,
    #[darling(default, multiple)]
    multi_indexes: Vec<MultiIndexes>,
    #[darling(default)]
    multi_primaries: Vec<LitStr>,
    #[darling(default)]
    multi_unique: Vec<LitStr>,
    #[darling(default)]
    is_without_row_id: bool,
    // #[darling(default)]
    // fts_module: ???,
}

impl WCDBTable {
    fn get_db_table(&self) -> Ident {
        Ident::new(&format!("Db{}", self.ident), Span::call_site())
    }

    fn get_field_ident_vec(&self) -> Vec<&Ident> {
        match &self.data {
            Data::Struct(fields) => fields
                .iter()
                .map(|field| field.ident.as_ref().unwrap())
                .collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    fn get_field_type_vec(&self) -> Vec<&Type> {
        match &self.data {
            Data::Struct(fields) => fields.iter().map(|field| &field.ty).collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }
}

#[derive(Debug, FromMeta)]
struct MultiIndexes {
    name: Option<LitStr>,
    columns: Vec<LitStr>,
}

#[derive(Debug, FromField)]
#[darling(attributes(WCDBField))]
struct WCDBField {
    ident: Option<Ident>,
    ty: Type,
    #[darling(default)]
    pub column_name: String,
    #[darling(default)]
    pub is_primary: bool,
    #[darling(default)]
    pub is_auto_increment: bool,
    #[darling(default)]
    pub enable_auto_increment_for_existing_table: bool,
    #[darling(default)]
    pub is_unique: bool,
    #[darling(default)]
    pub is_not_null: bool,
    #[darling(default)]
    pub is_not_indexed: bool,
}

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
    let table_ident = &table.ident;
    let db_table_ident = table.get_db_table();
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    let field_ident_vec = table.get_field_ident_vec();

    let singleton_statements = generate_singleton(&table)?;
    let extract_object_statements = generate_extract_object(&table)?;
    let bind_field_statements = generate_bind_field(&table)?;

    Ok(quote! {
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
            fn binding_type(&self) -> std::any::TypeId {
                std::any::TypeId::of::<#table_ident>()
            }

            fn all_binding_fields(&self) -> Vec<&wcdb_core::orm::field::Field<#table_ident>> {
                unsafe { vec![
                    #(&*self.#field_ident_vec,)*
                ] }
            }

            fn base_binding(&self) -> &wcdb_core::orm::binding::Binding {
                &*#binding_ident
            }

            fn extract_object(
                &self,
                fields: &Vec<&wcdb_core::orm::field::Field<#table_ident>>,
                prepared_statement: &wcdb_core::core::prepared_statement::PreparedStatement,
            ) -> #table_ident {
                #extract_object_statements
            }

            fn bind_field(
                &self,
                object: &#table_ident,
                field: &wcdb_core::orm::field::Field<#table_ident>,
                index: usize,
                prepared_statement: &std::sync::Arc<wcdb_core::core::prepared_statement::PreparedStatement>,
            ) {
                #bind_field_statements
            }

            fn is_auto_increment(&self, object: &#table_ident) -> bool {
                false
            }

            fn set_last_insert_row_id(&self, object: &mut #table_ident, last_insert_row_id: i64) {}
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

struct FieldInfo {
    column_type: String,
    nullable: bool,
    field_setter: String,
    field_getter: String,
}

impl FieldInfo {
    fn new(column_type: &str, nullable: bool, field_setter: &str, field_getter: &str) -> Self {
        Self {
            column_type: column_type.to_string(),
            nullable,
            field_setter: field_setter.to_string(),
            field_getter: field_getter.to_string(),
        }
    }
}

macro_rules! match_field_info {
    ($field_type_string:expr, $field:expr, $getter_name:ident) => {
        match FIELD_INFO_MAP.get(&$field_type_string) {
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

macro_rules! get_field_info_vec {
    ($field_type_vec:expr, $field_getter:ident) => {
        $field_type_vec
            .iter()
            .map(|field| {
                let field_type_string = get_field_type_string(field)?;
                let type_string = match_field_info!(field_type_string, field, $field_getter);
                Ok(Ident::new(&type_string, Span::call_site()))
            })
            .collect::<syn::Result<Vec<_>>>()?
    };
}

fn get_field_type_string(field: &Type) -> syn::Result<String> {
    match field {
        Type::Path(type_path) => Ok(type_path.path.segments[0].ident.to_string()),
        _ => Err(syn::Error::new(
            field.span(),
            "WCDBTable's field type only works on Path",
        )),
    }
}

static FIELD_INFO_MAP: Lazy<HashMap<String, FieldInfo>> = Lazy::new(|| {
    let mut all_info = HashMap::new();
    all_info.insert(
        "bool".to_string(),
        FieldInfo::new("Integer", false, "bind_bool", "get_bool"),
    );
    all_info.insert(
        "i8".to_string(),
        FieldInfo::new("Integer", false, "bind_i8", "get_i8"),
    );
    all_info.insert(
        "i16".to_string(),
        FieldInfo::new("Integer", false, "bind_i16", "get_i16"),
    );
    all_info.insert(
        "i32".to_string(),
        FieldInfo::new("Integer", false, "bind_i32", "get_i32"),
    );
    all_info.insert(
        "i64".to_string(),
        FieldInfo::new("Integer", false, "bind_i64", "get_i64"),
    );
    all_info.insert(
        "f32".to_string(),
        FieldInfo::new("Float", false, "bind_f32", "get_f32"),
    );
    all_info.insert(
        "f64".to_string(),
        FieldInfo::new("Float", false, "bind_f64", "get_f64"),
    );
    all_info.insert(
        "String".to_string(),
        FieldInfo::new("Text", false, "bind_text", "get_text"),
    );
    all_info.insert(
        "Option<String>".to_string(),
        FieldInfo::new("Text", true, "bind_text", "get_text"),
    );
    all_info
});

fn generate_singleton(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let db_table_ident = table.get_db_table();
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_ident_def_vec: Vec<Ident> = field_ident_vec
        .iter()
        .map(|ident| Ident::new(&format!("{}_def", ident.to_string()), Span::call_site()))
        .collect();
    let column_type_vec: Vec<_> = get_field_info_vec!(field_type_vec, column_type);
    let binding = format!("{}_BINDING", db_table_ident.to_string().to_uppercase());
    let binding_ident = Ident::new(&binding, Span::call_site());
    let instance = format!("{}_INSTANCE", db_table_ident.to_string().to_uppercase());
    let instance_ident = Ident::new(&instance, Span::call_site());
    let field_id_vec: Vec<usize> = (1..=field_ident_vec.len()).collect();
    Ok(quote! {
        pub static #binding_ident: once_cell::sync::Lazy<wcdb_core::orm::binding::Binding> = once_cell::sync::Lazy::new(|| {
            wcdb_core::orm::binding::Binding::new()
        });
        pub static #instance_ident: once_cell::sync::Lazy<#db_table_ident> = once_cell::sync::Lazy::new(|| {
            let mut instance = #db_table_ident::default();
            let instance_raw = unsafe { &instance as *const #db_table_ident };
            #(
                let field = Box::new(wcdb_core::orm::field::Field::new(
                    stringify!(#field_ident_vec),
                    instance_raw,
                    #field_id_vec,
                    false,
                    false
                ));
                let #field_ident_def_vec = wcdb_core::winq::column_def::ColumnDef::new_with_column_type(
                    &field.get_column(),
                    wcdb_core::winq::column_type::ColumnType::#column_type_vec
                );
                instance.#field_ident_vec = unsafe { Box::into_raw(field) };
                #binding_ident.add_column_def(#field_ident_def_vec);
            )*
            instance
        });
    })
}

fn generate_extract_object(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident;
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_get_type_vec: Vec<_> = get_field_info_vec!(field_type_vec, field_getter);
    let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();
    Ok(quote! {
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
    })
}

fn generate_bind_field(table: &WCDBTable) -> syn::Result<proc_macro2::TokenStream> {
    let table_ident = &table.ident;
    let field_ident_vec = table.get_field_ident_vec();
    let field_type_vec = table.get_field_type_vec();
    let field_id_vec: Vec<_> = (1..=field_type_vec.len()).collect();
    let field_bind_type_vec: Vec<_> = field_type_vec
        .iter()
        .map(|field| {
            let field_type_string = get_field_type_string(field)?;
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
    Ok(quote! {
        match field.get_field_id() {
            #(
                #field_id_vec => prepared_statement.#field_bind_type_vec(object.#field_ident_vec #as_ref_vec, index),
            )*
            _ => panic!(
                "Invalid id {} of field {} in {}",
                field.get_field_id(),
                wcdb_core::winq::identifier::IdentifierTrait::get_description(field),
                stringify!(#table_ident)
            ),
        }
    })
}
