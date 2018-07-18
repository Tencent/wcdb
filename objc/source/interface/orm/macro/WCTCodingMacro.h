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

#import <WCDB/Macro.hpp>
#import <WCDB/WCTConstraintMacro.h>
#import <WCDB/WCTIndexMacro.h>
#import <WCDB/WCTPropertyMacro.h>

//TODO support Inheritance
#define WCDB_IMPLEMENTATION(className)                                         \
    +(const WCTBinding &) objectRelationalMapping                              \
    {                                                                          \
        static const WCTBinding &s_binding                                     \
        = WCTBinding::bindingWithClass(className.class);                       \
        s_binding.checkSafeCall(className.class);                              \
        return s_binding;                                                      \
    }                                                                          \
    +(const WCTPropertyList &) allProperties                                   \
    {                                                                          \
        static const WCTPropertyList &s_properties                             \
        = WCTBinding::bindingWithClass(className.class).getAllProperties();    \
        return s_properties;                                                   \
    }                                                                          \
    +(const WCDB::Expression::All &) allResults                                \
    {                                                                          \
        return WCDB::Expression::All::default_();                              \
    }                                                                          \
    +(WCTColumnNamed) columnNamed { return WCTBinding::getColumnGenerator(); }

//Property - declare column
#define WCDB_PROPERTY(propertyName) __WCDB_PROPERTY_IMP(propertyName)

//Synthesize - implement column
#define WCDB_SYNTHESIZE(className, propertyName)                               \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, WCDB_STRINGIFY(propertyName))

#define WCDB_SYNTHESIZE_COLUMN(className, propertyName, columnName)            \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)

#define WCDB_SYNTHESIZE_DEFAULT(className, propertyName, defaultValue)         \
    __WCDB_SYNTHESIZE_DEFAULT_IMP(                                             \
    className, propertyName, WCDB_STRINGIFY(propertyName), defaultValue)

#define WCDB_SYNTHESIZE_COLUMN_DEFAULT(className, propertyName, columnName, defaultValue) \
    __WCDB_SYNTHESIZE_DEFAULT_IMP(className, propertyName, columnName, defaultValue)

//Constraint
//Column Constraint
#define WCDB_PRIMARY(className, propertyName)                                  \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedNotSet, false, WCTConflictNotSet)

#define WCDB_PRIMARY_ASC(className, propertyName)                              \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedAscending, false, WCTConflictNotSet)

#define WCDB_PRIMARY_DESC(className, propertyName)                             \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedDescending, false, WCTConflictNotSet)

#define WCDB_PRIMARY_AUTO_INCREMENT(className, propertyName)                   \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedNotSet, true, WCTConflictNotSet)

#define WCDB_PRIMARY_ASC_AUTO_INCREMENT(className, propertyName)               \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedAscending, true, WCTConflictNotSet)

#define WCDB_PRIMARY_IMP(className, propertyName, order, isAutoIncrement, conflictClause) \
    __WCDB_PRIMARY_IMP(className, propertyName, order, isAutoIncrement, conflictClause)

#define WCDB_NOT_NULL(className, propertyName)                                 \
    __WCDB_NOT_NULL_IMP(className, propertyName)

#define WCDB_UNIQUE(className, propertyName)                                   \
    __WCDB_UNIQUE_IMP(className, propertyName)

//Table Constraint
#define WCDB_MULTI_PRIMARY(className, constraintName, propertyName)            \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName, WCTOrderedNotSet)

#define WCDB_MULTI_PRIMARY_ASC(className, constraintName, propertyName)        \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName, WCTOrderedAscending)

#define WCDB_MULTI_PRIMARY_DESC(className, constraintName, propertyName)       \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName, WCTOrderedDescending)

#define WCDB_MULTI_PRIMARY_CONFLICT(className, constraintName, conflictClause) \
    __WCDB_MULTI_PRIMARY_CONFLICT_IMP(className, constraintName, conflictClause)

#define WCDB_MULTI_UNIQUE(className, constraintName, propertyName)             \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName, WCTOrderedNotSet)

#define WCDB_MULTI_UNIQUE_ASC(className, constraintName, propertyName)         \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName, WCTOrderedAscending)

#define WCDB_MULTI_UNIQUE_DESC(className, constraintName, propertyName)        \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName, WCTOrderedDescending)

#define WCDB_MULTI_UNIQUE_CONFLICT(className, constraintName, conflictClause)  \
    __WCDB_MULTI_UNIQUE_CONFLICT_IMP(className, constraintName, conflictClause)

#define WCDB_CHECK(className, constraintName, check)                           \
    __WCDB_CHECK_IMP(className, constraintName, check)

#define WCDB_FOREIGN_KEY(className, constraintName, columns, foreignKey)       \
    __WCDB_FOREIGN_KEY_IMP(className, constraintName, columns, foreignKey)

//Index
#define WCDB_INDEX(className, indexSubfixName, propertyName)                   \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, WCTOrderedNotSet, false)

#define WCDB_INDEX_ASC(className, indexSubfixName, propertyName)               \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, WCTOrderedAscending, false)

#define WCDB_INDEX_DESC(className, indexSubfixName, propertyName)              \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, WCTOrderedDescending, false)

#define WCDB_UNIQUE_INDEX(className, indexSubfixName, propertyName)            \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, WCTOrderedNotSet, true)

#define WCDB_UNIQUE_INDEX_ASC(className, indexSubfixName, propertyName)        \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, WCTOrderedAscending, true)

#define WCDB_UNIQUE_INDEX_DESC(className, indexSubfixName, propertyName)       \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, WCTOrderedDescending, true)

//Virtual Table Argument
#define WCDB_VIRTUAL_TABLE_ARGUMENT(className, left, right)                    \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, left, right)

#define WCDB_VIRTUAL_TABLE_ARGUMENT_TOKENIZE(className, tokenizeName)          \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, "tokenize", tokenizeName)

#define WCDB_VIRTUAL_TABLE_ARGUMENT_TOKENIZE_WCDB(className)                   \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, "tokenize", WCTTokenizerWCDB.cppString)

#define WCDB_VIRTUAL_TABLE_MODULE(className, moduleName)                       \
    __WCDB_VIRTUAL_TABLE_MODULE_IMP(className, moduleName)

#define WCDB_VIRTUAL_TABLE_MODULE_FTS3(className)                              \
    __WCDB_VIRTUAL_TABLE_MODULE_IMP(className, "fts3")
