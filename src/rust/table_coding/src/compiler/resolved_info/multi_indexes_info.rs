/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
use crate::compiler::resolved_info::column_info::ColumnInfo;
use proc_macro2::{Ident, Span};
use syn::LitStr;

pub struct MultiIndexesInfo {
    name: Option<LitStr>,
    columns: Vec<LitStr>,
}

impl MultiIndexesInfo {
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
