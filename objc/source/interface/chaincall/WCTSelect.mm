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

#import <WCDB/Assertion.hpp>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTTryDisposeGuard.h>

@implementation WCTSelect {
    WCTResultColumns _resultColumns;
}

- (instancetype)fromTable:(NSString *)tableName
{
    _statement.from(tableName);
    return self;
}

- (instancetype)onResultColumns:(const WCTResultColumns &)resultColumns
{
    _resultColumns = resultColumns;
    _statement.select(_resultColumns);
    return self;
}

- (instancetype)ofClass:(Class)cls
{
    return [self onResultColumns:[cls allProperties]];
}

- (NSArray /* <WCTObject*> */ *)allObjects
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    if (![self lazyPrepare]) {
        return nil;
    }
    NSArray *objects = [_handle allObjectsOnResultColumns:_resultColumns];
    [_handle finalizeStatement];
    return objects;
}

- (id /* WCTObject* */)nextObject
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    if (![self lazyPrepare]) {
        return nil;
    }
    BOOL done = NO;
    if (![_handle step:done] || done) {
        [_handle finalizeStatement];
        return nil;
    }
    return [_handle extractObjectOnResultColumns:_resultColumns];
}

@end
