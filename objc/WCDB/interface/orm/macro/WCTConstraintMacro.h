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
                           onConflict)                                         \
    WCDB_IF(autoIncrement, @synthesize isAutoIncrement;)                       \
    WCDB_IF(autoIncrement, @synthesize lastInsertedRowID;)                     \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->getColumnBinding(className.propertyName)                      \
            ->makePrimary(order, autoIncrement, onConflict);                   \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_NOT_NULL_IMP(className, propertyName)                           \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->getColumnBinding(className.propertyName)->makeNotNull();      \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_UNIQUE_IMP(className, propertyName)                             \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->getColumnBinding(className.propertyName)->makeUnique();       \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

//Table Constraint
#define __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName,      \
                                 order)                                        \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding                                                                \
            ->getOrCreateConstraintBinding<WCTConstraintPrimaryKeyBinding>(    \
                constraintName)                                                \
            ->addPrimaryKey(className.propertyName.index(order));              \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_MULTI_PRIMARY_CONFLICT_IMP(className, constraintName, conflict) \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding                                                                \
            ->getOrCreateConstraintBinding<WCTConstraintPrimaryKeyBinding>(    \
                constraintName)                                                \
            ->setConflict(conflict);                                           \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName,       \
                                order)                                         \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding                                                                \
            ->getOrCreateConstraintBinding<WCTConstraintUniqueBinding>(        \
                constraintName)                                                \
            ->addUnique(className.propertyName.index(order));                  \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_MULTI_UNIQUE_CONFLICT_IMP(className, constraintName, conflict)  \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding                                                                \
            ->getOrCreateConstraintBinding<WCTConstraintUniqueBinding>(        \
                constraintName)                                                \
            ->setConflict(conflict);                                           \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_CHECK_IMP(className, cosntraintName, expr)                      \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding                                                                \
            ->getOrCreateConstraintBinding<WCTConstraintCheckBinding>(         \
                constraintName)                                                \
            ->makeCheck(expr);                                                 \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));
