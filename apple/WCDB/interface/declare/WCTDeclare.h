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
#import <WCDB/utility.hpp>
#import <map>
#import <unordered_map>

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
typedef std::list<std::shared_ptr<WCTColumnBinding>> WCTColumnBindingList;

//Case insensitive
typedef std::map<std::string,
                 std::shared_ptr<WCTColumnBinding>,
                 WCDB::CaseInsensiveComparetor>
    WCTColumnBindingMap;

class WCTIndexBinding;
typedef std::unordered_map<std::string, std::shared_ptr<WCTIndexBinding>>
    WCTIndexBindingMap;

class WCTConstraintBindingBase;
typedef std::list<std::shared_ptr<WCTConstraintBindingBase>>
    WCTConstraintBindingList;
typedef std::unordered_map<std::string,
                           std::shared_ptr<WCTConstraintBindingBase>>
    WCTConstraintBindingMap;

#pragma mark - ORM - Coding
@protocol WCTTableCoding;
@protocol WCTColumnCoding;
class WCTExpr;
class WCTExprList;
class WCTAnyProperty;
class WCTProperty;
class WCTPropertyList;
class WCTResult;
class WCTResultList;
typedef WCTExpr WCTCondition;
typedef NS_ENUM(int, WCTOrderTerm) {
    WCTOrderedAscending = (WCTOrderTerm) WCDB::OrderTerm::ASC,
    WCTOrderedDescending = (WCTOrderTerm) WCDB::OrderTerm::DESC,
    WCTOrderedNotSet = (WCTOrderTerm) WCDB::OrderTerm::NotSet,
};
typedef WCDB::Order WCTOrderBy;
typedef WCDB::OrderList WCTOrderByList;
typedef WCTExpr WCTLimit;
typedef WCTExpr WCTOffset;
typedef WCTExpr WCTGroupBy;
typedef WCTExprList WCTGroupByList;
typedef WCTExpr WCTHaving;
typedef WCDB::ColumnIndex WCTIndex;
typedef WCDB::ColumnIndexList WCTIndexList;
typedef WCDB::ColumnDef WCTColumnDef;
typedef WCDB::ColumnDefList WCTColumnDefList;
typedef WCDB::TableConstraint WCTTableConstraint;
typedef WCDB::TableConstraintList WCTTableConstraintList;
typedef NS_ENUM(int, WCTConflict) {
    WCTConflictNotSet = (WCTConflict) WCDB::Conflict::NotSet,
    WCTConflictRollback = (WCTConflict) WCDB::Conflict::Rollback,
    WCTConflictAbort = (WCTConflict) WCDB::Conflict::Abort,
    WCTConflictFail = (WCTConflict) WCDB::Conflict::Fail,
    WCTConflictIgnore = (WCTConflict) WCDB::Conflict::Ignore,
    WCTConflictReplace = (WCTConflict) WCDB::Conflict::Replace,
};
typedef NS_ENUM(int, WCTColumnType) {
    WCTColumnTypeInteger32 = (WCTColumnType) WCDB::ColumnType::Integer32,
    WCTColumnTypeInteger64 = (WCTColumnType) WCDB::ColumnType::Integer64,
    WCTColumnTypeDouble = (WCTColumnType) WCDB::ColumnType::Float,
    WCTColumnTypeString = (WCTColumnType) WCDB::ColumnType::Text,
    WCTColumnTypeBinary = (WCTColumnType) WCDB::ColumnType::BLOB,
    WCTColumnTypeNil = (WCTColumnType) WCDB::ColumnType::Null,
};
typedef NS_ENUM(int, WCTDefaultType) {
    WCTDefaultTypeCurrentTime = (WCTDefaultType)
        WCDB::ColumnDef::DefaultType::CurrentTime,
    WCTDefaultTypeCurrentDate = (WCTDefaultType)
        WCDB::ColumnDef::DefaultType::CurrentDate,
    WCTDefaultTypeCurrentTimestamp = (WCTDefaultType)
        WCDB::ColumnDef::DefaultType::CurrentTimestamp,
};

#pragma mark - StatementHandle
@class WCTStatement;
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
