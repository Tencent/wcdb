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
#define __WCDB_PRIMARY_IMP(className, propertyName, order, autoIncrement,      \
                           conflictClause)                                     \
    WCDB_IF(autoIncrement, @synthesize isAutoIncrement;)                       \
    WCDB_IF(autoIncrement, @synthesize lastInsertedRowID;)                     \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]->addColumnConstraint(       \
            WCDB::ColumnConstraint()                                           \
                .withPrimaryKey(order, autoIncrement)                          \
                .onConflict(conflictClause),                                   \
            className.propertyName);                                           \
        return nullptr;                                                        \
    }();

#define __WCDB_NOT_NULL_IMP(className, propertyName)                           \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]->addColumnConstraint(       \
            WCDB::ColumnConstraint().withNotNull(), className.propertyName);   \
        return nullptr;                                                        \
    }();

#define __WCDB_UNIQUE_IMP(className, propertyName)                             \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]->addColumnConstraint(       \
            WCDB::ColumnConstraint().withUnique(), className.propertyName);    \
        return nullptr;                                                        \
    }();

//Table Constraint
#define __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName,      \
                                 order)                                        \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]                             \
            ->getOrCreateTableConstraint(constraintName)                       \
            ->withPrimaryKey(className.propertyName.asIndex(order));           \
        return nullptr;                                                        \
    }();

#define __WCDB_MULTI_PRIMARY_CONFLICT_IMP(className, constraintName, conflict) \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]                             \
            ->getOrCreateTableConstraint(constraintName)                       \
            ->onConflict(conflict);                                            \
        return nullptr;                                                        \
    }();

#define __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName,       \
                                order)                                         \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]                             \
            ->getOrCreateTableConstraint(constraintName)                       \
            ->withUnique(className.propertyName.asIndex(order));               \
        return nullptr;                                                        \
    }();

#define __WCDB_MULTI_UNIQUE_CONFLICT_IMP(className, constraintName, conflict)  \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]                             \
            ->getOrCreateTableConstraint(constraintName)                       \
            ->onConflict(conflict);                                            \
        return nullptr;                                                        \
    }();

#define __WCDB_CHECK_IMP(className, constraintName, expr)                      \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]                             \
            ->getOrCreateTableConstraint(constraintName)                       \
            ->withChecking(expr);                                              \
        return nullptr;                                                        \
    }();

#define __WCDB_FOREIGN_KEY_IMP(className, constraintName, columns, foreignKey) \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]                             \
            ->getOrCreateTableConstraint(constraintName)                       \
            ->withForeignKey(columns, foreignKey);                             \
        return nullptr;                                                        \
    }();
