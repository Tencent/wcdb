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
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTUnsafeHandle (ChainCall)

- (WCTInsert *)prepareInsert
{
    if (![self isSafe]) {
        return [[WCTInsert alloc] initWithCore:self
                                     andHandle:_handle];
    }
    if ([self getOrGenerateHandle]) {
        return [[WCTInsert alloc] initWithCore:self
                           andRecyclableHandle:_recyclableHandle];
    }
    return nil;
}

- (WCTDelete *)prepareDelete
{
    if (![self isSafe]) {
        return [[WCTDelete alloc] initWithCore:self
                                     andHandle:_handle];
    }
    if ([self getOrGenerateHandle]) {
        return [[WCTDelete alloc] initWithCore:self
                           andRecyclableHandle:_recyclableHandle];
    }
    return nil;
}

- (WCTUpdate *)prepareUpdate
{
    if (![self isSafe]) {
        return [[WCTUpdate alloc] initWithCore:self
                                     andHandle:_handle];
    }
    if ([self getOrGenerateHandle]) {
        return [[WCTUpdate alloc] initWithCore:self
                           andRecyclableHandle:_recyclableHandle];
    }
    return nil;
}

- (WCTSelect *)prepareSelect
{
    if (![self isSafe]) {
        return [[WCTSelect alloc] initWithCore:self
                                     andHandle:_handle];
    }
    if ([self getOrGenerateHandle]) {
        return [[WCTSelect alloc] initWithCore:self
                           andRecyclableHandle:_recyclableHandle];
    }
    return nil;
}

- (WCTRowSelect *)prepareRowSelect
{
    if (![self isSafe]) {
        return [[WCTRowSelect alloc] initWithCore:self
                                        andHandle:_handle];
    }
    if ([self getOrGenerateHandle]) {
        return [[WCTRowSelect alloc] initWithCore:self
                              andRecyclableHandle:_recyclableHandle];
    }
    return nil;
}

- (WCTMultiSelect *)prepareMultiSelect
{
    if (![self isSafe]) {
        return [[WCTMultiSelect alloc] initWithCore:self
                                          andHandle:_handle];
    }
    if ([self getOrGenerateHandle]) {
        return [[WCTMultiSelect alloc] initWithCore:self
                                andRecyclableHandle:_recyclableHandle];
    }
    return nil;
}

@end
