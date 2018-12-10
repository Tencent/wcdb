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

//#warning TODO - isCorrupted is hard to get the real corruption state
//- (void)test_recover_skip
//{
//    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
//
//    TestCaseAssertTrue([self.table getObjects] == nil);
//
//    [NSThread sleepForTimeInterval:1.0];
//    TestCaseAssertTrue([self.database isCorrupted]);
//}
//
//- (void)test_recover_custom
//{
//    self.database.recoveryMode = WCTRecoveryModeCustom;
//
//    __block BOOL tested = NO;
//    [self.database setNotificationWhenRecovering:^BOOL(WCTDatabase* database) {
//        tested = !database.isOpened && ![NSThread isMainThread];
//        return [self.database removeFiles];
//    }];
//
//    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
//
//    TestCaseAssertTrue([self.table getObjects] == nil);
//
//    [NSThread sleepForTimeInterval:1.0];
//    TestCaseAssertFalse([self.database isCorrupted]);
//
//    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.path]);
//    TestCaseAssertTrue(tested);
//}
//
//- (void)test_recover_remove
//{
//    self.database.recoveryMode = WCTRecoveryModeRemove;
//
//    __block BOOL tested = NO;
//    [self.database setNotificationWhenRecovering:^BOOL(WCTDatabase* database) {
//        tested = !database.isOpened && ![NSThread isMainThread];
//        return true;
//    }];
//
//    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
//
//    TestCaseAssertTrue([self.table getObjects] == nil);
//
//    [NSThread sleepForTimeInterval:1.0];
//    TestCaseAssertFalse([self.database isCorrupted]);
//
//    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.path]);
//    TestCaseAssertTrue(tested);
//}
//
//- (void)test_recover_deposited
//{
//    self.database.recoveryMode = WCTRecoveryModeDeposit;
//
//    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
//
//    TestCaseAssertTrue([self.table getObjects] == nil);
//
//    [NSThread sleepForTimeInterval:1.0];
//    TestCaseAssertFalse([self.database isCorrupted]);
//    TestCaseAssertTrue([self.database containsDeposited]);
//}

@end
