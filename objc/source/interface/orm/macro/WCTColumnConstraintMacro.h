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

#define __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName)         \
    +(void) WCDB_ORM_UNIQUE(className, WCDB_ORM_TYPE_COLUMN_CONSTRAINT) \
    {                                                                   \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);              \
        WCDB::ColumnConstraint columnConstraint;

#define __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)                          \
    binding.getOrCreateColumnDef(className.propertyName).constraint(columnConstraint); \
    }

// primary
#define __WCDB_ORDERED_PRIMARY_IMP(className, propertyName, order_, autoIncrement_) \
    WCDB_IF(autoIncrement_, @synthesize isAutoIncrement;)                           \
    WCDB_IF(autoIncrement_, @synthesize lastInsertedRowID;)                         \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName)                         \
    columnConstraint.primaryKey().order(order_);                                    \
    WCDB_IF(autoIncrement_, columnConstraint.autoIncrement());                      \
    __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)

#define __WCDB_PRIMARY_IMP(className, propertyName, autoIncrement_) \
    WCDB_IF(autoIncrement_, @synthesize isAutoIncrement;)           \
    WCDB_IF(autoIncrement_, @synthesize lastInsertedRowID;)         \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName)         \
    columnConstraint.primaryKey();                                  \
    WCDB_IF(autoIncrement_, columnConstraint.autoIncrement());      \
    __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)

// not null
#define __WCDB_NOT_NULL_IMP(className, propertyName)        \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName) \
    columnConstraint.notNull();                             \
    __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)

// unique
#define __WCDB_UNIQUE_IMP(className, propertyName)          \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName) \
    columnConstraint.unique();                              \
    __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)

// default
#define __WCDB_DEFAULT_IMP(className, propertyName, defaultValue) \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName)       \
    columnConstraint.default_(defaultValue);                      \
    __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)

// check
#define __WCDB_CHECK_IMP(className, propertyName, condition) \
    __WCDB_COLUMN_CONSTRAINT_BEGIN(className, propertyName)  \
    columnConstraint.check(condition);                       \
    __WCDB_COLUMN_CONSTRAINT_END(className, propertyName)
