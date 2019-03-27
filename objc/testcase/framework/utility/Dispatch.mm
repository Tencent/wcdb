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

#import <TestCase/Dispatch.h>

@implementation Dispatch {
    dispatch_queue_t _queue;
    dispatch_group_t _group;
}

- (instancetype)init
{
    if (self = [super init]) {
        _queue = dispatch_queue_create("com.Tencent.TestCase", DISPATCH_QUEUE_CONCURRENT);
        _group = dispatch_group_create();
    }
    return self;
}

- (void)async:(DispatchBlock)block
{
    dispatch_group_async(_group, _queue, block);
}

- (void)waitUntilDone
{
    dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
}

@end
