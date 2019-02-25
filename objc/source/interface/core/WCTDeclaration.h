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
#import <WCDB/Macro.hpp>
#import <WCDB/WINQ.h>

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
@class WCTHandle;

#pragma mark - ORM
class WCTBinding;
class WCTColumnBinding;

@protocol WCTTableCoding;
@protocol WCTColumnCoding;

#pragma mark - WINQ
class WCTProperty;
typedef WCDB::SyntaxList<WCTProperty> WCTProperties;
class WCTResultColumn;
typedef WCDB::SyntaxList<WCTResultColumn> WCTResultColumns;

using WCTOrder = WCDB::Order;
static constexpr const WCDB::Order WCTOrderedAscending = WCDB::Order::ASC;
static constexpr const WCDB::Order WCTOrderedDescending = WCDB::Order::DESC;

using WCTColumnType = WCDB::ColumnType;
static constexpr const WCDB::ColumnType WCTColumnTypeInteger32 = WCDB::ColumnType::Integer32;
static constexpr const WCDB::ColumnType WCTColumnTypeInteger64 = WCDB::ColumnType::Integer64;
static constexpr const WCDB::ColumnType WCTColumnTypeDouble = WCDB::ColumnType::Float;
static constexpr const WCDB::ColumnType WCTColumnTypeString = WCDB::ColumnType::Text;
static constexpr const WCDB::ColumnType WCTColumnTypeData = WCDB::ColumnType::BLOB;
static constexpr const WCDB::ColumnType WCTColumnTypeNil = WCDB::ColumnType::Null;

#pragma mark - Object
typedef NSObject<WCTTableCoding> WCTObject;
typedef NSDictionary<NSString * /*table name */, WCTObject *> WCTMultiObject;

typedef NSObject<WCTColumnCoding> WCTColumnCodingValue;
typedef NSArray<NSObject<WCTColumnCoding> *> WCTColumnCodingRow;

#pragma mark - Error
@class WCTError;
@class WCTPerformanceFootprint;

typedef int32_t WCTTag;
static constexpr WCTTag const WCTInvalidTag = 0;

#pragma mark - Migration
@class WCTMigrationDatabase;
@class WCTMigrationInfo;
