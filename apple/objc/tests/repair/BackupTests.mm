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

    [NSThread sleepForTimeInterval:self.delayForTolerance];
    return [self.fileManager fileExistsAtPath:self.database.firstMaterialPath];
}

- (void)test_auto_backup_when_checkpointed
{
    [self.database removeCheckpointConfig];
    self.database.autoBackup = YES;
    TestCaseAssertTrue([self checkAutoBackedup]);
}

- (void)test_auto_backup_when_meet_non_critical_frames_interval
{
    [self.database removeCheckpointConfig];
    self.database.autoBackup = YES;

    TestCaseObject *object = [self.random autoIncrementTestCaseObject];

    while (YES) {
        auto optionalNumberOfWalFrames = [self.database getNumberOfWalFrames];
        TestCaseAssertFalse(optionalNumberOfWalFrames.failed());
        if (optionalNumberOfWalFrames.value() >= self.backupFramesIntervalForNonCritical - self.framesForTolerance) {
            break;
        }
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    [NSThread sleepForTimeInterval:self.backupDelayForNonCritical + self.delayForTolerance];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);

    while (YES) {
        auto optionalNumberOfWalFrames = [self.database getNumberOfWalFrames];
        TestCaseAssertFalse(optionalNumberOfWalFrames.failed());
        if (optionalNumberOfWalFrames.value() >= self.backupFramesIntervalForNonCritical) {
            break;
        }
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    [NSThread sleepForTimeInterval:self.backupDelayForNonCritical + self.delayForTolerance];
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
}

- (void)test_auto_backup_when_meet_critical_frames_interval
{
    [self.database removeCheckpointConfig];
    self.database.autoBackup = YES;

    TestCaseObject *object = [self.random autoIncrementTestCaseObject];

    while (YES) {
        auto optionalNumberOfWalFrames = [self.database getNumberOfWalFrames];
        TestCaseAssertFalse(optionalNumberOfWalFrames.failed());
        if (optionalNumberOfWalFrames.value() >= self.backupFramesIntervalForCritical - self.framesForTolerance) {
            break;
        }
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    [NSThread sleepForTimeInterval:self.backupDelayForCritical + self.delayForTolerance];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);

    while (YES) {
        auto optionalNumberOfWalFrames = [self.database getNumberOfWalFrames];
        TestCaseAssertFalse(optionalNumberOfWalFrames.failed());
        if (optionalNumberOfWalFrames.value() >= self.backupFramesIntervalForCritical) {
            break;
        }
        TestCaseAssertTrue([self.table insertObject:object]);
    }
    [NSThread sleepForTimeInterval:self.backupDelayForCritical + self.delayForTolerance];
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
}

- (void)test_cancel_auto_backup
{
    [self.database removeCheckpointConfig];

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
//    BOOL result = [self.database truncateCheckpoint];
//    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:wal] == 0);
//    TestCaseAssertTrue(result);
//
//    TestCaseAssertResultNO(didBackup);
//    [didCheckpoint makeYES];
//    TestCaseLog(@"Did checkpoint");
//    [self.dispatch waitUntilDone];
//}

- (void)test_feature_closed_database_will_not_perform_auto_backup
{
    [self.database removeCheckpointConfig];
    self.database.autoBackup = YES;

    TestCaseObject *object = [self.random autoIncrementTestCaseObject];
    while (YES) {
        auto optionalNumberOfWalFrames = [self.database getNumberOfWalFrames];
        TestCaseAssertFalse(optionalNumberOfWalFrames.failed());
        if (optionalNumberOfWalFrames.value() >= self.backupFramesIntervalForNonCritical) {
            break;
        }
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    [self.database close];

    [NSThread sleepForTimeInterval:self.backupDelayForNonCritical + self.delayForTolerance];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
}

- (void)test_feature_auto_backup_for_attached
{
    NSString *attachedPath = [self.path stringByAppendingString:@"_attached"];
    NSString *attachedName = @"test_attached";
    WCTDatabase *attachedDatabase = [[WCTDatabase alloc] initWithPath:attachedPath];
    TestCaseAssertTrue([attachedDatabase createTable:self.tableName withClass:TestCaseObject.class]);

    self.database.autoBackup = YES;
    TestCaseAssertTrue([self.database execute:WCDB::StatementAttach().attach(attachedPath).as(attachedName)]);

    TestCaseObject *object = [self.random autoIncrementTestCaseObject];

    {
        // auto backup for main schema will not be triggered if operating other schemas
        // auto backup for other schema will not be triggered if it's not set up
        WCTInsert *insert = [[self.table prepareInsert] value:object];
        insert.statement.schema(attachedName);
        TestCaseAssertTrue([insert execute]);

        TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).schema(attachedName).with("TRUNCATE")]);

        [NSThread sleepForTimeInterval:self.delayForTolerance];
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:attachedDatabase.firstMaterialPath]);
    }

    {
        // auto backup for other schema will be triggered if it's set up
        attachedDatabase.autoBackup = YES;

        WCTInsert *insert = [[self.table prepareInsert] value:object];
        insert.statement.schema(attachedName);
        TestCaseAssertTrue([insert execute]);

        TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).schema(attachedName).with("TRUNCATE")]);

        [NSThread sleepForTimeInterval:self.delayForTolerance];
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:attachedDatabase.firstMaterialPath]);
    }
}

@end
