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
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTSelect {
    WCTPropertyList _properties;
    Class<WCTTableCoding> _class;
}

- (instancetype)fromTable:(NSString *)tableName
{
    _statement.from(tableName.UTF8String);
    return self;
}

- (instancetype)onProperty:(const WCTProperty &)property
{
    _statement.select(property);
    return self;
}

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _statement.select(properties);
    return self;
}

- (instancetype)ofClass:(Class<WCTTableCoding>)cls
{
    _class = cls;
    _statement.select([cls objectRelationalMappingForWCDB]->getAllProperties());
    return self;
}

- (Class<WCTTableCoding>)getClass
{
    if (!_class) {
        _class = _properties.front().getColumnBinding()->getClass();
    }
    return _class;
}

- (const WCTPropertyList &)getProperties
{
    assert((_class == nil) ^ _properties.empty());
    return _class != nil ? [_class objectRelationalMappingForWCDB]->getAllProperties() : _properties;
}

- (NSArray /* <WCTObject*> */ *)allObjects
{
    if (![self lazyPrepare]) {
        return nil;
    }

    NSMutableArray *objects = [[NSMutableArray alloc] init];
    WCTObject *object = nil;
    int index = 0;

    Class cls = [self getClass];
    const WCTPropertyList &properties = [self getProperties];

    bool done = false;
    while (_handle->step(done) && !done) {
        object = [[cls alloc] init];
        index = 0;
        for (const WCTProperty &property : properties) {
            [self extractValueAtIndex:index
                           toProperty:property
                             ofObject:object];
            ++index;
        }
        [objects addObject:object];
    }
    _handle->finalize();
    return done ? objects : nil;
}

- (id /* WCTObject* */)nextObject
{
    if (![self lazyPrepare]) {
        return nil;
    }

    bool done = false;
    WCTObject *object = nil;
    if (_handle->step(done) && !done) {
        Class cls = [self getClass];
        object = [[cls alloc] init];
        const WCTPropertyList &properties = [self getProperties];

        int index = 0;
        for (const WCTProperty &property : properties) {
            [self extractValueAtIndex:index
                           toProperty:property
                             ofObject:object];
            ++index;
        }
    }
    _handle->finalize();
    return done ? nil : object;
}

@end
