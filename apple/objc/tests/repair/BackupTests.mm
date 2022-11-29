//
// Created by sanhuazhang on 2019/05/02
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

#import "BackupTestCase.h"
#import "CoreConst.h"

@interface BackupTests : BackupTestCase

@end

@implementation BackupTests

- (void)test_backup
{
    [self
    excuteTest:^{
        TestCaseAssertTrue([self.database backup]);
    }];
}

- (void)test_dual_backup
{
    [self
    excuteTest:^{
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

        TestCaseAssertTrue([self.database backup]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

        [NSThread sleepForTimeInterval:1];
        TestCaseAssertTrue([self.database backup]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

        {
            // old one will be replaced
            NSDate *firstBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.database.firstMaterialPath error:nil][NSFileModificationDate];
            TestCaseAssertTrue(firstBackupModifiedDate != nil);
            NSDate *lastBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.database.lastMaterialPath error:nil][NSFileModificationDate];
            TestCaseAssertTrue(lastBackupModifiedDate != nil);
            TestCaseAssertTrue([firstBackupModifiedDate compare:lastBackupModifiedDate] == NSOrderedAscending);
        }

        [NSThread sleepForTimeInterval:1];
        TestCaseAssertTrue([self.database backup]);

        {
            // old one will be replaced
            NSDate *firstBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.database.firstMaterialPath error:nil][NSFileModificationDate];
            TestCaseAssertTrue(firstBackupModifiedDate != nil);
            NSDate *lastBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.database.lastMaterialPath error:nil][NSFileModificationDate];
            TestCaseAssertTrue(lastBackupModifiedDate != nil);
            TestCaseAssertTrue([firstBackupModifiedDate compare:lastBackupModifiedDate] == NSOrderedDescending);
        }
    }];
}

- (void)test_empty_backup
{
    [self
    excuteTest:^{
        TestCaseAssertTrue([self.database removeFiles]);
        TestCaseAssertTrue([self.database backup]);
    }];
}

- (void)test_wal
{
    [self
    excuteTest:^{
        TestCaseAssertTrue([[self.fileManager attributesOfItemAtPath:self.database.walPath error:nil] fileSize] > 0);
        TestCaseAssertTrue([self.database backup]);
    }];
}

- (void)test_empty_wal
{
    [self
    excuteTest:^{
        NSFileManager *fileManager = [NSFileManager defaultManager];
        TestCaseAssertTrue([[fileManager attributesOfItemAtPath:self.database.walPath error:nil] fileSize] > 0);
        TestCaseAssertTrue([self.database truncateCheckpoint]);
        TestCaseAssertTrue([[fileManager attributesOfItemAtPath:self.database.walPath error:nil] fileSize] == 0);
        TestCaseAssertTrue([self.database backup]);
    }];
}

- (void)test_filter
{
    [self
    excuteTest:^{
        [self.database filterBackup:nil];
        TestCaseAssertTrue([self.database backup]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

        [self.database filterBackup:^BOOL(NSString *tableName) {
            WCDB_UNUSED(tableName)
            return NO;
        }];
        [NSThread sleepForTimeInterval:1];
        TestCaseAssertTrue([self.database backup]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

        NSInteger firstMaterialSize = (NSInteger) [self.fileManager getFileSizeIfExists:self.database.firstMaterialPath];
        NSInteger lastMaterialSize = (NSInteger) [self.fileManager getFileSizeIfExists:self.database.lastMaterialPath];
        TestCaseAssertTrue(firstMaterialSize > lastMaterialSize);
    }];
}

- (void)test_backup_fail
{
    [self
    excuteTest:^{
        NSFileManager *fileManager = [NSFileManager defaultManager];
        TestCaseAssertTrue([fileManager createDirectoryAtPath:self.database.firstMaterialPath withIntermediateDirectories:YES attributes:nil error:nil]);
        TestCaseAssertTrue([fileManager createDirectoryAtPath:self.database.lastMaterialPath withIntermediateDirectories:YES attributes:nil error:nil]);

        TestCaseAssertFalse([self.database backup]);
    }];
}

- (BOOL)checkAutoBackedup
{
    TestCaseObject *object = [Random.shared autoIncrementTestCaseObject];
    TestCaseAssertTrue([self.table insertObject:object]);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertTrue([self.database passiveCheckpoint]);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForBackup + self.delayForTolerance];
    return [self.fileManager fileExistsAtPath:self.database.firstMaterialPath];
}

- (void)test_auto_backup_when_checkpointed
{
    [self
    excuteTest:^{
        [self.database enableAutoCheckpoint:NO];
        [self.database enableAutoBackup:YES];
        TestCaseAssertTrue([self checkAutoBackedup]);
    }];
}

- (int)getInteriorTablePage
{
    WCDB::Column pagetype("pagetype");
    WCDB::Column pageno("pageno");
    WCTValue *value = [self.database getValueOnResultColumn:pageno fromTable:@"dbStat" where:pagetype == "internal"];
    return value.numberValue.intValue;
}

@end
