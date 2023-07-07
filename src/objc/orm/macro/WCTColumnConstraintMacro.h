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

#define WCDB_ORM_TYPE_COLUMN_CONSTRAINT column_constraint

#define __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName)         \
    +(void) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_COLUMN_CONSTRAINT) \
    {                                                        \
        WCDB_STATIC_ASSERT_EXISTS(self.propertyName);        \
        WCDB::ColumnConstraint columnConstraint;

#define __WCDB_COLUMN_CONSTRAINT_END(propertyName)                         \
    binding.getColumnDef(self.propertyName)->constraint(columnConstraint); \
    }

// primary
#define __WCDB_ORDERED_PRIMARY_IMP(propertyName, order_, autoIncrement_) \
    WCDB_IF(autoIncrement_, @synthesize isAutoIncrement;)                \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName)                         \
    columnConstraint.primaryKey().order(order_);                         \
    WCDB_IF(autoIncrement_, columnConstraint.autoIncrement());           \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)

#define __WCDB_PRIMARY_IMP(propertyName, autoIncrement_)       \
    WCDB_IF(autoIncrement_, @synthesize isAutoIncrement;)      \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName)               \
    columnConstraint.primaryKey();                             \
    WCDB_IF(autoIncrement_, columnConstraint.autoIncrement()); \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)

#define __WCDB_PRIMARY_ENABLE_AUTOINCREMENT_FOR_EXISTING_TABLE \
    +(void) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_COLUMN_CONSTRAINT)   \
    {                                                          \
        binding.enableAutoIncrementForExistingTable();         \
    }

// not null
#define __WCDB_NOT_NULL_IMP(propertyName)        \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName) \
    columnConstraint.notNull();                  \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)

// unique
#define __WCDB_UNIQUE_IMP(propertyName)          \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName) \
    columnConstraint.unique();                   \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)

// default
#define __WCDB_DEFAULT_IMP(propertyName, defaultValue) \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName)       \
    columnConstraint.default_(defaultValue);           \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)

// check
#define __WCDB_CHECK_IMP(propertyName, condition) \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName)  \
    columnConstraint.check(condition);            \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)

// unindexed for fts
#define __WCDB_UNINDEXED_IMP(propertyName)       \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(propertyName) \
    columnConstraint.unIndexed();                \
    __WCDB_COLUMN_CONSTRAINT_END(propertyName)
