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
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTMultiSelect+Private.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTSelectBase+Private.h>
#import <WCDB/handle_statement.hpp>

@implementation WCTMultiSelect {
    WCTPropertyList _propertyList;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andPropertyList:(const WCTPropertyList &)propertyList fromTables:(NSArray<NSString *> *)tableNames isDistinct:(BOOL)isDistinct
{
    if (self = [super initWithCore:core]) {
        _propertyList.insert(_propertyList.begin(), propertyList.begin(), propertyList.end());
        WCDB::SubqueryList subqueryList;
        for (NSString *tableName in tableNames) {
            subqueryList.push_back(tableName.UTF8String);
        }
        _statement.select(_propertyList, isDistinct).from(subqueryList);
    }
    return self;
}

- (WCTMultiObject *)nextMultiObject
{
    if ([self lazyPrepare] && [self next]) {
        NSMutableDictionary *multiObject = [[NSMutableDictionary alloc] init];
        int index = 0;
        Class cls = nil;
        NSString *tableName = nil;
        WCTObject *object = nil;
        for (const WCTProperty &property : _propertyList) {
            const char *columnTableName = _statementHandle->getColumnTableName(index);
            const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
            cls = columnBinding->getClass();
            if (columnTableName && cls) {
                tableName = @(columnTableName);
                object = [multiObject objectForKey:tableName];
                if (!object) {
                    object = [[cls alloc] init];
                    [multiObject setObject:object forKey:tableName];
                }
                if (![self extractPropertyToObject:object
                                           atIndex:index
                                 withColumnBinding:columnBinding]) {
                    return nil;
                };
            } else {
                if (!columnTableName) {
                    WCDB::Error::Warning("Extracting multi object with an empty table name");
                }
                if (!cls) {
                    WCDB::Error::Warning("Extracting multi object with an empty binding cls");
                }
            }
            ++index;
        }
        return multiObject;
    }
    return nil;
}

- (NSArray<WCTMultiObject *> *)allMultiObjects
{
    if ([self lazyPrepare]) {
        NSMutableArray *allMultiObjects = [[NSMutableArray alloc] init];
        NSMutableDictionary *multiObject = nil;
        int index = 0;
        while ([self next]) {
            multiObject = [[NSMutableDictionary alloc] init];
            index = 0;
            for (const WCTProperty &property : _propertyList) {
                const char *columnTableName = _statementHandle->getColumnTableName(index);
                const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
                Class cls = columnBinding->getClass();
                if (columnTableName && cls) {
                    NSString *tableName = @(columnTableName);
                    WCTObject *object = [multiObject objectForKey:tableName];
                    if (!object) {
                        object = [[cls alloc] init];
                        [multiObject setObject:object forKey:tableName];
                    }
                    if (![self extractPropertyToObject:object
                                               atIndex:index
                                     withColumnBinding:columnBinding]) {
                        return nil;
                    };
                } else {
                    if (!columnTableName) {
                        WCDB::Error::Warning("Extracting multi object with an empty table name");
                    }
                    if (!cls) {
                        WCDB::Error::Warning("Extracting multi object with an empty binding cls");
                    }
                }
                ++index;
            }
            [allMultiObjects addObject:multiObject];
        }
        return _error.isOK() ? allMultiObjects : nil;
    }
    return nil;
}

@end
