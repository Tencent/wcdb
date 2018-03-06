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

@implementation WCTMultiSelect {
    WCTPropertyList _properties;
}

- (instancetype)fromTables:(NSArray<NSString *> *)tableNames
{
    std::list<WCDB::TableOrSubquery> tables;
    for (NSString *tableName in tableNames) {
        tables.push_back(tableName.UTF8String);
    }
    _statement.from(tables);
    return self;
}

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _properties = properties;
    _statement.select(_properties);
    return self;
}

- (instancetype)ofClasses:(NSArray<Class<WCTTableCoding>> *)classes
{
    _properties.clear();
    for (Class<WCTTableCoding> cls in classes) {
        for (const WCTProperty &property : [cls objectRelationalMappingForWCDB]->getAllProperties()) {
            _properties.push_back(property);
        }
    }
    _statement.select(_properties);
    return self;
}

- (instancetype)distinct
{
    _statement.distinct();
    return self;
}

- (WCTMultiObject *)nextMultiObject
{
    if (![self lazyPrepare]) {
        return nil;
    }
    bool done;
    if (!_handle->step(done) || done) {
        _handle->finalize();
        return nil;
    }
    NSMutableDictionary<NSString *, WCTObject *> *multiObject = [[NSMutableDictionary<NSString *, WCTObject *> alloc] init]; // table name -> object
    int index = 0;
    for (const WCTProperty &property : _properties) {
        const char *tableName = _handle->getColumnTableName(index);
        NSString *tableNameNS = tableName ? @(tableName) : @"";
        WCTObject *object = [multiObject objectForKey:tableNameNS];
        if (!object) {
            object = [[property.getColumnBinding()->getClass() alloc] init];
            [multiObject setObject:object forKey:tableNameNS];
        }
        [self extractValueAtIndex:index
                       toProperty:property
                         ofObject:object];
        ++index;
    }
    return multiObject;
}

- (NSArray<WCTMultiObject *> *)allMultiObjects
{
    if (![self lazyPrepare]) {
        return nil;
    }
    bool done;
    if (!_handle->step(done) || done) {
        _handle->finalize();
        return nil;
    }
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

    std::vector<MultiInfo> infos;
    int index = 0;

    for (const WCTProperty &property : _properties) {
        const char *tableName = _handle->getColumnTableName(index);
        Class cls = property.getColumnBinding()->getClass();
        infos.push_back(MultiInfo(property, @(tableName), cls));
        ++index;
    }

    NSMutableArray *multiObjects = [[NSMutableArray alloc] init];
    do {
        NSMutableDictionary<NSString *, WCTObject *> *multiObject = [[NSMutableDictionary<NSString *, WCTObject *> alloc] init]; // table name -> object
        index = 0;
        for (const MultiInfo &info : infos) {
            WCTObject *object = [multiObject objectForKey:info.tableName];
            if (!object) {
                object = [[info.cls alloc] init];
                [multiObject setObject:object
                                forKey:info.tableName];
            }
            [self extractValueAtIndex:index
                           toProperty:info.property
                             ofObject:object];
            ++index;
        }
        [multiObjects addObject:multiObject];
    } while (_handle->step(done) && !done);
    _handle->finalize();
    return done ? multiObjects : nil;
}

@end
