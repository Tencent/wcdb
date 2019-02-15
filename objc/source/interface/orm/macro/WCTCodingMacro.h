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

#import <WCDB/WCTMacro.h>

#import <WCDB/WCTColumnConstraintMacro.h>
#import <WCDB/WCTIndexMacro.h>
#import <WCDB/WCTPropertyMacro.h>
#import <WCDB/WCTTableConstraintMacro.h>
#import <WCDB/WCTVirtualTableMacro.h>

#define WCDB_IMPLEMENTATION(className)                                        \
    +(const WCTBinding &) objectRelationalMapping                             \
    {                                                                         \
        static const WCTBinding *s_binding = new WCTBinding(className.class); \
        WCTBinding::assertNoInheritance(className.class, self);               \
        return *s_binding;                                                    \
    }                                                                         \
    +(const WCTProperties &) allProperties                                    \
    {                                                                         \
        return [className objectRelationalMapping].getProperties();           \
    }

//Property - declare column
#define WCDB_PROPERTY(propertyName) __WCDB_PROPERTY_IMP(propertyName)

//Synthesize - implement column
#define WCDB_SYNTHESIZE(className, propertyName) \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, WCDB_STRINGIFY(propertyName))

#define WCDB_SYNTHESIZE_COLUMN(className, propertyName, columnName) \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)

//Column Constraint
#define WCDB_PRIMARY(className, propertyName) \
    __WCDB_PRIMARY_IMP(className, propertyName, false)

#define WCDB_PRIMARY_ASC(className, propertyName) \
    __WCDB_ORDERED_PRIMARY_IMP(className, propertyName, WCTOrderedAscending, false)

#define WCDB_PRIMARY_DESC(className, propertyName) \
    __WCDB_ORDERED_PRIMARY_IMP(className, propertyName, WCTOrderedDescending, false)

#define WCDB_PRIMARY_AUTO_INCREMENT(className, propertyName) \
    __WCDB_PRIMARY_IMP(className, propertyName, true)

#define WCDB_PRIMARY_ASC_AUTO_INCREMENT(className, propertyName) \
    __WCDB_ORDERED_PRIMARY_IMP(className, propertyName, WCTOrderedAscending, true)

#define WCDB_NOT_NULL(className, propertyName) \
    __WCDB_NOT_NULL_IMP(className, propertyName)

#define WCDB_UNIQUE(className, propertyName) \
    __WCDB_UNIQUE_IMP(className, propertyName)

#define WCDB_DEFAULT(className, propertyName, defaultValue) \
    __WCDB_DEFAULT_IMP(className, propertyName, defaultValue)

#define WCDB_CHECK(className, propertyName, condition) \
    __WCDB_CHECK_IMP(className, propertyName, condition)

//Table Constraint
#define WCDB_MULTI_PRIMARY(className, constraintName, propertyName) \
    __WCDB_MULTI_PRIMARY_IMP(className, constraintName, propertyName)

#define WCDB_MULTI_PRIMARY_ASC(className, constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_PRIMARY_IMP(className, constraintName, propertyName, WCTOrderedAscending)

#define WCDB_MULTI_PRIMARY_DESC(className, constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_PRIMARY_IMP(className, constraintName, propertyName, WCTOrderedDescending)

#define WCDB_MULTI_UNIQUE(className, constraintName, propertyName) \
    __WCDB_MULTI_UNIQUE_IMP(className, constraintName, propertyName)

#define WCDB_MULTI_UNIQUE_ASC(className, constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_UNIQUE_IMP(className, constraintName, propertyName, WCTOrderedAscending)

#define WCDB_MULTI_UNIQUE_DESC(className, constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_UNIQUE_IMP(className, constraintName, propertyName, WCTOrderedDescending)

//Index
#define WCDB_INDEX(className, indexSubfixName, propertyName) \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, false)

#define WCDB_INDEX_ASC(className, indexSubfixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                                    \
    className, indexSubfixName, propertyName, WCTOrderedAscending, false)

#define WCDB_INDEX_DESC(className, indexSubfixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                                     \
    className, indexSubfixName, propertyName, WCTOrderedDescending, false)

#define WCDB_UNIQUE_INDEX(className, indexSubfixName, propertyName) \
    __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, true)

#define WCDB_UNIQUE_INDEX_ASC(className, indexSubfixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                                           \
    className, indexSubfixName, propertyName, WCTOrderedAscending, true)

#define WCDB_UNIQUE_INDEX_DESC(className, indexSubfixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                                            \
    className, indexSubfixName, propertyName, WCTOrderedDescending, true)

//Virtual Table Argument
#define WCDB_VIRTUAL_TABLE_ARGUMENT(className, argument) \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, argument)

#define WCDB_VIRTUAL_TABLE_ARGUMENT_TOKENIZE_ONE_OR_BINARY(className) \
    WCDB_VIRTUAL_TABLE_ARGUMENT(                                      \
    className, WCDB::StatementCreateVirtualTable::tokenize(WCTTokenizerOneOrBinary))
#define WCDB_VIRTUAL_TABLE_ARGUMENT_TOKENIZE_LEGACY_ONE_OR_BINARY(className) \
    WCDB_VIRTUAL_TABLE_ARGUMENT(                                             \
    className, WCDB::StatementCreateVirtualTable::tokenize(WCTTokenizerLegacyOneOrBinary))

#define WCDB_VIRTUAL_TABLE_MODULE(className, moduleName) \
    __WCDB_VIRTUAL_TABLE_MODULE_IMP(className, moduleName)

#define WCDB_VIRTUAL_TABLE_MODULE_FTS3(className) \
    WCDB_VIRTUAL_TABLE_MODULE(className, WCTModuleFTS3)

#define WCDB_VIRTUAL_TABLE_MODULE_FTS5(className) \
    WCDB_VIRTUAL_TABLE_MODULE(className, WCTModuleFTS5)
