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
use crate::compiler::resolved_info::default_value_info::DefaultValueInfo;
use crate::compiler::rust_field_orm_info::{RustFieldORMInfo, RUST_FIELD_ORM_INFO_MAP};
use crate::macros::wcdb_field::WCDBField;

#[derive(Clone, Debug)]
pub struct ColumnInfo {
    property_name: String,
    property_type: String,
    nullable: bool,
    column_name: String,
    is_primary: bool,
    is_auto_increment: bool,
    enable_auto_increment_for_existing_table: bool,
    default_value: Option<DefaultValueInfo>,
    is_unique: bool,
    is_not_null: bool,
    is_not_indexed: bool,
    has_index: bool,
    index_name: String,
    index_is_unique: bool,
}

impl ColumnInfo {
    pub fn new() -> Self {
        ColumnInfo {
            property_name: "".to_string(),
            property_type: "".to_string(),
            nullable: false,
            column_name: "".to_string(),
            is_primary: false,
            is_auto_increment: false,
            enable_auto_increment_for_existing_table: false,
            default_value: None,
            is_unique: false,
            is_not_null: false,
            is_not_indexed: false,
            has_index: false,
            index_name: "".to_string(),
            index_is_unique: false,
        }
    }

    /// ColumnInfo(propertyName='multiPrimary1', propertyType='int', nullable=false, columnName='',
    /// isPrimary=false, isAutoIncrement=false, enableAutoIncrementForExistingTable=false,
    /// defaultValue=null, isUnique=false, isNotNull=false, isNotIndexed=false, hasIndex=false,
    /// indexName='', indexIsUnique=false)
    pub fn resolve(field: &WCDBField) -> ColumnInfo {
        let mut column_info = ColumnInfo::new();
        column_info.property_name = field
            .ident()
            .iter()
            .map(|field_name| field_name.to_string())
            .collect::<String>();
        column_info.property_type =
            WCDBField::get_property_type(&field.ty()).unwrap_or(String::from("None"));
        column_info.nullable = field.is_not_null();
        column_info.column_name = field.column_name();
        column_info.is_primary = field.is_primary();
        column_info.is_auto_increment = field.is_auto_increment();
        column_info.enable_auto_increment_for_existing_table =
            field.enable_auto_increment_for_existing_table();
        column_info.is_unique = field.is_unique();
        column_info.is_not_null = field.is_not_null();
        column_info.is_not_indexed = field.is_not_indexed();
        column_info.default_value = DefaultValueInfo::resolve(field.default());

        match field.index() {
            None => {
                column_info.has_index = false;
            }
            Some(index) => {
                column_info.has_index = true;
                column_info.index_name = index.name();
                column_info.index_is_unique = index.is_unique();
            }
        }

        column_info
    }

    pub fn property_name(&self) -> String {
        self.property_name.clone()
    }

    pub fn property_type(&self) -> String {
        self.property_type.clone()
    }

    pub fn nullable(&self) -> bool {
        self.nullable
    }

    pub fn column_name(&self) -> String {
        self.column_name.clone()
    }

    pub fn is_primary(&self) -> bool {
        self.is_primary
    }

    pub fn is_auto_increment(&self) -> bool {
        self.is_auto_increment
    }

    pub fn enable_auto_increment_for_existing_table(&self) -> bool {
        self.enable_auto_increment_for_existing_table
    }

    pub fn default_value(&self) -> &Option<DefaultValueInfo> {
        &self.default_value
    }

    pub fn is_unique(&self) -> bool {
        self.is_unique
    }

    pub fn is_not_null(&self) -> bool {
        self.is_not_null
    }

    pub fn is_not_indexed(&self) -> bool {
        self.is_not_indexed
    }

    pub fn has_index(&self) -> bool {
        self.has_index
    }

    pub fn index_name(&self) -> String {
        self.index_name.clone()
    }

    pub fn index_is_unique(&self) -> bool {
        self.index_is_unique
    }

    pub fn get_field_orm_info(&self) -> &RustFieldORMInfo {
        let property_type = self.property_type();
        let field_orm_info_opt = RUST_FIELD_ORM_INFO_MAP.get(property_type.as_str());
        assert!(
            field_orm_info_opt.is_some(),
            "filed not support {}",
            property_type.as_str()
        );
        field_orm_info_opt.unwrap()
    }

    pub fn set_property_name(&mut self, property_name: String) {
        self.property_name = property_name;
    }

    pub fn set_property_type(&mut self, property_type: String) {
        self.property_type = property_type;
    }

    pub fn set_nullable(&mut self, nullable: bool) {
        self.nullable = nullable;
    }

    pub fn set_column_name(&mut self, column_name: String) {
        self.column_name = column_name;
    }

    pub fn set_is_primary(&mut self, is_primary: bool) {
        self.is_primary = is_primary;
    }

    pub fn set_is_auto_increment(&mut self, is_auto_increment: bool) {
        self.is_auto_increment = is_auto_increment;
    }

    pub fn set_enable_auto_increment_for_existing_table(
        &mut self,
        enable_auto_increment_for_existing_table: bool,
    ) {
        self.enable_auto_increment_for_existing_table = enable_auto_increment_for_existing_table;
    }

    pub fn set_default_value(&mut self, default_value: Option<DefaultValueInfo>) {
        self.default_value = default_value;
    }

    pub fn set_is_unique(&mut self, is_unique: bool) {
        self.is_unique = is_unique;
    }

    pub fn set_is_not_null(&mut self, is_not_null: bool) {
        self.is_not_null = is_not_null;
    }

    pub fn set_is_not_indexed(&mut self, is_not_indexed: bool) {
        self.is_not_indexed = is_not_indexed;
    }

    pub fn set_has_index(&mut self, has_index: bool) {
        self.has_index = has_index;
    }

    pub fn set_index_name(&mut self, index_name: String) {
        self.index_name = index_name;
    }

    pub fn set_index_is_unique(&mut self, index_is_unique: bool) {
        self.index_is_unique = index_is_unique;
    }
}
