//
// Created by qiuwenchen on 2023/12/20.
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

#import "TestCase.h"
#import <Foundation/Foundation.h>

@interface VacuumRobustyTests : RobustyTestCase

@end

@implementation VacuumRobustyTests

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
    [self.database enableAutoCheckpoint:YES];
}

- (void)test_vacuum_robusty
{
    NSArray* sizes = @[
        @(10 * 1024 * 1024),
        @(100 * 1024 * 1024),
#ifndef WCDB_QUICK_TESTS
        @(1024 * 1024 * 1024),
#endif
    ];
    for (NSNumber* size in sizes) {
        self.expectedDatabaseSize = size.integerValue;
        [self.database removeFiles];
        TestCaseAssertTrue([self fillDatabaseUntilMeetExpectedSize]);

        NSDictionary<NSString*, NSArray<TestCaseObject*>*>* expectedTableObjects = [self getTableObjects];
        TestCaseAssertTrue(expectedTableObjects != nil);

        TestCaseAssertTrue([self.database vacuum:nil]);

        NSDictionary<NSString*, NSArray<TestCaseObject*>*>* currentTableObjects = [self getTableObjects];

        TestCaseAssertTrue([currentTableObjects isEqualToDictionary:expectedTableObjects]);
    }
}

@end
