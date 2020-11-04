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

#import <WCDB/Macro.h>

#import <WCDB/WCTMacroUtility.h>

#import <WCDB/WCTColumnConstraintMacro.h>
#import <WCDB/WCTIndexMacro.h>
#import <WCDB/WCTPropertyMacro.h>
#import <WCDB/WCTTableConstraintMacro.h>
#import <WCDB/WCTVirtualTableMacro.h>

#define WCDB_IMPLEMENTATION(className)                                        \
    +(const WCTBinding &) objectRelationalMapping                             \
    {                                                                         \
        static const WCTBinding *s_binding = new WCTBinding(className.class); \
        return *s_binding;                                                    \
    }                                                                         \
    +(const WCTProperties &) allProperties                                    \
    {                                                                         \
        return [className objectRelationalMapping].getProperties();           \
    }                                                                         \
    +(className *) WCDB_ORM_TYPER                                             \
    {                                                                         \
        return nil;                                                           \
    }

// Property - declare column
#define WCDB_PROPERTY(propertyName) __WCDB_PROPERTY_IMP(propertyName)

// Synthesize - implement column
#define WCDB_SYNTHESIZE(propertyName) \
    __WCDB_SYNTHESIZE_IMP(propertyName, propertyName, WCDB_STRINGIFY(propertyName))

#define WCDB_SYNTHESIZE_COLUMN(propertyName, columnName) \
    __WCDB_SYNTHESIZE_IMP(propertyName, propertyName, columnName)

#define WCDB_SYNTHESIZE_CUSTOM(WINQName, propertyName, columnName) \
    __WCDB_SYNTHESIZE_IMP(WINQName, propertyName, columnName)

// Column Constraint
#define WCDB_PRIMARY(propertyName) \
    __WCDB_PRIMARY_IMP(propertyName, false)

#define WCDB_PRIMARY_ASC(propertyName) \
    __WCDB_ORDERED_PRIMARY_IMP(propertyName, WCTOrderedAscending, false)

#define WCDB_PRIMARY_DESC(propertyName) \
    __WCDB_ORDERED_PRIMARY_IMP(propertyName, WCTOrderedDescending, false)

#define WCDB_PRIMARY_AUTO_INCREMENT(propertyName) \
    __WCDB_PRIMARY_IMP(propertyName, true)

#define WCDB_PRIMARY_ASC_AUTO_INCREMENT(propertyName) \
    __WCDB_ORDERED_PRIMARY_IMP(propertyName, WCTOrderedAscending, true)

#define WCDB_NOT_NULL(propertyName) \
    __WCDB_NOT_NULL_IMP(propertyName)

#define WCDB_UNIQUE(propertyName) \
    __WCDB_UNIQUE_IMP(propertyName)

#define WCDB_DEFAULT(propertyName, defaultValue) \
    __WCDB_DEFAULT_IMP(propertyName, defaultValue)

#define WCDB_CHECK(propertyName, condition) \
    __WCDB_CHECK_IMP(propertyName, condition)

//For FTS
#define WCDB_NOTINDEXED(propertyName) \
    __WCDB_NOTINDEXED_IMP(propertyName)

// Table Constraint
#define WCDB_WITHOUT_ROWID \
    __WCDB_WITHOUT_ROWID

#define WCDB_MULTI_PRIMARY(constraintName, propertyName) \
    __WCDB_MULTI_PRIMARY_IMP(constraintName, propertyName)

#define WCDB_MULTI_PRIMARY_ASC(constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_PRIMARY_IMP(constraintName, propertyName, WCTOrderedAscending)

#define WCDB_MULTI_PRIMARY_DESC(constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_PRIMARY_IMP(constraintName, propertyName, WCTOrderedDescending)

#define WCDB_MULTI_UNIQUE(constraintName, propertyName) \
    __WCDB_MULTI_UNIQUE_IMP(constraintName, propertyName)

#define WCDB_MULTI_UNIQUE_ASC(constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_UNIQUE_IMP(constraintName, propertyName, WCTOrderedAscending)

#define WCDB_MULTI_UNIQUE_DESC(constraintName, propertyName) \
    __WCDB_ORDERED_MULTI_UNIQUE_IMP(constraintName, propertyName, WCTOrderedDescending)

// Index
#define WCDB_INDEX(indexSuffixName, propertyName) \
    __WCDB_INDEX_IMP(indexSuffixName, propertyName, false)

#define WCDB_INDEX_ASC(indexSuffixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                         \
    indexSuffixName, propertyName, WCTOrderedAscending, false)

#define WCDB_INDEX_DESC(indexSuffixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                          \
    indexSuffixName, propertyName, WCTOrderedDescending, false)

#define WCDB_UNIQUE_INDEX(indexSuffixName, propertyName) \
    __WCDB_INDEX_IMP(indexSuffixName, propertyName, true)

#define WCDB_UNIQUE_INDEX_ASC(indexSuffixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                                \
    indexSuffixName, propertyName, WCTOrderedAscending, true)

#define WCDB_UNIQUE_INDEX_DESC(indexSuffixName, propertyName) \
    __WCDB_ORDERED_INDEX_IMP(                                 \
    indexSuffixName, propertyName, WCTOrderedDescending, true)

// Index Operation
#define WCDB_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY(indexSuffixName) \
    __WCDB_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY_IMP(indexSuffixName)

#define WCDB_INDEX_TO_BE_DROPPED(indexSuffixName) \
    __WCDB_INDEX_TO_BE_DROPPED(indexSuffixName)

// Virtual Table Argument
#define WCDB_VIRTUAL_TABLE_ARGUMENT(argument) \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(argument)

#define WCDB_VIRTUAL_TABLE_TOKENIZE(tokenizeName) \
    __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(WCDB::StatementCreateVirtualTable::tokenize(tokenizeName))

#define WCDB_VIRTUAL_TABLE_MODULE(moduleName) \
    __WCDB_VIRTUAL_TABLE_MODULE_IMP(moduleName)
