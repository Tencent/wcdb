//
// Created by sanhuazhang on 2019/05/02
//

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

#import "WCTMultiSelect.h"
#import "Assertion.hpp"
#import "WCTChainCall+Private.h"
#import "WCTHandle+Private.h"
#import "WCTHandle.h"
#import "WCTORM.h"
#import "WCTSelectable+Private.h"
#import "WCTTryDisposeGuard.h"

@implementation WCTMultiSelect {
    WCTResultColumns _resultColumns;
}

- (instancetype)fromTables:(NSArray<NSString *> *)tableNames
{
    WCTRemedialAssert(tableNames != nil, @"Table names can't be null.", return self;);
    WCDB::TablesOrSubqueries tables;
    for (NSString *tableName in tableNames) {
        tables.push_back(tableName);
    }
    _statement.from(tables);
    return self;
}

- (instancetype)onResultColumns:(const WCTResultColumns &)resultColumns
{
    _resultColumns = resultColumns;
    _statement.select(resultColumns);
    return self;
}

- (WCTMultiObject *)firstMultiObject
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    if (![self lazyPrepare]) {
        [self saveChangesAndError:false];
        return nil;
    }
    BOOL succeed = false;
    if (!(succeed = [_handle step]) || [_handle done]) {
        [self saveChangesAndError:succeed];
        return nil;
    }
    [self saveChangesAndError:succeed];
    return [_handle extractMultiObjectOnResultColumns:_resultColumns];
}

- (NSArray<WCTMultiObject *> *)allMultiObjects
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    if (![self lazyPrepare]) {
        [self saveChangesAndError:false];
        return nil;
    }
    NSArray<WCTMultiObject *> *results = [_handle allMultiObjectsOnResultColumns:_resultColumns];
    [self saveChangesAndError:results != nil];
    return results;
}

@end
