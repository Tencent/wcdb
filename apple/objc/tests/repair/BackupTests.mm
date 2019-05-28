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
    TestCaseAssertTrue([self.database backup]);
}

- (void)test_dual_backup
{
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
}

- (void)test_empty_backup
{
    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertTrue([self.database backup]);
}

- (void)test_wal
{
    TestCaseAssertTrue([[self.fileManager attributesOfItemAtPath:self.database.walPath error:nil] fileSize] > 0);

    TestCaseAssertTrue([self.database backup]);
}

- (void)test_empty_wal
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    TestCaseAssertTrue([[fileManager attributesOfItemAtPath:self.database.walPath error:nil] fileSize] > 0);
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    TestCaseAssertTrue([[fileManager attributesOfItemAtPath:self.database.walPath error:nil] fileSize] == 0);
    TestCaseAssertTrue([self.database backup]);
}

- (void)test_filter
{
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
}

- (void)test_backup_fail
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    TestCaseAssertTrue([fileManager createDirectoryAtPath:self.database.firstMaterialPath withIntermediateDirectories:YES attributes:nil error:nil]);
    TestCaseAssertTrue([fileManager createDirectoryAtPath:self.database.lastMaterialPath withIntermediateDirectories:YES attributes:nil error:nil]);

    TestCaseAssertFalse([self.database backup]);
}

- (BOOL)checkAutoBackedup
{
    TestCaseObject *object = [self.random autoIncrementTestCaseObject];
    TestCaseAssertTrue([self.table insertObject:object]);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertTrue([self.database passiveCheckpoint]);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForBackup + self.delayForTolerance];
    return [self.fileManager fileExistsAtPath:self.database.firstMaterialPath];
}

- (void)test_auto_backup_when_checkpointed
{
    [self.database removeCheckpointConfig];
    self.database.autoBackup = YES;
    TestCaseAssertTrue([self checkAutoBackedup]);
}

- (void)test_cancel_auto_backup
{
    self.database.autoBackup = YES;
    TestCaseAssertTrue([self checkAutoBackedup]);

    TestCaseAssertTrue([self.fileManager removeItemAtPath:self.database.firstMaterialPath error:nil]);
    self.database.autoBackup = NO;
    TestCaseAssertFalse([self checkAutoBackedup]);
}

- (int)getInteriorTablePage
{
    WCDB::Column pagetype("pagetype");
    WCDB::Column pageno("pageno");
    WCTValue *value = [self.database getValueOnResultColumn:pageno fromTable:@"dbStat" where:pagetype == "internal"];
    return value.numberValue.intValue;
}

- (void)test_feature_closed_database_will_not_perform_auto_backup
{
    [self.database removeCheckpointConfig];
    self.database.autoBackup = YES;

    TestCaseAssertTrue([self.database passiveCheckpoint]);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    [self.database close];

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForBackup + self.delayForTolerance];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
}

@end
