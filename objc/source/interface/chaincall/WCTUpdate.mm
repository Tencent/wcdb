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

#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTUpdate {
    WCDB::StatementUpdate _statement;
    WCTPropertyList _properties;
}

- (instancetype)table:(NSString *)tableName
{
    WCTRemedialAssert(tableName, "Table name can't be null.", return self;);
    _statement.update(tableName.cppString);
    return self;
}

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _properties = properties;
    int bindParameterIndex = 0;
    for (const WCTProperty &property : properties) {
        _statement.set(property, WCDB::BindParameter(++bindParameterIndex));
    }
    return self;
}

- (instancetype)where:(const WCDB::Expression &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    _statement.orderBy(orders);
    return self;
}

- (instancetype)limit:(const WCDB::Expression &)limit
{
    _statement.limit(limit);
    return self;
}

- (instancetype)offset:(const WCDB::Expression &)offset
{
    _statement.offset(offset);
    return self;
}

- (BOOL)executeWithObject:(WCTObject *)object
{
    if (object == nil) {
        return YES;
    }
    BOOL result;
    if (_properties.empty()) {
        result = [self execute:_statement withObject:object];
    } else {
        result = [self execute:_statement withObject:object onProperties:_properties];
    }
    [self doAutoFinalize:!result];
    return result;
}

- (BOOL)executeWithValue:(WCTColumnCodingValue *)value
{
    if (value == nil) {
        return YES;
    }
    BOOL result = [self execute:_statement withValue:value];
    [self doAutoFinalize:!result];
    return result;
}

- (BOOL)executeWithRow:(WCTOneRow *)row
{
    if (row.count == 0) {
        return YES;
    }
    BOOL result = [self execute:_statement withRow:row];
    [self doAutoFinalize:!result];
    return result;
}

- (WCDB::StatementUpdate &)statement
{
    return _statement;
}

@end
