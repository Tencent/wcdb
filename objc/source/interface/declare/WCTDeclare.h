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
#import <WCDB/abstract.h>
#import <WCDB/core.h>
#import <WCDB/utility.hpp>

#pragma mark - Chain Call
@class WCTInsert;
@class WCTDelete;
@class WCTUpdate;
@class WCTSelectBase;
@class WCTSelect;
@class WCTRowSelect;
@class WCTMultiSelect;

#pragma mark - Database
@class WCTDatabase;
typedef int WCTTag;

#pragma mark - Table
@class WCTTable;

#pragma mark - ORM - Binding
class WCTBinding;
class WCTColumnBinding;

#pragma mark - ORM - Coding
@protocol WCTTableCoding;
@protocol WCTColumnCoding;

class WCTProperty;
class WCTPropertyList;

using WCTOrder = WCDB::Order;
constexpr const WCDB::Order WCTOrderedAscending = WCDB::Order::ASC;
constexpr const WCDB::Order WCTOrderedDescending = WCDB::Order::DESC;
constexpr const WCDB::Order WCTOrderedNotSet = WCDB::Order::NotSet;

constexpr const WCDB::ConflictClause WCTConflictNotSet =
    WCDB::ConflictClause::NotSet;
constexpr const WCDB::ConflictClause WCTConflictRollback =
    WCDB::ConflictClause::Rollback;
constexpr const WCDB::ConflictClause WCTConflictAbort =
    WCDB::ConflictClause::Abort;
constexpr const WCDB::ConflictClause WCTConflictFail =
    WCDB::ConflictClause::Fail;
constexpr const WCDB::ConflictClause WCTConflictIgnore =
    WCDB::ConflictClause::Ignore;
constexpr const WCDB::ConflictClause WCTConflictReplace =
    WCDB::ConflictClause::Replace;

using WCTColumnType = WCDB::ColumnType;
constexpr const WCDB::ColumnType WCTColumnTypeInteger32 =
    WCDB::ColumnType::Integer32;
constexpr const WCDB::ColumnType WCTColumnTypeInteger64 =
    WCDB::ColumnType::Integer64;
constexpr const WCDB::ColumnType WCTColumnTypeDouble = WCDB::ColumnType::Float;
constexpr const WCDB::ColumnType WCTColumnTypeString = WCDB::ColumnType::Text;
constexpr const WCDB::ColumnType WCTColumnTypeBinary = WCDB::ColumnType::BLOB;
constexpr const WCDB::ColumnType WCTColumnTypeNull = WCDB::ColumnType::Null;

#pragma mark - HandleStatement
@class WCTCoreStatement;
typedef NSObject<WCTTableCoding> WCTObject;
typedef NSDictionary<NSString *, WCTObject *>
    WCTMultiObject;        //table name -> object
typedef NSObject WCTValue; //NSData, NSNumber, NSString, NSNull
typedef NS_ENUM(int, WCTValueType) {
    WCTValueTypeUnknown = -1,
    WCTValueTypeNil = 0, //NSNull or nil.
    WCTValueTypeString,  //NSString
    WCTValueTypeNumber,  //NSNumber
    WCTValueTypeData,    //NSData
    WCTValueTypeColumnCoding, //id<WCTColumnCoding>, only for passing WCTValue to WCDB
};
typedef NSArray<WCTValue *> WCTOneRow;
typedef NSArray<WCTValue *> WCTOneColumn;
typedef NSArray<NSArray<WCTValue *> *> WCTColumnsXRows;

@class WCTError;
