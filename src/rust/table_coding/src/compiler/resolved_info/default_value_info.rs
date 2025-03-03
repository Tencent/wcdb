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
use crate::macros::field_attr::FieldAttr;
use crate::macros::wcdb_default::WCDBDefault;

#[derive(Clone, Debug)]
pub struct DefaultValueInfo {
    i32_value: i64,
    f64_value: f64,
    text_value: String,
}

impl DefaultValueInfo {
    pub fn new() -> Self {
        DefaultValueInfo {
            i32_value: 0,
            f64_value: 0.0,
            text_value: "".to_string(),
        }
    }

    pub fn i32_value(&self) -> i64 {
        self.i32_value
    }

    pub fn f64_value(&self) -> f64 {
        self.f64_value
    }

    pub fn text_value(&self) -> String {
        self.text_value.clone()
    }

    pub(crate) fn resolve(attr: &Option<FieldAttr>) -> Option<DefaultValueInfo> {
        match attr {
            None => None,
            Some(val) => match val.default() {
                None => None,
                Some(default) => Some(DefaultValueInfo::create(&default)),
            },
        }
    }

    pub(crate) fn create(default_value: &WCDBDefault) -> DefaultValueInfo {
        let mut info = DefaultValueInfo::new();
        info.i32_value = default_value.i32_value();
        info.f64_value = default_value.f64_value();
        info.text_value = default_value.text_value().to_string();
        info
    }
}
