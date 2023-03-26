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

#import <Foundation/Foundation.h>

#ifdef __cplusplus

#import "WCTOptional.h"
#import "WCTTag.h"
#import "WINQ.h"

#endif // __cplusplus

#pragma mark - Chain Call
@class WCTInsert;
@class WCTDelete;
@class WCTUpdate;
@class WCTSelectable;
@class WCTSelect;
@class WCTMultiSelect;

#pragma mark - Database
@class WCTDatabase;

#pragma mark - Table
@class WCTTable;

#pragma mark - Handle
@class WCTHandle;

#pragma mark - Statement
@class WCTPreparedStatement;

#pragma mark - ORM

#ifdef __cplusplus

class WCTBinding;
class WCTColumnBinding;

#endif // __cplusplus

@protocol WCTTableCoding;
@protocol WCTColumnCoding;

#ifdef __cplusplus

#pragma mark - WINQ
class WCTProperty;
typedef WCDB::SyntaxList<WCTProperty> WCTProperties;
class WCTResultColumn;
typedef WCDB::SyntaxList<WCTResultColumn> WCTResultColumns;

using WCTOrder = WCDB::Order;
static constexpr const WCDB::Order WCTOrderedAscending = WCDB::Order::ASC;
static constexpr const WCDB::Order WCTOrderedDescending = WCDB::Order::DESC;

#endif // __cplusplus

typedef NS_ENUM(NSUInteger, WCTColumnType) {
    WCTColumnTypeNull = 0,
    WCTColumnTypeInteger,
    WCTColumnTypeFloat,
    WCTColumnTypeString,
    WCTColumnTypeData,
};

#ifdef __cplusplus
static_assert((NSUInteger) WCDB::ColumnType::Null == (NSUInteger) WCTColumnTypeNull, "");
static_assert((NSUInteger) WCDB::ColumnType::Integer == (NSUInteger) WCTColumnTypeInteger, "");
static_assert((NSUInteger) WCDB::ColumnType::Float == (NSUInteger) WCTColumnTypeFloat, "");
static_assert((NSUInteger) WCDB::ColumnType::Text == (NSUInteger) WCTColumnTypeString, "");
static_assert((NSUInteger) WCDB::ColumnType::BLOB == (NSUInteger) WCTColumnTypeData, "");
#endif // __cplusplus

#pragma mark - Object
typedef NSObject<WCTTableCoding> WCTObject;
typedef NSDictionary<NSString * /*table name */, WCTObject *> WCTMultiObject;

typedef NSObject<WCTColumnCoding> WCTColumnCodingValue;
typedef NSArray<NSObject<WCTColumnCoding> *> WCTColumnCodingRow;

#pragma mark - Error
@class WCTError;

#ifdef __cplusplus

class WCTOptionalBool;
class WCTOptionalSize;

#endif // __cplusplus

#pragma mark - Migration
@class WCTMigrationBaseInfo;
@class WCTMigrationUserInfo;
