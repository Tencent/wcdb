use crate::compiler::resolved_info::column_info::ColumnInfo;
use darling::FromMeta;
use proc_macro2::{Ident, Span};
use syn::LitStr;

#[derive(Debug, FromMeta, Clone)]
pub struct MultiIndexes {
    name: Option<LitStr>,
    columns: Vec<LitStr>,
}

impl MultiIndexes {
    pub fn get_index_name_ident(&self) -> Ident {
        let index_name = match &self.name {
            None => {
                let columns = &self.columns;
                columns
                    .iter()
                    .flat_map(|s| vec!["_".to_string(), s.value().clone()])
                    .collect::<String>()
                    + "_index"
            }
            Some(index_name) => index_name.value(),
        };
        Ident::new(&index_name, Span::call_site())
    }

    pub(crate) fn get_index_column_name_ident_vec(
        &self,
        all_field_info_vec: &Vec<ColumnInfo>,
    ) -> Vec<Ident> {
        if self.columns.is_empty() {
            return vec![];
        }
        let mut ret_vec = vec![];
        for column in self.columns.iter() {
            let column_name = &column.value();
            let mut property_name = column_name.clone();
            for column_info in all_field_info_vec {
                if column_info.column_name() == column_name.clone() {
                    property_name = column_info.property_name();
                    break;
                }
            }
            ret_vec.push(Ident::new(property_name.as_str(), Span::call_site()));
        }
        ret_vec
    }

    pub fn get_is_full_name(&self) -> bool {
        self.name.is_some()
    }
}
