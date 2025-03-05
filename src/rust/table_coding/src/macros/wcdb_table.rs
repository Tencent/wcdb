use crate::compiler::resolved_info::column_info::ColumnInfo;
use crate::macros::multi_indexes::MultiIndexes;
use crate::macros::multi_primary::MultiPrimary;
use crate::macros::multi_unique::MultiUnique;
use crate::macros::wcdb_field::WCDBField;
use darling::ast::Data;
use darling::FromDeriveInput;
use proc_macro2::{Ident, Span};
use syn::{Generics, Type};

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
    pub fn get_db_table(&self) -> Ident {
        Ident::new(&format!("Db{}", self.ident), Span::call_site())
    }

    pub fn get_field_vec(&self) -> Vec<&WCDBField> {
        match &self.data {
            Data::Struct(fields) => fields.iter().collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_all_column_info_vec(&self) -> Vec<ColumnInfo> {
        match &self.data {
            Data::Struct(fields) => fields
                .iter()
                .map(|field| {
                    let mut info = ColumnInfo::new();
                    info.set_property_name(field.ident().as_ref().unwrap().to_string().clone());
                    info.set_column_name(if field.column_name().is_empty() {
                        info.property_name().clone()
                    } else {
                        field.column_name().clone()
                    });
                    info
                })
                .collect(),
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

    pub fn get_field_column_name_ident_vec(&self) -> Vec<Ident> {
        match &self.data {
            Data::Struct(fields) => {
                fields
                    .iter()
                    .map(|field| {
                        let mut ident = field.ident().clone().unwrap();
                        if field.column_name().len() > 0 {
                            // 使用 column_name 当做表名
                            ident = Ident::new(field.column_name().as_str(), ident.span());
                        }
                        ident
                    })
                    .collect()
            }
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_field_is_auto_increment_vec(&self) -> Vec<bool> {
        match &self.data {
            Data::Struct(fields) => fields
                .iter()
                .map(|field| field.is_auto_increment())
                .collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_field_is_primary_key_vec(&self) -> Vec<bool> {
        match &self.data {
            Data::Struct(fields) => fields.iter().map(|field| field.is_primary()).collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_enable_auto_increment_for_existing_table(&self) -> bool {
        match &self.data {
            Data::Struct(fields) => {
                for field in fields.iter() {
                    if field.enable_auto_increment_for_existing_table() {
                        return true;
                    }
                }
                false
            }
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_field_type_vec(&self) -> Vec<&Type> {
        match &self.data {
            Data::Struct(fields) => fields.iter().map(|field| field.ty()).collect(),
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_auto_increment_ident_field(&self) -> Option<&WCDBField> {
        match &self.data {
            Data::Struct(fields) => {
                let mut ret = None;
                for field in fields.iter() {
                    if field.is_primary() && field.is_auto_increment() {
                        ret = Some(field);
                        break;
                    }
                }
                ret
            }
            _ => panic!("WCDBTable only works on structs"),
        }
    }

    pub fn get_multi_index_vec(&self) -> Vec<MultiIndexes> {
        self.multi_indexes.iter().map(|item| item.clone()).collect()
    }

    pub fn get_multi_primary_vec(&self) -> Vec<MultiPrimary> {
        self.multi_primaries
            .iter()
            .map(|item| item.clone())
            .collect()
    }

    pub fn get_multi_unique_vec(&self) -> Vec<MultiUnique> {
        self.multi_unique.iter().map(|item| item.clone()).collect()
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
