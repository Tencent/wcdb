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

//Column Constraint
#define __WCDB_PRIMARY_IMP(className, propertyName, order, autoIncrement, conflictClause) \
    WCDB_IF(autoIncrement, @synthesize isAutoIncrement;)                                  \
    WCDB_IF(autoIncrement, @synthesize lastInsertedRowID;)                                \
    +(void) WCDB_ORM(className, primary)                                                  \
    {                                                                                     \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);                                \
        const WCTProperty &property = binding.getProperty(WCDB_STRINGIFY(propertyName));  \
        binding.getColumnDef(property).constraint(                                        \
        WCDB::ColumnConstraint()                                                          \
        .withPrimaryKey(order, autoIncrement)                                             \
        .onConflict(conflictClause));                                                     \
    }

#define __WCDB_NOT_NULL_IMP(className, propertyName)                                     \
    +(void) WCDB_ORM(className, not_null)                                                \
    {                                                                                    \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);                               \
        const WCTProperty &property = binding.getProperty(WCDB_STRINGIFY(propertyName)); \
        binding.getColumnDef(property).constraint(                                       \
        WCDB::ColumnConstraint().withNotNull());                                         \
    }

#define __WCDB_UNIQUE_IMP(className, propertyName)                                       \
    +(void) WCDB_ORM(className, unique)                                                  \
    {                                                                                    \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);                               \
        const WCTProperty &property = binding.getProperty(WCDB_STRINGIFY(propertyName)); \
        binding.getColumnDef(property).constraint(                                       \
        WCDB::ColumnConstraint().withUnique());                                          \
    }

//Table Constraint
#define __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName, order)         \
    +(void) WCDB_ORM(className, multi_primary)                                           \
    {                                                                                    \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);                               \
        const WCTProperty &property = binding.getProperty(WCDB_STRINGIFY(propertyName)); \
        binding.getOrCreateTableConstraint(constraintName)                               \
        .withPrimaryKey(property.asIndex(order));                                        \
    }

#define __WCDB_MULTI_PRIMARY_CONFLICT_IMP(className, constraintName, conflict) \
    +(void) WCDB_ORM(className, multi_primary)                                 \
    {                                                                          \
        binding.getOrCreateTableConstraint(constraintName)                     \
        .onConflict(conflict);                                                 \
    }

#define __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName, order)          \
    +(void) WCDB_ORM(className, multi_unique)                                            \
    {                                                                                    \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);                               \
        const WCTProperty &property = binding.getProperty(WCDB_STRINGIFY(propertyName)); \
        binding.getOrCreateTableConstraint(constraintName)                               \
        .withUnique(property.asIndex(order));                                            \
    }

#define __WCDB_MULTI_UNIQUE_CONFLICT_IMP(className, constraintName, conflict) \
    +(void) WCDB_ORM(className, multi_unique)                                 \
    {                                                                         \
        binding.getOrCreateTableConstraint(constraintName)                    \
        .onConflict(conflict);                                                \
    }

#define __WCDB_CHECK_IMP(className, constraintName, expr)               \
    +(void) WCDB_ORM(className, check)                                  \
    {                                                                   \
        binding.getOrCreateTableConstraint(constraintName).check(expr); \
    }

#define __WCDB_FOREIGN_KEY_IMP(className, constraintName, columns, foreignKey) \
    +(void) WCDB_ORM(className, foreign_key)                                   \
    {                                                                          \
        binding.getOrCreateTableConstraint(constraintName)                     \
        .withForeignKey(columns, foreignKey);                                  \
    }
