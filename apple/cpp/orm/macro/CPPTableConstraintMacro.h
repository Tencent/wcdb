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

#define __WCDB_CPP_TABLE_CONSTRAINT_BEGIN(constraintName, fieldName)           \
    {                                                                          \
        static_assert(has_synthesized_##fieldName,                             \
                      "You need to synthesize this field before you use it");  \
        WCDB::TableConstraint& tableConstraint                                 \
        = g_binding->getOrCreateTableConstraint(constraintName);               \
        auto temp_memory_point = &WCDBORMType::fieldName;                      \
        WCDB_UNUSED(temp_memory_point)

#define __WCDB_CPP_TABLE_CONSTRAINT_END(constraintName, fieldName) }

// multi-primary
#define __WCDB_CPP_MULTI_PRIMARY_IMP(constraintName, fieldName)                \
    __WCDB_CPP_TABLE_CONSTRAINT_BEGIN(constraintName, fieldName)               \
    tableConstraint.primaryKey().indexed(_field_##fieldName.asIndex());        \
    __WCDB_CPP_TABLE_CONSTRAINT_END(constraintName, fieldName)

#define __WCDB_CPP_ORDERED_MULTI_PRIMARY_IMP(constraintName, fieldName, order_)       \
    __WCDB_CPP_TABLE_CONSTRAINT_BEGIN(constraintName, fieldName)                      \
    tableConstraint.primaryKey().indexed(_field_##fieldName.asIndex().order(order_)); \
    __WCDB_CPP_TABLE_CONSTRAINT_END(constraintName, fieldName)

// multi-unique
#define __WCDB_CPP_MULTI_UNIQUE_IMP(constraintName, fieldName)                 \
    __WCDB_CPP_TABLE_CONSTRAINT_BEGIN(constraintName, fieldName)               \
    tableConstraint.unique().indexed(_field_##fieldName.asIndex());            \
    __WCDB_CPP_TABLE_CONSTRAINT_END(constraintName, fieldName)

#define __WCDB_CPP_ORDERED_MULTI_UNIQUE_IMP(constraintName, fieldName, order_)    \
    __WCDB_CPP_TABLE_CONSTRAINT_BEGIN(constraintName, fieldName)                  \
    tableConstraint.unique().indexed(_field_##fieldName.asIndex().order(order_)); \
    __WCDB_CPP_TABLE_CONSTRAINT_END(constraintName, fieldName)

#define __WCDB_CPP_WITHOUT_ROWID                                               \
    {                                                                          \
        WCDB::StatementCreateTable& statement = binding.statementTable;        \
        statement.withoutRowID();                                              \
    }
