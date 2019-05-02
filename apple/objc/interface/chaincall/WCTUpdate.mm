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

#import <WCDB/Assertion.hpp>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTTryDisposeGuard.h>
#import <WCDB/WCTUpdate.h>

typedef NS_ENUM(NSUInteger, WCTUpdateType) {
    WCTUpdateTypeObject,
    WCTUpdateTypeValue,
    WCTUpdateTypeRow,
};

@implementation WCTUpdate {
    WCDB::StatementUpdate _statement;
    WCTProperties _properties;

    WCTUpdateType _type;
    NSObject *_value;
}

- (WCDB::StatementUpdate &)statement
{
    return _statement;
}

- (instancetype)table:(NSString *)tableName
{
    _statement.update(tableName);
    return self;
}

- (instancetype)set:(const WCTProperties &)properties
{
    _properties = properties;
    int bindParameterIndex = 1;
    for (const WCTProperty &property : properties) {
        _statement.set(property).to(WCDB::BindParameter(bindParameterIndex));
        ++bindParameterIndex;
    }
    return self;
}

- (instancetype)where:(const WCDB::Expression &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orders:(const WCDB::OrderingTerms &)orders
{
    _statement.orders(orders);
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

- (instancetype)toObject:(WCTObject *)object
{
    _type = WCTUpdateTypeObject;
    _value = object;
    return self;
}

- (instancetype)toValue:(WCTColumnCodingValue *)value
{
    _type = WCTUpdateTypeValue;
    _value = value;
    return self;
}

- (instancetype)toRow:(WCTColumnCodingRow *)row
{
    _type = WCTUpdateTypeRow;
    _value = row;
    return self;
}

- (BOOL)execute
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    BOOL result = YES;
    if (_value != nil) {
        result = NO;
        if ([_handle prepare:_statement]) {
            switch (_type) {
            case WCTUpdateTypeValue: {
                WCTColumnCodingValue *value = (WCTColumnCodingValue *) _value;
                [_handle bindValue:value toIndex:1];
                break;
            }
            case WCTUpdateTypeObject: {
                WCTObject *object = (WCTObject *) _value;
                [_handle bindProperties:_properties ofObject:object];
                break;
            }
            case WCTUpdateTypeRow: {
                WCTColumnCodingRow *row = (WCTColumnCodingRow *) _value;
                int index = 1;
                for (WCTColumnCodingValue *value in row) {
                    [_handle bindValue:value toIndex:index];
                    ++index;
                }
                break;
            }
            }
            result = [_handle step];
            [_handle finalizeStatement];
        }
    }
    return result;
}

@end
