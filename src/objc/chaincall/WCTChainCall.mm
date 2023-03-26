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

#import "WCTChainCall.h"
#import "Assertion.hpp"
#import "WCTChainCall+Private.h"
#import "WCTHandle.h"

@implementation WCTChainCall

- (instancetype)initWithHandle:(WCTHandle*)handle
{
    WCTAssert(handle != nil);
    if (self = [super init]) {
        _disposeAction = WCTChainCallDisposeActionFinalizeStatement;
        _handle = handle;
    }
    return self;
}

- (WCTError*)error
{
    return _error;
}

- (int)getChanges
{
    return _changes;
}

- (instancetype)invalidateWhenDispose
{
    _disposeAction = WCTChainCallDisposeActionInvalidate;
    return self;
}

- (instancetype)finalizeStatementWhenDispose
{
    _disposeAction = WCTChainCallDisposeActionFinalizeStatement;
    return self;
}

- (void)saveChangesAndError:(BOOL)succeed
{
    if (succeed) {
        _changes = [_handle getChanges];
    } else {
        _error = _handle.error;
    }
}

- (void)tryDispose
{
    switch (_disposeAction) {
    case WCTChainCallDisposeActionFinalizeStatement:
        [_handle finalizeStatement];
        break;
    case WCTChainCallDisposeActionInvalidate:
        [_handle invalidate];
        break;
    }
}

@end
