//
// Created by 陈秋文 on 2022/8/28.
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

#include <WCDB/Macro.h>

#define __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                          \
    {                                                                          \
        static_assert(has_synthesized_##fieldName,                             \
                      "You need to synthesize this field before you use it");  \
        WCDB::ColumnConstraint columnConstraint;

#define __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)                              \
    WCDB::ColumnDef* columnDef                                                   \
    = g_binding->getColumnDef(WCDB::castMemberPointer(&WCDBORMType::fieldName)); \
    columnDef->constraint(columnConstraint);                                     \
    }

// primary
#define __WCDB_CPP_ORDERED_PRIMARY_IMP(fieldName, order_, autoIncrement_)      \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.primaryKey().order(order_);                               \
    WCDB_IF(autoIncrement_, columnConstraint.autoIncrement());                 \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)

#define __WCDB_CPP_PRIMARY_IMP(fieldName, autoIncrement_)                      \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.primaryKey();                                             \
    WCDB_IF(autoIncrement_, columnConstraint.autoIncrement());                 \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)

// not null
#define __WCDB_CPP_NOT_NULL_IMP(fieldName)                                     \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.notNull();                                                \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)

// unique
#define __WCDB_CPP_UNIQUE_IMP(fieldName)                                       \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.unique();                                                 \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)

// default
#define __WCDB_CPP_DEFAULT_IMP(fieldName, defaultValue)                        \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.default_(defaultValue);                                   \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)

// check
#define __WCDB_CPP_CHECK_IMP(fieldName, condition)                             \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.check(condition);                                         \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)

// unindexed for fts
#define __WCDB_CPP_UNINDEXED_IMP(fieldName)                                    \
    __WCDB_CPP_COLUMN_CONSTRAINT_BEGIN(fieldName)                              \
    columnConstraint.notIndexed();                                              \
    __WCDB_CPP_COLUMN_CONSTRAINT_END(fieldName)
