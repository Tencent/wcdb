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

#import <WCDB/WCTBinding.h>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTUpdate.h>
#import <WCDB/handle_statement.hpp>
#import <WCDB/in_case_lock_guard.hpp>
#import <WCDB/utility.hpp>

@implementation WCTUpdate {
    WCDB::StatementUpdate _statement;
    WCTPropertyList _propertyList;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andProperties:(const WCTPropertyList &)propertyList andTableName:(NSString *)tableName
{
    if (self = [super initWithCore:core]) {
        _statement.update(tableName.UTF8String);
        _propertyList.insert(_propertyList.begin(), propertyList.begin(), propertyList.end());
        WCDB::UpdateValueList updateValueList;
        for (const WCTProperty &property : propertyList) {
            const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
            if (columnBinding) {
                updateValueList.push_back({property, WCTExpr::BindParameter});
            } else {
                WCDB::Error::ReportInterface(_core->getTag(),
                                             _core->getPath(),
                                             WCDB::Error::InterfaceOperation::Update,
                                             WCDB::Error::InterfaceCode::ORM,
                                             [NSString stringWithFormat:@"Updating [%@] with an unknown column [%s]", tableName, columnBinding->columnName.c_str()].UTF8String,
                                             &_error);
            }
        }
        _statement.set(updateValueList);
    }
    return self;
}

- (instancetype)where:(const WCTCondition &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orderBy:(const WCTOrderByList &)orderList
{
    _statement.orderBy(orderList);
    return self;
}

- (instancetype)limit:(const WCTLimit &)limit
{
    _statement.limit(limit);
    return self;
}

- (instancetype)offset:(const WCTOffset &)offset
{
    _statement.offset(offset);
    return self;
}

- (BOOL)executeWithObject:(WCTObject *)object
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    if (!_error.isOK()) {
        return NO;
    }
    if (!object) {
        WCDB::Error::Warning("Updating with a nil object");
        return YES;
    }
    Class cls = object.class;
    if (![cls conformsToProtocol:@protocol(WCTTableCoding)]) {
        WCDB::Error::ReportInterface(_core->getTag(),
                                     _core->getPath(),
                                     WCDB::Error::InterfaceOperation::Update,
                                     WCDB::Error::InterfaceCode::ORM,
                                     [NSString stringWithFormat:@"%@ should conform to protocol WCTTableCoding", NSStringFromClass(cls)].UTF8String,
                                     &_error);
        return NO;
    }
    WCDB::RecyclableStatement statementHandle = _core->prepare(_statement, _error);
    if (!statementHandle) {
        return NO;
    }
    int index = 1;
    for (const WCTProperty &property : _propertyList) {
        if (![self bindProperty:property
                         ofObject:object
                toStatementHandle:statementHandle
                          atIndex:index
                        withError:_error]) {
            return NO;
        }
        ++index;
    }
    statementHandle->step();
    _error = statementHandle->getError();
    return _error.isOK();
}

- (BOOL)executeWithRow:(WCTOneRow *)row
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    if (!_error.isOK()) {
        return NO;
    }
    if (row.count == 0) {
        WCDB::Error::Warning("Updating with a empty row");
        return YES;
    }

    WCDB::RecyclableStatement statementHandle = _core->prepare(_statement, _error);
    if (!statementHandle) {
        return NO;
    }
    int index = 1;
    for (WCTValue *value in row) {
        if (![self bindWithValue:value
                toStatementHandle:statementHandle
                          atIndex:index
                        withError:_error]) {
            return NO;
        }
        ++index;
    }
    statementHandle->step();
    _error = statementHandle->getError();
    return _error.isOK();
}

@end
