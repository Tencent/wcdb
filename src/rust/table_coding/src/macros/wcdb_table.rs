use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::macros::multi_indexes::MultiIndexes;
use crate::macros::multi_primary::MultiPrimary;
use crate::macros::multi_unique::MultiUnique;
use crate::macros::wcdb_field::WCDBField;
use darling::ast::Data;
use darling::FromDeriveInput;
use proc_macro2::{Ident, Span};
use syn::Generics;

#[derive(Debug, FromDeriveInput)]
#[darling(attributes(WCDBTable))]
pub struct WCDBTable {
    ident: Ident,
    generics: Generics,
    data: Data<(), WCDBField>,
    // For fields with columnName macro, specify MultiIndexes columns as columnName; otherwise use propertyName
    #[darling(default, multiple)]
    multi_indexes: Vec<MultiIndexes>,
    // For fields with columnName macro, specify MultiPrimary columns as columnName; otherwise use propertyName
    #[darling(default, multiple)]
    multi_primaries: Vec<MultiPrimary>,
    // For fields with columnName macro, specify MultiUnique columns as columnName; otherwise use propertyName
    #[darling(default, multiple)]
    multi_unique: Vec<MultiUnique>,
    #[darling(default)]
    is_without_row_id: bool,
    // #[darling(default)]
    // fts_module: ???,
}

impl WCDBTable {
    pub fn get_db_table_ident(&self) -> Ident {
        Ident::new(&format!("Db{}", self.ident), Span::call_site())
    }

    pub fn get_struct_name(&self) -> String {
        self.ident.to_string()
    }

    pub fn get_db_table_name(&self) -> String {
        format!("Db{}", self.get_struct_name())
    }

    pub fn get_all_column_info(&self) -> Vec<ColumnInfo> {
        match &self.data {
            Data::Struct(fields) => {
                let mut all_column_info = Vec::new();
                for field in &fields.fields {
                    all_column_info.push(ColumnInfo::resolve(&field));
                }
                all_column_info
            }
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_field_ident_vec(&self) -> Vec<&Ident> {
        match &self.data {
            Data::Struct(fields) => fields
                .iter()
                .map(|field| field.ident().as_ref().unwrap())
                .collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub(crate) fn ident(&self) -> &Ident {
        &self.ident
    }

    pub(crate) fn generics(&self) -> &Generics {
        &self.generics
    }

    pub(crate) fn data(&self) -> &Data<(), WCDBField> {
        &self.data
    }

    pub(crate) fn multi_indexes(&self) -> &Vec<MultiIndexes> {
        &self.multi_indexes
    }

    pub(crate) fn multi_primaries(&self) -> &Vec<MultiPrimary> {
        &self.multi_primaries
    }

    pub(crate) fn multi_unique(&self) -> &Vec<MultiUnique> {
        &self.multi_unique
    }

    pub(crate) fn is_without_row_id(&self) -> bool {
        self.is_without_row_id
    }
}
