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

#import "MigrateTestCase.h"
#import "CoreConst.h"

@implementation MigrateTestCase

- (void)doTestFilter
{
    WCTDatabase *filter = [[WCTDatabase alloc] initWithPath:[self.path stringByAppendingString:@"_filter"]];
    __block int tested = 0;
    NSString *expectedTableName = self.tableName;
    [filter filterMigration:^(WCTMigrationUserInfo *userInfo) {
        if ([userInfo.table isEqualToString:expectedTableName]) {
            ++tested;
        }
    }];
    TestCaseAssertTrue([filter getObjectOfClass:MigrationObject.class fromTable:self.tableName] == nil);
    TestCaseAssertTrue([filter getObjectOfClass:MigrationObject.class fromTable:self.tableName] == nil);
    TestCaseAssertTrue(tested == 1);
}

- (void)doTestStepMigrate
{
    BOOL succeed = [self.database stepMigration];
    TestCaseAssertTrue(succeed);
    TestCaseAssertFalse([self.database isMigrated]);

    // check source table migration is not started
    // It's not a good practice.
    TestCaseAssertFalse([[self.sourceDatabase getObjectsOfClass:MigrationObject.class fromTable:self.sourceTable orders:MigrationObject.identifier.asOrder(WCTOrderedAscending)] isEqualToArray:self.objects]);
}

- (void)doTestMigrate
{
    TestCaseAssertFalse(self.database.isMigrated);
    BOOL succeed;
    do {
        succeed = [self.database stepMigration];
    } while (succeed && ![self.database isMigrated]);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(self.database.isMigrated);

    TestCaseAssertTrue([[self.table getObjectsOrders:MigrationObject.identifier.asOrder(WCTOrderedAscending)] isEqualToArray:self.objects]);

    // check source table is already dropped.
    // It's not a good practice.
    TestCaseAssertFalse([self.sourceDatabase tableExists:self.sourceTable]);
}

- (void)doTestNotification
{
    __block BOOL tableMigrated = NO;
    __block int migrated = 0;
    NSString *expectedTableName = self.tableName;
    WCTTag tag = self.database.tag;
    [self.database setNotificationWhenMigrated:^(WCTDatabase *database, WCTMigrationBaseInfo *info) {
        TestCaseAssertEqual(tag, database.tag);
        if (info == nil) {
            ++migrated;
            TestCaseAssertTrue(database.isMigrated);
        } else if ([info.table isEqualToString:expectedTableName]) {
            tableMigrated = YES;
        }
    }];

    BOOL succeed;
    do {
        succeed = [self.database stepMigration];
    } while (succeed && ![self.database isMigrated]);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue([self.database isMigrated]);
    TestCaseAssertTrue(tableMigrated);
    TestCaseAssertEqual(migrated, 1);

    // Notification will receive multiple times.
    succeed = [self.database stepMigration];
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue([self.database isMigrated]);
    TestCaseAssertEqual(migrated, 2);
}

- (void)doTestFeatureInterruptMigrate
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    TestCaseResult *write = [TestCaseResult yes];

    [self.dispatch async:^{
        while (write.isYES) {
            [NSThread sleepForTimeInterval:0.5];
            TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(1)]);
        }
    }];

    TestCaseCounter *tested = [TestCaseCounter value:0];
    weakify(self);
    [WCTDatabase globalTraceError:^(WCTError *error) {
        strongify_or_return(self);
        if (error.code == WCTErrorCodeInterrupt
            && error.level == WCTErrorLevelIgnore
            && error.tag == self.database.tag) {
            [tested increment];
        }
        if (error.level == WCTErrorLevelError) {
            TestCaseLog(@"hit");
        }
    }];

    do {
        if ([self.database isMigrated]) {
            // add more table to trigger migration
            NSString *table = [NSString stringWithFormat:@"t_%@", self.random.string];
            NSString *sourceTable = [NSString stringWithFormat:@"t_source_%@", self.random.string];
            [self.toMigrate setObject:sourceTable forKey:table];
            TestCaseAssertTrue([self.sourceDatabase createTable:sourceTable withClass:TestCaseObject.class]);
            TestCaseAssertTrue([self.database createTable:table withClass:TestCaseObject.class]);
        }
        TestCaseAssertTrue([self.database stepMigration]);
    } while (YES);

    [WCTDatabase globalTraceError:nil];
    [write makeNO];

    //    TestCaseAssertResultYES(tested);
    TestCaseAssertFalse([self.database isMigrated]);
    [self.dispatch waitUntilDone];
}

- (void)doTestAutoMigrate
{
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    NSUInteger fileSize = [self.database getFilesSize];
    if (self.isCrossDatabaseMigration) {
        fileSize += [self.sourceDatabase getFilesSize];
    }
    int pages = int(fileSize / 4096);

    TestCaseResult *tableMigrated = [TestCaseResult no];
    TestCaseResult *migrated = [TestCaseResult no];
    NSString *expectedTableName = self.tableName;
    [self.database setNotificationWhenMigrated:^(WCTDatabase *database, WCTMigrationBaseInfo *info) {
        WCDB_UNUSED(database);
        if (info == nil) {
            [migrated makeYES];
        } else if ([info.table isEqualToString:expectedTableName]) {
            [tableMigrated makeYES];
        }
    }];
    self.database.autoMigrate = YES;

    // wait until auto migrate done
    [NSThread sleepForTimeInterval:(pages + 2) * WCDB::MigrationQueueTimeIntervalForMigrating];

    TestCaseAssertResultYES(tableMigrated);
    TestCaseAssertResultYES(migrated);
}

- (void)doTestFeatureClosedDatabaseWillNotPerformAutoMigrate
{
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    NSUInteger fileSize = [self.database getFilesSize];
    if (self.isCrossDatabaseMigration) {
        fileSize += [self.sourceDatabase getFilesSize];
    }
    int pages = int(fileSize / 4096);

    TestCaseResult *tableMigrated = [TestCaseResult no];
    TestCaseResult *migrated = [TestCaseResult no];
    NSString *expectedTableName = self.tableName;
    [self.database setNotificationWhenMigrated:^(WCTDatabase *database, WCTMigrationBaseInfo *info) {
        WCDB_UNUSED(database);
        if (info == nil) {
            [migrated makeYES];
        } else if ([info.table isEqualToString:expectedTableName]) {
            [tableMigrated makeYES];
        }
    }];
    [self.database close];
    self.database.autoMigrate = YES;

    // wait until auto migrate done
    [NSThread sleepForTimeInterval:(pages + 2) * WCDB::MigrationQueueTimeIntervalForMigrating];

    TestCaseAssertResultNO(tableMigrated);
    TestCaseAssertResultNO(migrated);
}

- (void)doTestFeatureAutoMigrateWillStopDueToError
{
    TestCaseAssertTrue([self.database canOpen]);

    TestCaseCounter *numberOfFailures = [TestCaseCounter value:0];
    weakify(self);
    [WCTDatabase globalTraceError:^(WCTError *error) {
        strongify_or_return(self);
        if (error.code == WCTErrorCodeIOError
            && error.level == WCTErrorLevelError
            && error.tag == self.database.tag) {
            [numberOfFailures increment];
        }
    }];

    [WCTDatabase simulateIOError:WCTSimulateWriteIOError];
    self.database.autoMigrate = YES;

    // wait until auto migrate stopped
    while (numberOfFailures.value < WCDB::MigrationQueueTolerableFailures)
        ;
    [WCTDatabase simulateIOError:WCTSimulateNoneIOError];

    [WCTDatabase globalTraceError:nil];

    TestCaseResult *result = [TestCaseResult yes];
    [self.database traceSQL:^(NSString *sql) {
        WCDB_UNUSED(sql)
        [result makeNO];
    }];
    // wait to confirm migration is stopped.
    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];
    TestCaseAssertResultYES(result);
}

- (void)doTestFeatureAutoMigrateWillNotStopDueToInterrupt
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    self.database.autoMigrate = YES;

    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];

    TestCaseResult *result = [TestCaseResult no];
    [self.database traceSQL:^(NSString *sql) {
        WCDB_UNUSED(sql)
        [result makeYES];
    }];
    [handle invalidate];

    // wait to confirm migration still running.
    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];
    TestCaseAssertResultYES(result);
}

- (void)doTestFeatureMigrateNewlyCreatedTableAfterMigrated
{
    NSString *anotherSourceTable = @"testAnotherSourceTable";
    NSString *anotherTable = @"testAnotherTable";
    [self.toMigrate setObject:anotherSourceTable forKey:anotherTable];
    TestCaseAssertTrue([self.sourceDatabase createTable:anotherSourceTable withClass:TestCaseObject.class]);
    do {
        TestCaseAssertTrue([self.database stepMigration]);
    } while (![self.database isMigrated]);
    TestCaseAssertTrue(self.database.isMigrated);

    TestCaseAssertTrue([self.database createTable:anotherTable withClass:TestCaseObject.class]);

    // migrated status is reset.
    TestCaseAssertFalse(self.database.isMigrated);

    do {
        TestCaseAssertTrue([self.database stepMigration]);
    } while (![self.database isMigrated]);
    TestCaseAssertTrue(self.database.isMigrated);
}

- (void)doTestFeatureStepAsLeastAsPossibleButNotWaste
{
#warning TODO - wait for the answer of SQLite staff about the dirty page of ROLLBACK TO stmt.
    //    [self.database removeCheckpointConfig];
    //    TestCaseAssertTrue([self.database truncateCheckpoint]);
    //
    //    BOOL succeed;
    //    int lastWalFrameCount = [self getNumberOfWalFrames];
    //    TestCaseAssertEqual(lastWalFrameCount, 0);
    //    do {
    //        succeed = [self.database stepMigration];
    //
    //        int walFrameCount = [self getNumberOfWalFrames];
    //        // <=3: 1. sqlite_sequence 2. source table 3. table
    //        TestCaseAssertTrue(walFrameCount - lastWalFrameCount <= 3);
    //        lastWalFrameCount = walFrameCount;
    //    } while (succeed && ![self.database isMigrated]);
    //    TestCaseAssertTrue(succeed);
    //    TestCaseAssertTrue(done);
}

@end
