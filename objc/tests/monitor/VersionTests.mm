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

@interface VersionTests : TestCase

@end

@implementation VersionTests

- (void)test
{
    TestCaseAssertTrue([WCTDatabase.version isEqualToString:@"1.1.0"]);
    TestCaseAssertTrue(WCTDatabase.commitHash.length > 0);
    TestCaseAssertTrue(WCTDatabase.buildTimestamp > 0);
    TestCaseAssertEqual(WCTDatabase.buildTime.timeIntervalSince1970, WCTDatabase.buildTimestamp);

    NSString *identifier = [NSString stringWithFormat:@"%@_%@_%lu", @"bfd56a1a2d98b0b21babd84fcf6db4ac", WCTDatabase.version, (unsigned long) WCTDatabase.buildTimestamp];
    TestCaseAssertTrue([WCTDatabase.identifier isEqualToString:identifier]);
}

@end
