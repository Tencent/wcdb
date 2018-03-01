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

#import <WCDB/Utility.hpp>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTCommon.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTProperty.h>

@implementation WCTInsert {
    WCTPropertyList _properties;
    WCDB::StatementInsert _statement;
    BOOL _replace;
}

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
                   andProperties:(const WCTPropertyList &)properties
                    andTableName:(NSString *)tableName
                  andReplaceFlag:(BOOL)replace
{
    if (self = [super initWithDatabase:database]) {
        _replace = replace;
        _properties = properties;
        if (!replace) {
            _statement.insertInto(tableName.UTF8String);
        } else {
            _statement.insertOrReplaceInto(tableName.UTF8String);
        }
        _statement.on(_properties).values(std::list<WCDB::Expression>(_properties.size(), WCDB::BindParameter::default_));
    }
    return self;
}

- (BOOL)doInsertObjects:(NSArray<WCTObject *> *)objects withError:(WCDB::Error &)error
{
    WCDB::RecyclableStatement handleStatement = _database->prepare(_statement, error);
    if (!handleStatement) {
        return NO;
    }
    int index;
    BOOL canFillLastInsertedRowID = [objects[0] respondsToSelector:@selector(lastInsertedRowID)];

    std::vector<bool> autoIncrements;
    for (const WCTProperty &property : _properties) {
        const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
        autoIncrements.push_back(!_replace && columnBinding->columnDef.isPrimary() && columnBinding->columnDef.isAutoIncrement());
    }

    for (WCTObject *object in objects) {
        index = 1;
        for (const WCTProperty &property : _properties) {
            if (autoIncrements[index - 1] && object.isAutoIncrement) {
                handleStatement->bind<WCTColumnTypeNull>(index);
            } else {
                if (![self bindProperty:property
                                 ofObject:object
                        toStatementHandle:handleStatement
                                  atIndex:index
                                withError:error]) {
                    return NO;
                }
            }
            ++index;
        }
        handleStatement->step();
        if (!handleStatement->isOK()) {
            error = handleStatement->getError();
            return NO;
        }
        if (!_replace && canFillLastInsertedRowID && object.isAutoIncrement) {
            object.lastInsertedRowID = handleStatement->getLastInsertedRowID();
        }
        handleStatement->reset();
        if (!handleStatement->isOK()) {
            error = handleStatement->getError();
            return NO;
        }
    }
    return error.isOK();
}

- (BOOL)executeWithObjects:(NSArray<WCTObject *> *)objects
{
    if (!_error.isOK()) {
        return NO;
    }
    if (objects.count == 0) {
        WCDB::Error::Warning("Inserting with an empty/nil object");
        return NO;
    }
    if (objects.count == 1) {
        return [self doInsertObjects:objects withError:_error];
    }
    _database->runEmbeddedTransaction([self, objects](WCDB::Error &error) {
        [self doInsertObjects:objects withError:error];
    },
                                      _error);
    return _error.isOK();
}

@end
