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
#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTSelect {
    WCDB::ResultColumns _resultColumns;
    Class<WCTTableCoding> _class;
}

- (instancetype)fromTable:(NSString *)tableName
{
    _statement.from(tableName);
    return self;
}

- (instancetype)onResultColumns:(const WCDB::ResultColumns &)resultColumns
{
    _resultColumns = resultColumns;
#warning TODO
    //    _class = resultColumns.front().getColumnBinding().getClass();
    _statement.select(resultColumns);
    return self;
}

- (instancetype)ofClass:(Class<WCTTableCoding>)cls
{
    _class = cls;
#warning TODO
    //    _resultColumns = [cls allProperties];
    _statement.select(_resultColumns);
    return self;
}

- (BOOL)lazyPrepare
{
    WCTRemedialAssert(_class != nil || !_resultColumns.empty(), "Class or result columns is not specificed.", return NO;);
    return [super lazyPrepare];
}

- (NSArray /* <WCTObject*> */ *)allObjects
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
#warning TODO
    NSArray *objects; // = [self allObjectsOnResultColumns:_resultColumns];
    [self doAutoFinalize:!objects];
    return objects;
}

- (id /* WCTObject* */)nextObject
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    BOOL done = NO;
    id object = [self nextObjectOnResultColumns:_resultColumns orDone:done];
    if (!object || _finalizeImmediately) {
        [self doAutoFinalize:!done];
    }
    return object;
}

@end
