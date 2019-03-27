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

@interface BackupTests : BackupTestCase

@end

@implementation BackupTests

- (void)test_backup
{
    TestCaseAssertTrue([self.database backup]);
}

- (void)test_dual_backup
{
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.lastMaterialPath]);

    TestCaseAssertTrue([self.database backup]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.lastMaterialPath]);

    [NSThread sleepForTimeInterval:1];
    TestCaseAssertTrue([self.database backup]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.lastMaterialPath]);

    {
        // old one will be replaced
        NSDate *firstBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.firstMaterialPath error:nil][NSFileModificationDate];
        TestCaseAssertTrue(firstBackupModifiedDate != nil);
        NSDate *lastBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.lastMaterialPath error:nil][NSFileModificationDate];
        TestCaseAssertTrue(lastBackupModifiedDate != nil);
        TestCaseAssertTrue([firstBackupModifiedDate compare:lastBackupModifiedDate] == NSOrderedAscending);
    }

    [NSThread sleepForTimeInterval:1];
    TestCaseAssertTrue([self.database backup]);

    {
        // old one will be replaced
        NSDate *firstBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.firstMaterialPath error:nil][NSFileModificationDate];
        TestCaseAssertTrue(firstBackupModifiedDate != nil);
        NSDate *lastBackupModifiedDate = [self.fileManager attributesOfItemAtPath:self.lastMaterialPath error:nil][NSFileModificationDate];
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
    TestCaseAssertTrue([[self.fileManager attributesOfItemAtPath:self.walPath error:nil] fileSize] > 0);

    TestCaseAssertTrue([self.database backup]);
}

- (void)test_empty_wal
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    TestCaseAssertTrue([[fileManager attributesOfItemAtPath:self.walPath error:nil] fileSize] > 0);
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")]);
    TestCaseAssertTrue([[fileManager attributesOfItemAtPath:self.walPath error:nil] fileSize] == 0);
    TestCaseAssertTrue([self.database backup]);
}

- (void)test_filter
{
    TestCaseAssertTrue([self.database backup]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.lastMaterialPath]);

    [self.database filterBackup:^BOOL(NSString *tableName) {
        WCDB_UNUSED(tableName)
        return NO;
    }];
    [NSThread sleepForTimeInterval:1];
    TestCaseAssertTrue([self.database backup]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.lastMaterialPath]);

    NSInteger firstMaterialSize = (NSInteger) [self.fileManager getFileSizeIfExists:self.firstMaterialPath];
    NSInteger lastMaterialSize = (NSInteger) [self.fileManager getFileSizeIfExists:self.lastMaterialPath];
    TestCaseAssertTrue(firstMaterialSize > lastMaterialSize);
}

- (void)test_backup_fail
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *firstBackupPath = [self.database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [self.database.path stringByAppendingString:@"-last.material"];
    TestCaseAssertTrue([fileManager createDirectoryAtPath:firstBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);
    TestCaseAssertTrue([fileManager createDirectoryAtPath:lastBackupPath withIntermediateDirectories:YES attributes:nil error:nil]);

    TestCaseAssertFalse([self.database backup]);
}

- (BOOL)checkAutoBackedup
{
    TestCaseObject *object = [self.random autoIncrementTestCaseObject];
    TestCaseAssertTrue([self.table insertObject:object]);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint())]);

    [NSThread sleepForTimeInterval:self.delayForTolerance];
    return [self.fileManager fileExistsAtPath:self.firstMaterialPath];
}

- (void)test_auto_backup_when_checkpointed
{
    [self.database removeConfigForName:WCTConfigNameCheckpoint];
    self.database.autoBackup = YES;
    TestCaseAssertTrue([self checkAutoBackedup]);
}

- (void)test_auto_backup_when_meet_non_critical_frames_interval
{
    [self.database removeConfigForName:WCTConfigNameCheckpoint];
    self.database.autoBackup = YES;

    TestCaseObject *object = [self.random autoIncrementTestCaseObject];

    while ([self getWalFrameCount] < self.backupFramesIntervalForNonCritical - self.framesForTolerance) {
        TestCaseAssertTrue([self.table insertObject:object]);
    }
    [NSThread sleepForTimeInterval:self.backupDelayForNonCritical + self.delayForTolerance];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.firstMaterialPath]);

    while ([self getWalFrameCount] < self.backupFramesIntervalForNonCritical) {
        TestCaseAssertTrue([self.table insertObject:object]);
    }
    [NSThread sleepForTimeInterval:self.backupDelayForNonCritical + self.delayForTolerance];
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
}

- (void)test_auto_backup_when_meet_critical_frames_interval
{
    [self.database removeConfigForName:WCTConfigNameCheckpoint];
    self.database.autoBackup = YES;

    TestCaseObject *object = [self.random autoIncrementTestCaseObject];

    while ([self getWalFrameCount] < self.backupFramesIntervalForCritical - self.framesForTolerance) {
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    [NSThread sleepForTimeInterval:self.backupDelayForCritical + self.delayForTolerance];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.firstMaterialPath]);

    while ([self getWalFrameCount] < self.backupFramesIntervalForCritical) {
        TestCaseAssertTrue([self.table insertObject:object]);
    }
    [NSThread sleepForTimeInterval:self.backupDelayForCritical + self.delayForTolerance];
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
}

- (void)test_cancel_auto_backup
{
    [self.database removeConfigForName:WCTConfigNameCheckpoint];

    self.database.autoBackup = YES;
    TestCaseAssertTrue([self checkAutoBackedup]);

    TestCaseAssertTrue([self.fileManager removeItemAtPath:self.firstMaterialPath error:nil]);
    self.database.autoBackup = NO;
    TestCaseAssertFalse([self checkAutoBackedup]);
}

#warning TODO - find the way to reproduce errors
//- (void)test_error_prone
//{
//    TestCaseAssertTrue([self.database canOpen]);
//
//    NSString* shm = [self.database.path stringByAppendingString:@"-shm"];
//    NSString* wal = [self.database.path stringByAppendingString:@"-wal"];
//
//    TestCaseResult* willBackup = [TestCaseResult no];
//    TestCaseResult* didBackup = [TestCaseResult no];
//
//    TestCaseResult* willCheckpoint = [TestCaseResult no];
//    TestCaseResult* didCheckpoint = [TestCaseResult no];
//
//    TestCaseResult* sleep = [TestCaseResult no];
//    [WCTDatabase globalTraceSQL:^(NSString * sql) {
//        // it will be trigger once by write locker inside Backup
//        if ([sleep isNO] && [sql isEqualToString:@"ROLLBACK"]) {
//            [sleep makeYES];
//            TestCaseAssertResultYES(willBackup);
//            TestCaseAssertResultNO(didBackup);
//            TestCaseAssertResultNO(didCheckpoint);
//            TestCaseLog(@"Will sleep");
//            while ([didCheckpoint isNO]) {}
//        }
//    }];
//
//    [self.dispatch async:^{
//        TestCaseLog(@"Will backup");
//        [willBackup makeYES];
//        TestCaseAssertTrue([self.database backup]);
//        TestCaseAssertResultYES(didCheckpoint);
//        [didBackup makeYES];
//        TestCaseLog(@"Did backup");
//    }];
//
//    // wait until backup start
//    while ([willBackup isNO]) {}
//
//    // Corrupt shm
//    TestCaseAssertTrue([self.fileManager fileExistsAtPath:shm]);
//    int fd = open(shm.UTF8String, O_RDWR);
//    TestCaseAssertTrue(fd != -1);
//    lseek(fd, 48 * 2, SEEK_SET);
//    NSData* dirty = self.random.data;
//    TestCaseAssertTrue(write(fd, dirty.bytes, dirty.length) == dirty.length);
//    close(fd);
//
//    TestCaseLog(@"Will checkpoint");
//    [willCheckpoint makeYES];
//
//    TestCaseAssertTrue([self.database isOpened]);
//    TestCaseAssertTrue([self.table updateProperty:TestCaseObject.content toValue:self.random.string]);
//    for (int i = 0; i < 100; ++i) {
//        TestCaseObject* object = [[TestCaseObject alloc] init];
//        object.content = self.random.string;
//        object.isAutoIncrement = YES;
//        TestCaseAssertTrue([self.table insertObject:object]);
//    }
//    while ([sleep isNO]) {}
//    BOOL result = [self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")];
//    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:wal] == 0);
//    TestCaseAssertTrue(result);
//
//    TestCaseAssertResultNO(didBackup);
//    [didCheckpoint makeYES];
//    TestCaseLog(@"Did checkpoint");
//    [self.dispatch waitUntilDone];
//}

@end
