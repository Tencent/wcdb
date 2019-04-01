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
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>
#endif

@interface ObservationTests : CRUDTestCase

@end

@implementation ObservationTests

- (void)test_corrupted_notification
{
    [self insertPresetObjects];

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
    [self insertPresetObjects];

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
    [self insertPresetObjects];

    TestCaseAssertFalse([self.database isCorrupted]);
    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
    TestCaseAssertFalse([self.database isCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    TestCaseAssertTrue([self.database isCorrupted]);

    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertFalse([self.database isCorrupted]);
}

- (void)test_purge
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [self.database purge];
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)test_feature_purge_will_not_clear_active_handle
{
    // acquire handle and keep it
    WCTHandle* handle = [self.database getHandle];
    TestCaseAssertTrue([handle execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [self.database purge];
    TestCaseAssertTrue([self.database isOpened]);
}

- (void)test_purge_all
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [WCTDatabase purgeAll];
    TestCaseAssertFalse([self.database isOpened]);
}

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
- (void)test_feature_auto_purge_when_memory_warning
{
    // acquire handle
    TestCaseAssertTrue([self.database canOpen]);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    // purge will be done after a few moment.
    [NSThread sleepForTimeInterval:2.0];
    TestCaseAssertFalse([self.database isOpened]);

    TestCaseAssertTrue([self.database canOpen]);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    // purge will not be done too frequent.
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurging / 2];
    TestCaseAssertTrue([self.database isOpened]);

    TestCaseAssertTrue([self.database canOpen]);
    // purge can be done after a few rest.
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurging / 2];
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    // purge will be done after a few moment.
    [NSThread sleepForTimeInterval:2.0];
    TestCaseAssertFalse([self.database isOpened]);
}
#endif

@end
