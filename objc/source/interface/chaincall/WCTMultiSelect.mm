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

struct MultiInfo {
    WCTProperty property;
    NSString *tableName;
    Class cls;
    MultiInfo(const WCTProperty &property_, NSString *tableName_, Class cls_)
        : property(property_)
        , tableName(tableName_)
        , cls(cls_)
    {
    }
};
typedef struct MultiInfo MultiInfo;

@implementation WCTMultiSelect {
    WCTPropertyList _properties;
    std::list<MultiInfo> _infos;
}

- (instancetype)fromTables:(NSArray<NSString *> *)tableNames
{
    std::list<WCDB::TableOrSubquery> tables;
    for (NSString *tableName in tableNames) {
        tables.push_back(tableName.cppString);
    }
    _statement.from(tables);
    return self;
}

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _properties = properties;
    _statement.select(properties);
    return self;
}

- (void)lazyInitMultiInfo
{
    WCTInnerAssert(_handle != nullptr);
    if (_infos.empty()) {
        int index = 0;
        for (const WCTProperty &property : _properties) {
            const char *tableName = _handle->getColumnTableName(index);
            NSString *nsTableName = nil;
            if (tableName) {
                nsTableName = [NSString stringWithUTF8String:tableName];
            }
            if (!nsTableName) {
                nsTableName = @"";
            }
            Class cls = property.getColumnBinding().getClass();
            _infos.push_back(MultiInfo(property, nsTableName, cls));
            ++index;
        }
    }
}

- (WCTMultiObject *)extractMutliObject
{
    NSMutableDictionary<NSString *, WCTObject *> *multiObject = [[NSMutableDictionary<NSString *, WCTObject *> alloc] init]; // table name -> object
    int index = 0;
    for (const auto &info : _infos) {
        WCTObject *object = [multiObject objectForKey:info.tableName];
        if (!object) {
            object = [[info.cls alloc] init];
            [multiObject setObject:object forKey:info.tableName];
        }
        [self extractValueAtIndex:index
                       toProperty:info.property
                         ofObject:object];
        ++index;
    }
    return multiObject;
}

- (WCTMultiObject *)nextMultiObject
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }

    WCTInnerAssert(_handle != nullptr);
    bool done = false;
    if (!_handle->step(done) || done) {
        [self doAutoFinalize:!done];
        return nil;
    }

    [self lazyInitMultiInfo];

    WCTMultiObject *object = [self extractMutliObject];
    if (_finalizeImmediately) {
        [self doAutoFinalize:NO];
    }
    return object;
}

- (NSArray<WCTMultiObject *> *)allMultiObjects
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }

    WCTInnerAssert(_handle != nullptr);
    bool done = false;
    if (!_handle->step(done) || done) {
        [self doAutoFinalize:!done];
        return nil;
    }

    [self lazyInitMultiInfo];

    NSMutableArray<WCTMultiObject *> *multiObjects = [[NSMutableArray alloc] init];
    do {
        [multiObjects addObject:[self extractMutliObject]];
    } while (_handle->step(done) && !done);

    [self doAutoFinalize:!done];
    return done ? multiObjects : nil;
}

@end
