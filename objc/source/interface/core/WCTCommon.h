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
#import <WCDB/WINQ.h>

#pragma mark - Chain Call
@class WCTInsert;
@class WCTDelete;
@class WCTUpdate;
@class WCTSelectable;
@class WCTSelect;
@class WCTRowSelect;
@class WCTMultiSelect;

#pragma mark - Database
@class WCTDatabase;

#pragma mark - Table
@class WCTTable;

#pragma mark - Handle
@class WCTUnsafeHandle;
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
constexpr const WCDB::Order WCTOrderedAscending = WCDB::Order::ASC;
constexpr const WCDB::Order WCTOrderedDescending = WCDB::Order::DESC;

constexpr const WCDB::Conflict WCTConflictRollback = WCDB::Conflict::Rollback;
constexpr const WCDB::Conflict WCTConflictAbort = WCDB::Conflict::Abort;
constexpr const WCDB::Conflict WCTConflictFail = WCDB::Conflict::Fail;
constexpr const WCDB::Conflict WCTConflictIgnore = WCDB::Conflict::Ignore;
constexpr const WCDB::Conflict WCTConflictReplace = WCDB::Conflict::Replace;

using WCTColumnType = WCDB::ColumnType;
constexpr const WCDB::ColumnType WCTColumnTypeInteger32 = WCDB::ColumnType::Integer32;
constexpr const WCDB::ColumnType WCTColumnTypeInteger64 = WCDB::ColumnType::Integer64;
constexpr const WCDB::ColumnType WCTColumnTypeFloat = WCDB::ColumnType::Float;
constexpr const WCDB::ColumnType WCTColumnTypeString = WCDB::ColumnType::Text;
constexpr const WCDB::ColumnType WCTColumnTypeData = WCDB::ColumnType::BLOB;
constexpr const WCDB::ColumnType WCTColumnTypeNil = WCDB::ColumnType::Null;

#pragma mark - Object
typedef NSObject<WCTTableCoding> WCTObject;
typedef NSDictionary<NSString *, WCTObject *>
WCTMultiObject; //table name -> object

typedef NSObject<WCTColumnCoding> WCTColumnCodingValue;
typedef NSArray<NSObject<WCTColumnCoding> *> WCTColumnCodingRow;

#pragma mark - Error
@class WCTError;
@class WCTPerformanceFootprint;

#pragma mark - Migration
@class WCTMigrationDatabase;
@class WCTMigrationInfo;
