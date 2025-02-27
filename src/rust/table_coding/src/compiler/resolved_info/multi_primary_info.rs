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
use crate::macros::multi_primary::MultiPrimary;

pub struct MultiPrimaryInfo {
    columns: Vec<String>,
}

impl MultiPrimaryInfo {
    pub fn new() -> Self {
        MultiPrimaryInfo { columns: vec![] }
    }

    pub fn resolve(multi_primary: &MultiPrimary) -> MultiPrimaryInfo {
        let mut info = MultiPrimaryInfo::new();
        for x in multi_primary.columns() {
            info.columns.push(x.value());
        }
        info
    }

    pub fn columns(&self) -> &Vec<String> {
        &self.columns
    }
}
