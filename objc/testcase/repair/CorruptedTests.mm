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
#import "CoreConst.h"

@interface CorruptedTests : BackupTestCase

@end

@implementation CorruptedTests

- (void)test_notification
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

- (void)test_feature_repeat_notify_when_failed
{
    TestCaseResult* result = [TestCaseResult no];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
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
    [NSThread sleepForTimeInterval:WCDB::CorruptionQueueTimeIntervalForInvokingEvent];
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

- (void)test_compatible_backup_will_not_trigger_corruption
{
#warning TODO - ignore corruption from RepairKit since there is some known issues during backup.

    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);

    TestCaseAssertFalse([self.database isCorrupted]);

    TestCaseResult* corrupted = [TestCaseResult no];
    __weak typeof(self) weakSelf = self;
    [WCTDatabase globalTraceSQL:^(NSString* sql) {
        typeof(self) strongSelf = weakSelf;
        if (strongSelf == nil) {
            return;
        }

        if ([corrupted isNO] && [sql isEqualToString:@"ROLLBACK"]) {
            // Backup Write Handle Rollback, which means that it's already inited.
            NSFileHandle* fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:strongSelf.path];
            if (fileHandle == nil) {
                TestCaseFailure();
                return;
            }
            unsigned long long fileSize = [fileHandle seekToEndOfFile];
            [fileHandle seekToFileOffset:0];
            [fileHandle writeData:[self.random dataWithLength:(NSInteger) fileSize]];
            [fileHandle closeFile];
            [corrupted makeYES];
        }
    }];

    TestCaseResult* tested = [TestCaseResult no];
    [WCTDatabase globalTraceError:^(WCTError* error) {
        NSLog(@"%@", error);
        typeof(self) strongSelf = weakSelf;
        if (strongSelf == nil) {
            return;
        }
        if (error.code == WCTErrorCodeCorrupt
            && error.level == WCTErrorLevelIgnore
            && [error.source isEqualToString:@"Repair"]
            && [error.path isEqualToString:strongSelf.database.path]
            && error.tag == strongSelf.database.tag) {
            TestCaseAssertResultYES(corrupted);
            [tested makeYES];
        }
    }];

    TestCaseAssertFalse([self.database backup]);
    TestCaseAssertResultYES(tested);
    TestCaseAssertFalse([self.database isCorrupted]);
    [WCTDatabase resetGlobalErrorTracer];
}

@end
