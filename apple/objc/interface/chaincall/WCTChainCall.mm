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
#import <WCDB/WCTChainCall.h>
#import <WCDB/WCTHandle.h>

@implementation WCTChainCall

- (instancetype)initWithHandle:(WCTHandle*)handle
{
    WCTInnerAssert(handle != nil);
    if (self = [super init]) {
        _disposeAction = WCTChainCallDisposeNoAction;
        _handle = handle;
    }
    return self;
}

- (WCTHandle*)handle
{
    return _handle;
}

- (WCTError*)error
{
    return _handle.error;
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

- (void)tryDispose
{
    switch (_disposeAction) {
    case WCTChainCallDisposeNoAction:
        break;
    case WCTChainCallDisposeActionFinalizeStatement:
        [_handle finalizeStatement];
        break;
    case WCTChainCallDisposeActionInvalidate:
        [_handle invalidate];
        break;
    }
}

@end
