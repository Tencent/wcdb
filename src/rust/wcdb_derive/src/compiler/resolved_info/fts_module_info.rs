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
use crate::macros::fts_module::FTSModule;

pub struct FTSModuleInfo {
    fts_version: String,
    tokenizer: String,
    tokenizer_parameters: Vec<String>,
    external_table: String,
}

impl FTSModuleInfo {
    pub fn new() -> Self {
        FTSModuleInfo {
            fts_version: "".to_string(),
            tokenizer: "".to_string(),
            tokenizer_parameters: vec![],
            external_table: "".to_string(),
        }
    }

    pub fn resolve(fts_module: &FTSModule) -> Self {
        let tokenizer_parameters = fts_module
            .tokenizer_parameters()
            .iter()
            .map(|s| s.value())
            .collect::<Vec<_>>();
        Self {
            fts_version: fts_module.version().to_string(),
            tokenizer: fts_module.tokenizer().to_string(),
            tokenizer_parameters,
            external_table: fts_module.external_table().to_string(),
        }
    }

    pub fn fts_version(&self) -> String {
        self.fts_version.clone()
    }

    pub fn tokenizer(&self) -> String {
        self.tokenizer.clone()
    }

    pub fn tokenizer_parameters(&self) -> &Vec<String> {
        &self.tokenizer_parameters
    }

    pub fn external_table(&self) -> String {
        self.external_table.clone()
    }
}
