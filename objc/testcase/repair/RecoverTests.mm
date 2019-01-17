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

#import "BackupTestCase.h"

@interface RecoverTests : BackupTestCase

@end

@implementation RecoverTests

- (void)test_recover
{
    __block TestCaseResult* result = [TestCaseResult failure];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
        if (![NSThread isMainThread] && database.isBlockaded) {
            [result succeed];
        }
        return [self.database removeFiles];
    }];

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    TestCaseAssertTrue([self.table getObjects] == nil);

    [NSThread sleepForTimeInterval:1.0];

    TestCaseAssertResultSuccessful(result);
}

- (void)test_avoid_frequency
{
    __block TestCaseResult* result = [TestCaseResult failure];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
        [result succeed];
        return true;
    }];

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultSuccessful(result);

    [result fail];
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultFailed(result);

    [result fail];
    [NSThread sleepForTimeInterval:5.0];
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultSuccessful(result);

    TestCaseAssertTrue([self.database removeFiles]);
}

@end
