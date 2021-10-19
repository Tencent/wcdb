//
// Created by sanhuazhang on 2019/05/02
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

#define WCDB_ORM_TYPE_TABLE_CONSTRAINT table_constraint

#define __WCDB_TABLE_CONSTRAINT_BEGIN(constraintName, propertyName)            \
    +(void) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_TABLE_CONSTRAINT)                    \
    {                                                                          \
        WCDB_STATIC_ASSERT_EXISTS(self.propertyName);                          \
        WCDB::TableConstraint& tableConstraint                                 \
        = binding.getOrCreateTableConstraint(constraintName);

#define __WCDB_TABLE_CONSTRAINT_END(constraintName, propertyName) }

// multi-primary
#define __WCDB_MULTI_PRIMARY_IMP(constraintName, propertyName)                 \
    __WCDB_TABLE_CONSTRAINT_BEGIN(constraintName, propertyName)                \
    tableConstraint.primaryKey().indexed(self.propertyName.asIndex());         \
    __WCDB_TABLE_CONSTRAINT_END(constraintName, propertyName)

#define __WCDB_ORDERED_MULTI_PRIMARY_IMP(constraintName, propertyName, order_)       \
    __WCDB_TABLE_CONSTRAINT_BEGIN(constraintName, propertyName)                      \
    tableConstraint.primaryKey().indexed(self.propertyName.asIndex().order(order_)); \
    __WCDB_TABLE_CONSTRAINT_END(constraintName, propertyName)

// multi-unique
#define __WCDB_MULTI_UNIQUE_IMP(constraintName, propertyName)                  \
    __WCDB_TABLE_CONSTRAINT_BEGIN(constraintName, propertyName)                \
    tableConstraint.unique().indexed(self.propertyName.asIndex());             \
    __WCDB_TABLE_CONSTRAINT_END(constraintName, propertyName)

#define __WCDB_ORDERED_MULTI_UNIQUE_IMP(constraintName, propertyName, order_)    \
    __WCDB_TABLE_CONSTRAINT_BEGIN(constraintName, propertyName)                  \
    tableConstraint.unique().indexed(self.propertyName.asIndex().order(order_)); \
    __WCDB_TABLE_CONSTRAINT_END(constraintName, propertyName)

#define __WCDB_WITHOUT_ROWID                                                   \
    +(void) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_TABLE_CONSTRAINT)                    \
    {                                                                          \
        WCDB::StatementCreateTable& statement = binding.statementTable;        \
        statement.withoutRowID();                                              \
    }
