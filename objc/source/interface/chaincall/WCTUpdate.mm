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

#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTUpdate {
    WCDB::StatementUpdate _statement;
    WCTPropertyList _properties;
}

- (instancetype)table:(NSString *)tableName
{
    _statement.update(tableName.UTF8String);
    return self;
}

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _properties = properties;
    for (const WCTProperty &property : properties) {
        _statement.set(property, WCDB::BindParameter::default_);
    }
    return self;
}

- (instancetype)onProperty:(const WCTProperty &)property
{
    _properties = property;
    _statement.set(property, WCDB::BindParameter::default_);
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
    if (!_handle->prepare(_statement)) {
        return NO;
    }
    const WCTPropertyList &properties = _properties.empty() ? [object.class objectRelationalMappingForWCDB]->getAllProperties() : _properties;
    int index = 1;
    for (const WCTProperty &property : properties) {
        [self bindProperty:property
                  ofObject:object
                   toIndex:index];
        ++index;
    }
    bool result = _handle->step();
    _handle->finalize();
    return result;
}

- (BOOL)executeWithValue:(WCTValue *)value
{
    if (value == nil) {
        return YES;
    }
    if (!_handle->prepare(_statement)) {
        return NO;
    }
    [self bindValue:value toIndex:1];
    bool result = _handle->step();
    _handle->finalize();
    return result;
}

- (BOOL)executeWithRow:(WCTOneRow *)row
{
    if (row.count == 0) {
        return YES;
    }
    if (!_handle->prepare(_statement)) {
        return NO;
    }
    int index = 1;
    for (WCTValue *value in row) {
        [self bindValue:value toIndex:index];
        ++index;
    }
    bool result = _handle->step();
    _handle->finalize();
    return result;
}

@end
