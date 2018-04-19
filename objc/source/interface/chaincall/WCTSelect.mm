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

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _properties = properties;
    return self;
}

- (instancetype)ofClass:(Class<WCTTableCoding>)cls
{
    _class = cls;
    return self;
}

- (BOOL)lazyPrepare
{
    WCTAssert(_class != nil || !_properties.empty(), "Class or properties is not specificed.");
    if (_statement.isResultColumnsNotSet()) {
        _statement.select(!_properties.empty() ? _properties : [_class objectRelationalMappingForWCDB]->getAllProperties());
    }
    if (!_class) {
        _class = _properties.front().getColumnBinding()->getClass();
    }
    return [super lazyPrepare];
}

- (NSArray /* <WCTObject*> */ *)allObjects
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    NSArray *objects;
    if (!_properties.empty()) {
        objects = [self allObjectsOnProperties:_properties];
    } else {
        objects = [self allObjectsOfClass:_class];
    }
    [self doAutoFinalize:!objects];
    return objects;
}

- (id /* WCTObject* */)nextObject
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    BOOL done;
    id object;
    if (!_properties.empty()) {
        object = [self nextObjectOnProperties:_properties orDone:done];
    } else {
        object = [self nextObjectOfClass:_class orDone:done];
    }
    if (!object || _finalizeImmediately) {
        [self doAutoFinalize:!done];
    }
    return object;
}

@end
