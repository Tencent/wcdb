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
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTInsert {
    WCTProperties _properties;
    WCDB::StatementInsert _statement;
    BOOL _replace;
}

- (instancetype)init
{
    if (self = [super init]) {
        _replace = NO;
    }
    return self;
}

- (instancetype)orReplace
{
    _statement.orReplace();
    _replace = true;
    return self;
}

- (instancetype)intoTable:(NSString *)tableName
{
    _statement.insertIntoTable(tableName);
    if (_replace) {
        _statement.orReplace();
    }
    return self;
}

- (instancetype)onProperties:(const WCTProperties &)properties
{
    _properties = properties;
    _statement.columns(properties)
    .values(WCDB::BindParameter((int) properties.size()));
    return self;
}

- (BOOL)executeWithObject:(WCTObject *)object
{
    if (object == nil) {
        return YES;
    }

    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }

    const WCTProperties &properties = _properties.empty() ? [object.class allProperties] : _properties;

    WCDB::Expressions e = WCDB::BindParameters();

    if (_statement.syntax.columns.empty()) {
        _statement
        .columns(properties)
        .values(WCDB::BindParameter::bindParameters(properties.size()));
    }

    if (!handle->prepare(_statement)) {
        [self finalizeHandleIfGeneratedAndKeepError:YES];
        return NO;
    }

    std::vector<bool> autoIncrements;
    for (const WCTProperty &property : properties) {
        const WCTColumnBinding &columnBinding = property.getColumnBinding();
        bool isAutoIncrement = false;
        if (!_replace) {
            for (const auto &constraint : columnBinding.columnDef.syntax.constraints) {
                if (constraint.switcher == WCDB::ColumnConstraint::SyntaxType::Switch::PrimaryKey) {
                    isAutoIncrement = constraint.autoIncrement;
                    break;
                }
            }
        }
        autoIncrements.push_back(isAutoIncrement);
    }

    BOOL canFillLastInsertedRowID = [object respondsToSelector:@selector(lastInsertedRowID)];

    int index = 1;
    for (const WCTProperty &property : properties) {
        const WCTColumnBinding &columnBinding = property.getColumnBinding();
#warning TODO
        //        bool isAutoIncrement = !_replace && columnBinding.columnDef.isPrimary() && columnBinding.columnDef.isAutoIncrement();
        //
        //        if (!isAutoIncrement || !object.isAutoIncrement) {
        //            [self bindProperty:property
        //                      ofObject:object
        //                       toIndex:index];
        //        } else {
        //            handle->bindNull(index);
        //        }
        ++index;
    }
    bool result = handle->step();
    if (result) {
        if (!_replace && canFillLastInsertedRowID && object.isAutoIncrement) {
            object.lastInsertedRowID = handle->getLastInsertedRowID();
        }
    }
    [self doAutoFinalize:!result];
    return result;
}

- (BOOL)executeWithObjects:(NSArray<WCTObject *> *)objects
{
    if (objects.count == 0) {
        return YES;
    }

    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }

    BOOL committed = handle->runNestedTransaction([self, objects](WCDB::Handle *handle) -> bool {
        const WCTProperties &properties = _properties.empty() ? [objects[0].class allProperties] : _properties;

        if (_statement.syntax.columns.empty()) {
            _statement
            .columns(properties)
            .values(WCDB::BindParameter::bindParameters(properties.size()));
        }

        if (!handle->prepare(_statement)) {
            return false;
        }

        bool commit = true;

        std::vector<bool> autoIncrements;
        for (const WCTProperty &property : properties) {
            const WCTColumnBinding &columnBinding = property.getColumnBinding();
#warning TODO
            //            autoIncrements.push_back(!_replace && columnBinding.columnDef.isPrimary() && columnBinding.columnDef.isAutoIncrement());
        }

        BOOL canFillLastInsertedRowID = [objects[0] respondsToSelector:@selector(lastInsertedRowID)];

        int index;
        for (WCTObject *object in objects) {
            index = 1;
            for (const WCTProperty &property : properties) {
                if (!autoIncrements[index - 1] || !object.isAutoIncrement) {
                    [self bindProperty:property
                              ofObject:object
                               toIndex:index];
                } else {
                    handle->bindNull(index);
                }
                ++index;
            }
            if (!handle->step()) {
                commit = false;
                break;
            }
            if (!_replace && canFillLastInsertedRowID && object.isAutoIncrement) {
                object.lastInsertedRowID = handle->getLastInsertedRowID();
            }
            handle->reset();
        }
        handle->finalize();
        return commit;
    });

    [self doAutoFinalize:!committed];
    return committed;
}

- (WCDB::StatementInsert &)getStatement
{
    return _statement;
}

@end
