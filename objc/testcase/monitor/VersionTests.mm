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

#import "Version.h"
#import <TestCase/TestCase.h>

@interface VersionTests : BaseTestCase

@end

@implementation VersionTests

- (void)test
{
    TestCaseAssertTrue([WCTDatabase.version isEqualToString:@WCDB_VERSION]);
    TestCaseAssertTrue([WCTDatabase.commitHash isEqualToString:@WCDB_COMMIT_HASH]);
    TestCaseAssertEqual(WCTDatabase.buildTimestamp, WCDB_BUILD_TIMESTAMP);
    TestCaseAssertTrue([WCTDatabase.buildTime isEqualToDate:[NSDate dateWithTimeIntervalSince1970:WCDB_BUILD_TIMESTAMP]]);

    NSString *identifier = [NSString stringWithFormat:@"%@_%@_%d", @WCDB_IDENTIFIER, @WCDB_VERSION, WCDB_BUILD_TIMESTAMP];
    TestCaseAssertTrue([WCTDatabase.identifier isEqualToString:identifier]);
}

@end
