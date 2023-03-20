//
// Created by 陈秋文 on 2022/8/27.
//

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

#pragma once

#include "CPPBindingMacro.h"
#include "CPPColumnConstraintMacro.h"
#include "CPPFieldMacro.h"
#include "CPPIndexMacro.h"
#include "CPPTableConstraintMacro.h"
#include "CPPVirtualTableMacro.h"
#include "Macro.h"

#define WCDB_FIELD(memberPointer) WCDB::Field(&memberPointer)

#define WCDB_CPP_ORM_DECLARATION(className)                                    \
public:                                                                        \
    __WCDB_CPP_ORM_COMMON_DECLARATION(className)                               \
    static const WCDB::Fields allFields();                                     \
    static const WCDB::Binding &getObjectRelationBinding();

#define WCDB_CPP_ORM_DECLARE_AND_START_IMPLEMENT(className)                    \
public:                                                                        \
    __WCDB_CPP_ORM_COMMON_DECLARATION(className)                               \
    static const WCDB::Fields allFields()                                      \
    __WCDB_CPP_ORM_ALL_PROPERTIES_IMPLEMENTATION static const WCDB::Binding &  \
    getObjectRelationBinding() __WCDB_CPP_ORM_GET_BINDING_IMPLEMENTATIONP_START

#define WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(className)                                                                    \
    using _wcdb_orm_##className = className;                                                                            \
    const WCDB::Fields _wcdb_orm_##className::allFields()                                                               \
    __WCDB_CPP_ORM_ALL_PROPERTIES_IMPLEMENTATION const WCDB::Binding &_wcdb_orm_##className::getObjectRelationBinding() \
    __WCDB_CPP_ORM_GET_BINDING_IMPLEMENTATIONP_START

#define WCDB_CPP_ORM_IMPLEMENTATION_END __WCDB_CPP_ORM_IMPLEMENTATION_END

#define WCDB_CPP_SYNTHESIZE(fieldName)                                         \
    __WCDB_CPP_SYNTHESIZE_IMP(fieldName, WCDB_STRINGIFY(fieldName))

#define WCDB_CPP_SYNTHESIZE_COLUMN(fieldName, columnName)                      \
    __WCDB_CPP_SYNTHESIZE_IMP(fieldName, columnName)

// Column Constraint
#define WCDB_CPP_PRIMARY(fieldName) __WCDB_CPP_PRIMARY_IMP(fieldName, false)

#define WCDB_CPP_PRIMARY_ASC(fieldName)                                        \
    __WCDB_CPP_ORDERED_PRIMARY_IMP(fieldName, WCDB::Order::ASC, false)

#define WCDB_CPP_PRIMARY_DESC(fieldName)                                       \
    __WCDB_CPP_ORDERED_PRIMARY_IMP(fieldName, WCDB::Order::DESC, false)

#define WCDB_CPP_PRIMARY_AUTO_INCREMENT(fieldName)                             \
    __WCDB_CPP_PRIMARY_IMP(fieldName, true)

#define WCDB_CPP_PRIMARY_ASC_AUTO_INCREMENT(fieldName)                         \
    __WCDB_CPP_ORDERED_PRIMARY_IMP(fieldName, WCDB::Order::ASC, true)

#define WCDB_CPP_NOT_NULL(fieldName) __WCDB_CPP_NOT_NULL_IMP(fieldName)

#define WCDB_CPP_UNIQUE(fieldName) __WCDB_CPP_UNIQUE_IMP(fieldName)

#define WCDB_CPP_DEFAULT(fieldName, defaultValue)                              \
    __WCDB_CPP_DEFAULT_IMP(fieldName, defaultValue)

#define WCDB_CPP_CHECK(fieldName, condition)                                   \
    __WCDB_CPP_CHECK_IMP(fieldName, condition)

//For FTS
#define WCDB_CPP_NOTINDEXED(fieldName) __WCDB_CPP_NOTINDEXED_IMP(fieldName)

// Table Constraint
#define WCDB_CPP_WITHOUT_ROWID __WCDB_CPP_WITHOUT_ROWID

#define WCDB_CPP_MULTI_PRIMARY(constraintName, fieldName)                      \
    __WCDB_CPP_MULTI_PRIMARY_IMP(constraintName, fieldName)

#define WCDB_CPP_MULTI_PRIMARY_ASC(constraintName, fieldName)                  \
    __WCDB_CPP_ORDERED_MULTI_PRIMARY_IMP(constraintName, fieldName, WCDB::Order::ASC)

#define WCDB_CPP_MULTI_PRIMARY_DESC(constraintName, fieldName)                 \
    __WCDB_CPP_ORDERED_MULTI_PRIMARY_IMP(constraintName, fieldName, WCDB::Order::DESC)

#define WCDB_CPP_MULTI_UNIQUE(constraintName, fieldName)                       \
    __WCDB_CPP_MULTI_UNIQUE_IMP(constraintName, fieldName)

#define WCDB_CPP_MULTI_UNIQUE_ASC(constraintName, fieldName)                   \
    __WCDB_CPP_ORDERED_MULTI_UNIQUE_IMP(constraintName, fieldName, WCDB::Order::ASC)

#define WCDB_CPP_MULTI_UNIQUE_DESC(constraintName, fieldName)                  \
    __WCDB_CPP_ORDERED_MULTI_UNIQUE_IMP(constraintName, fieldName, WCDB::Order::DESC)

// Index
#define WCDB_CPP_INDEX(indexSuffixName, fieldName)                             \
    __WCDB_CPP_INDEX_IMP(indexSuffixName, fieldName, false)

#define WCDB_CPP_INDEX_ASC(indexSuffixName, fieldName)                         \
    __WCDB_CPP_ORDERED_INDEX_IMP(indexSuffixName, fieldName, WCDB::Order::ASC, false)

#define WCDB_CPP_INDEX_DESC(indexSuffixName, fieldName)                        \
    __WCDB_CPP_ORDERED_INDEX_IMP(indexSuffixName, fieldName, WCDB::Order::DESC, false)

#define WCDB_CPP_UNIQUE_INDEX(indexSuffixName, fieldName)                      \
    __WCDB_CPP_INDEX_IMP(indexSuffixName, fieldName, true)

#define WCDB_CPP_UNIQUE_INDEX_ASC(indexSuffixName, fieldName)                  \
    __WCDB_CPP_ORDERED_INDEX_IMP(indexSuffixName, fieldName, WCDB::Order::ASC, true)

#define WCDB_CPP_UNIQUE_INDEX_DESC(indexSuffixName, fieldName)                 \
    __WCDB_CPP_ORDERED_INDEX_IMP(indexSuffixName, fieldName, WCDB::Order::DESC, true)

// Index Operation
#define WCDB_CPP_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY(indexSuffixName)           \
    __WCDB_CPP_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY_IMP(indexSuffixName)

#define WCDB_CPP_INDEX_TO_BE_DROPPED(indexSuffixName)                          \
    __WCDB_CPP_INDEX_TO_BE_DROPPED(indexSuffixName)

// Virtual Table Argument
#define WCDB_CPP_VIRTUAL_TABLE_ARGUMENT(argument)                              \
    __WCDB_CPP_VIRTUAL_TABLE_ARGUMENT_IMP(argument)

#define WCDB_CPP_VIRTUAL_TABLE_TOKENIZE(tokenizeName)                          \
    __WCDB_CPP_VIRTUAL_TABLE_ARGUMENT_IMP(                                     \
    WCDB::FTSTokenizerUtil::tokenize(tokenizeName, nullptr))

#define WCDB_CPP_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(tokenizeName, ...)     \
    __WCDB_CPP_VIRTUAL_TABLE_ARGUMENT_IMP(                                     \
    WCDB::FTSTokenizerUtil::tokenize(tokenizeName, __VA_ARGS__, nullptr))

#define WCDB_CPP_VIRTUAL_TABLE_EXTERNAL_CONTENT(tableName)                     \
    __WCDB_CPP_VIRTUAL_TABLE_ARGUMENT_IMP(                                     \
    WCDB::StatementCreateVirtualTable::externalContent(tableName))

#define WCDB_CPP_VIRTUAL_TABLE_MODULE(moduleName)                              \
    __WCDB_CPP_VIRTUAL_TABLE_MODULE_IMP(moduleName)

#define WCDB_CPP_UNINDEXED(propertyName) __WCDB_CPP_UNINDEXED_IMP(propertyName)
