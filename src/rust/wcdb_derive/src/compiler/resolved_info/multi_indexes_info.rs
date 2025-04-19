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
use crate::macros::multi_indexes::MultiIndexes;
use proc_macro2::{Ident, Span};
use std::collections::HashMap;

pub struct MultiIndexesInfo {
    name: String,
    columns: Vec<String>,
}

impl MultiIndexesInfo {
    pub fn new() -> Self {
        MultiIndexesInfo {
            name: "".to_string(),
            columns: vec![],
        }
    }

    pub fn get_index_name_ident(&self) -> Ident {
        let mut index_name = self.name.clone();
        if self.name.is_empty() {
            let join_str = self.columns().join("_");
            index_name = format!("{}{}{}", "_", join_str, "_index");
        }
        Ident::new(&index_name, Span::call_site())
    }

    pub(crate) fn get_index_column_name_ident_vec(
        &self,
        all_columns_map: &HashMap<String, ColumnInfo>,
    ) -> Vec<Ident> {
        if self.columns.is_empty() {
            return vec![];
        }
        let mut ret_vec = vec![];
        for column in self.columns.iter() {
            let mut property_name = column.clone();
            match all_columns_map.get(&property_name) {
                None => {}
                Some(val) => {
                    property_name = val.property_name();
                }
            }
            ret_vec.push(Ident::new(property_name.as_str(), Span::call_site()));
        }
        ret_vec
    }

    pub fn name(&self) -> String {
        self.name.clone()
    }

    pub fn columns(&self) -> &Vec<String> {
        &self.columns
    }

    pub fn resolve(multi_indexes: &MultiIndexes) -> MultiIndexesInfo {
        let mut info = MultiIndexesInfo::new();
        match multi_indexes.name() {
            None => {}
            Some(val) => {
                info.name = val.value();
            }
        }
        for lit_str in multi_indexes.columns() {
            info.columns.push(lit_str.value());
        }

        info
    }
}
