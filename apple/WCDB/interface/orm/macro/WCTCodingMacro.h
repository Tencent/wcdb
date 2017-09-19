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

#import <WCDB/WCTConstraintMacro.h>
#import <WCDB/WCTIndexMacro.h>
#import <WCDB/WCTMacroHelper.h>
#import <WCDB/WCTPropertyMacro.h>
#import <WCDB/macro.hpp>

#define __WCDB_BINDING(className) _s_##className##_binding
#define __WCDB_PROPERTIES(className) _s_##className##_properties

#define WCDB_IMPLEMENTATION(className)                                         \
    static WCTBinding __WCDB_BINDING(className)(className.class);              \
    static WCTPropertyList __WCDB_PROPERTIES(className);                       \
    +(const WCTBinding *) objectRelationalMappingForWCDB                       \
    {                                                                          \
        if (self.class != className.class) {                                   \
            WCDB::Error::Abort("Inheritance is not supported for ORM");        \
        }                                                                      \
        return &__WCDB_BINDING(className);                                     \
    }                                                                          \
    +(const WCTPropertyList &) AllProperties                                   \
    {                                                                          \
        return __WCDB_PROPERTIES(className);                                   \
    }                                                                          \
    +(const WCTAnyProperty &) AnyProperty                                      \
    {                                                                          \
        static const WCTAnyProperty s_anyProperty(className.class);            \
        return s_anyProperty;                                                  \
    }                                                                          \
    +(WCTPropertyNamed) PropertyNamed { return WCTProperty::PropertyNamed; }

//Property - declare column
#define WCDB_PROPERTY(propertyName) __WCDB_PROPERTY_IMP(propertyName)

//Synthesize - implement column
#define WCDB_SYNTHESIZE(className, propertyName)                               \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, WCDB_STRINGIFY(propertyName))

#define WCDB_SYNTHESIZE_COLUMN(className, propertyName, columnName)            \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)

#define WCDB_SYNTHESIZE_DEFAULT(className, propertyName, defaultValue)         \
    __WCDB_SYNTHESIZE_DEFAULT_IMP(className, propertyName,                     \
                                  WCDB_STRINGIFY(propertyName), defaultValue)

#define WCDB_SYNTHESIZE_COLUMN_DEFAULT(className, propertyName, columnName,    \
                                       defaultValue)                           \
    __WCDB_SYNTHESIZE_DEFAULT_IMP(className, propertyName, columnName,         \
                                  defaultValue)

//Constraint
//Column Constraint
#define WCDB_PRIMARY(className, propertyName)                                  \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedNotSet, false,       \
                       WCTConflictNotSet)

#define WCDB_PRIMARY_ASC(className, propertyName)                              \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedAscending, false,    \
                       WCTConflictNotSet)

#define WCDB_PRIMARY_DESC(className, propertyName)                             \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedDescending, false,   \
                       WCTConflictNotSet)

#define WCDB_PRIMARY_AUTO_INCREMENT(className, propertyName)                   \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedNotSet, true,        \
                       WCTConflictNotSet)

#define WCDB_PRIMARY_ASC_AUTO_INCREMENT(className, propertyName)               \
    __WCDB_PRIMARY_IMP(className, propertyName, WCTOrderedAscending, true,     \
                       WCTConflictNotSet)

#define WCDB_PRIMARY_IMP(className, propertyName, order, isAutoIncrement,      \
                         onConflict)                                           \
    __WCDB_PRIMARY_IMP(className, propertyName, order, isAutoIncrement,        \
                       onConflict)

#define WCDB_NOT_NULL(className, propertyName)                                 \
    __WCDB_NOT_NULL_IMP(className, propertyName)

#define WCDB_UNIQUE(className, propertyName)                                   \
    __WCDB_UNIQUE_IMP(className, propertyName)

//Table Constraint
#define WCDB_MULTI_PRIMARY(className, constraintName, propertyName)            \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName,          \
                             WCTOrderedNotSet)

#define WCDB_MULTI_PRIMARY_ASC(className, constraintName, propertyName)        \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName,          \
                             WCTOrderedAscending)

#define WCDB_MULTI_PRIMARY_DESC(className, constraintName, propertyName)       \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName,          \
                             WCTOrderedDescending)

#define WCDB_MULTI_PRIMARY_CONFLICT(className, constraintName, onConflict)     \
    __WCDB_MULTI_PRIMARY_CONFLICT_IMP(className, constraintName, onConflict)

#define WCDB_MULTI_UNIQUE(className, constraintName, propertyName)             \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName,           \
                            WCTOrderedNotSet)

#define WCDB_MULTI_UNIQUE_ASC(className, constraintName, propertyName)         \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName,           \
                            WCTOrderedAscending)

#define WCDB_MULTI_UNIQUE_DESC(className, constraintName, propertyName)        \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName,           \
                            WCTOrderedDescending)

#define WCDB_MULTI_UNIQUE_CONFLICT(className, constraintName, onConflict)      \
    __WCDB_MULTI_UNIQUE_CONFLICT_IMP(className, constraintName, onConflict)

#define WCDB_CHECK(className, constraintName, check)                           \
    __WCDB_CHECK_IMP(className, constraintName, check)

//Index
#define WCDB_INDEX(className, indexSubfixName, propertyName)                   \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName,                 \
                     WCTOrderedNotSet, false)

#define WCDB_INDEX_ASC(className, indexSubfixName, propertyName)               \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName,                 \
                     WCTOrderedAscending, false)

#define WCDB_INDEX_DESC(className, indexSubfixName, propertyName)              \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName,                 \
                     WCTOrderedDescending, false)

#define WCDB_UNIQUE_INDEX(className, indexSubfixName, propertyName)            \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName,                 \
                     WCTOrderedNotSet, true)

#define WCDB_UNIQUE_INDEX_ASC(className, indexSubfixName, propertyName)        \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName,                 \
                     WCTOrderedAscending, true)

#define WCDB_UNIQUE_INDEX_DESC(className, indexSubfixName, propertyName)       \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName,                 \
                     WCTOrderedDescending, true)

//Virtual Table Argument
#define WCDB_VIRTUAL_TABLE_ARGUMENT(className, left, right)                    \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, left, right)

#define WCDB_VIRTUAL_TABLE_TOKENIZE(className, tokenizeName)                   \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, "tokenize", tokenizeName)

#define WCDB_VIRTUAL_TABLE_MODULE(className, moduleName)                       \
    __WCDB_VIRTUAL_TABLE_MODULE_IMP(className, moduleName)
