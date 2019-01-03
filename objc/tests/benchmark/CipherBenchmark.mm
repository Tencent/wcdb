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

#import "BaselineBenchmark.h"

@interface CipherBenchmark : BaselineBenchmark <BenchmarkFactoryPreparation>

@end

@implementation CipherBenchmark

- (void)setUp
{
    [super setUp];
    self.destination = self.path;
    self.factory.delegate = self;
}

- (void)databaseWillStartPreparing:(WCTDatabase*)database
{
    [database setCipherKey:[@"benchmark" dataUsingEncoding:NSUTF8StringEncoding]];
}

- (void)test_write
{
    [self doTestWrite];
}

- (void)test_read
{
    [self doTestRead];
}

- (void)test_batch_write
{
    [self doTestBatchWrite];
}

@end
