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
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTInsert {
    WCTPropertyList _properties;
    WCDB::StatementInsert _statement;
    BOOL _replace;
}

- (instancetype)orReplace
{
    _statement.getMutableLang().type = WCDB::Lang::InsertSTMT::Type::InsertOrReplace;
    _replace = true;
    return self;
}

- (instancetype)intoTable:(NSString *)tableName
{
    if (!_replace) {
        _statement.insertInto(tableName.UTF8String);
    } else {
        _statement.insertOrReplaceInto(tableName.UTF8String);
    }
    return self;
}

- (instancetype)onProperties:(const WCTPropertyList &)properties
{
    _properties = properties;
    _statement.on(properties);
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

    std::vector<bool> autoIncrements;
    for (const WCTProperty &property : properties) {
        const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
        autoIncrements.push_back(!_replace && columnBinding->columnDef.isPrimary() && columnBinding->columnDef.isAutoIncrement());
    }

    BOOL canFillLastInsertedRowID = [object respondsToSelector:@selector(lastInsertedRowID)];

    int index = 1;
    for (const WCTProperty &property : properties) {
        const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
        bool isAutoIncrement = !_replace && columnBinding->columnDef.isPrimary() && columnBinding->columnDef.isAutoIncrement();

        if (!isAutoIncrement || !object.isAutoIncrement) {
            [self bindProperty:property
                      ofObject:object
                       toIndex:index];
        } else {
            _handle->bindNull(index);
        }
        ++index;
    }
    bool result = _handle->step();
    if (result) {
        if (!_replace && canFillLastInsertedRowID && object.isAutoIncrement) {
            object.lastInsertedRowID = _handle->getLastInsertedRowID();
        }
    }
    _handle->finalize();
    return result;
}

- (BOOL)executeWithObjects:(NSArray<WCTObject *> *)objects
{
    if (objects.count == 0) {
        return YES;
    }

    bool isBegan = false;
    if (!_handle->isInTransaction()) {
        if (!_handle->execute(WCDB::StatementBegin::immediate)) {
            return NO;
        }
        isBegan = true;
    }

    bool failed = false;
    if (_handle->prepare(_statement)) {
        const WCTPropertyList &properties = _properties.empty() ? [objects[0].class objectRelationalMappingForWCDB]->getAllProperties() : _properties;

        std::vector<bool> autoIncrements;
        for (const WCTProperty &property : properties) {
            const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
            autoIncrements.push_back(!_replace && columnBinding->columnDef.isPrimary() && columnBinding->columnDef.isAutoIncrement());
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
                    _handle->bindNull(index);
                }
                ++index;
            }
            if (!_handle->step()) {
                failed = true;
                break;
            }
            if (!_replace && canFillLastInsertedRowID && object.isAutoIncrement) {
                object.lastInsertedRowID = _handle->getLastInsertedRowID();
            }
            _handle->reset();
        }
        _handle->finalize();
    }

    if (!isBegan) {
        if (!failed) {
            return YES;
        }
    } else {
        if (!failed) {
            if (_handle->execute(WCDB::StatementCommit::default_)) {
                return YES;
            }
        }
        _handle->skipError(true);
        _handle->execute(WCDB::StatementRollback::default_);
        _handle->skipError(false);
    }
    return NO;
}

@end
