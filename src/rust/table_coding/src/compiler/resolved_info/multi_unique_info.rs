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
use crate::macros::multi_unique::MultiUnique;

pub struct MultiUniqueInfo {
    columns: Vec<String>,
}

impl MultiUniqueInfo {
    pub fn new() -> Self {
        MultiUniqueInfo { columns: vec![] }
    }

    pub fn resolve(multi_unique: &MultiUnique) -> MultiUniqueInfo {
        let mut info = MultiUniqueInfo::new();
        for x in multi_unique.columns() {
            info.columns.push(x.value().to_string());
        }
        info
    }

    pub fn columns(&self) -> &Vec<String> {
        &self.columns
    }
}
