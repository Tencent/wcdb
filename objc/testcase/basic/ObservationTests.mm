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

#import "CoreConst.h"
#import <TestCase/TestCase.h>

@interface ObservationTests : CRUDTestCase

@end

@implementation ObservationTests

- (void)setUp
{
    [super setUp];

    [self insertPresetObjects];
}

- (void)test_corrupted_notification
{
    TestCaseResult* result = [TestCaseResult no];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
        if (![NSThread isMainThread] && database.isBlockaded) {
            [result makeYES];
        }
        return [self.database removeFiles];
    }];

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultYES(result);
}

- (void)test_feature_repeat_corrupted_notify_when_failed
{
    TestCaseResult* result = [TestCaseResult no];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
        WCDB_UNUSED(database)
        [result makeYES];
        return NO;
    }];

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultYES(result);
    [result makeNO];

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultNO(result);
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForReinvokingCorruptedEvent];
    TestCaseAssertResultYES(result);

    TestCaseAssertTrue([self.database removeFiles]);
}

- (void)test_is_corrupted
{
    TestCaseAssertFalse([self.database isCorrupted]);
    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
    TestCaseAssertFalse([self.database isCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    TestCaseAssertTrue([self.database isCorrupted]);

    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertFalse([self.database isCorrupted]);
}

@end
