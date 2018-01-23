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

#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/handle_statement.hpp>
#import <WCDB/utility.hpp>

@implementation WCTInsert {
    WCTPropertyList _propertyList;
    WCDB::StatementInsert _statement;
    BOOL _replace;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andProperties:(const WCTPropertyList &)propertyList andTableName:(NSString *)tableName andReplaceFlag:(BOOL)replace
{
    if (self = [super initWithCore:core]) {
        _replace = replace;
        _propertyList.insert(_propertyList.begin(), propertyList.begin(), propertyList.end());
        _statement = WCDB::StatementInsert()
                         .insert(tableName.UTF8String,
                                 _propertyList,
                                 _replace ? WCDB::Conflict::Replace : WCDB::Conflict::NotSet)
                         .values(WCDB::ExpressionList(_propertyList.size(), WCDB::Expression::BindParameter));
    }
    return self;
}

- (BOOL)doInsertObjects:(NSArray<WCTObject *> *)objects withError:(WCDB::Error &)error
{
    WCDB::RecyclableStatement statementHandle = _core->prepare(_statement, error);
    if (!statementHandle) {
        return NO;
    }
    int index;
    BOOL canFillLastInsertedRowID = [objects[0] respondsToSelector:@selector(lastInsertedRowID)];
    for (WCTObject *object in objects) {
        index = 1;
        for (const WCTProperty &property : _propertyList) {
            const std::shared_ptr<WCTColumnBinding> &columnBinding = property.getColumnBinding();
            if (!_replace && columnBinding->isPrimary() && columnBinding->isAutoIncrement() && object.isAutoIncrement) {
                statementHandle->bind<(WCDB::ColumnType) WCTColumnTypeNull>(index);
            } else {
                if (![self bindProperty:property
                                 ofObject:object
                        toStatementHandle:statementHandle
                                  atIndex:index
                                withError:error]) {
                    return NO;
                }
            }
            ++index;
        }
        statementHandle->step();
        if (!statementHandle->isOK()) {
            error = statementHandle->getError();
            return NO;
        }
        if (!_replace && canFillLastInsertedRowID && object.isAutoIncrement) {
            object.lastInsertedRowID = statementHandle->getLastInsertedRowID();
        }
        statementHandle->reset();
        if (!statementHandle->isOK()) {
            error = statementHandle->getError();
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
    _core->runEmbeddedTransaction([self, objects](WCDB::Error &error) {
        [self doInsertObjects:objects withError:error];
    },
                                  _error);
    return _error.isOK();
}

@end
