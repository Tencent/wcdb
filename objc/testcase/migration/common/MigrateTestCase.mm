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
    BOOL done = NO;
    BOOL succeed = [self.database stepMigrationOrDone:done];
    TestCaseAssertTrue(succeed);
    TestCaseAssertFalse(done);

    // check source table migration is not started
    // It's not a good practice.
    TestCaseAssertFalse([[self.sourceDatabase getObjectsOfClass:MigrationObject.class fromTable:self.sourceTable orders:MigrationObject.identifier.asOrder(WCTOrderedAscending)] isEqualToArray:self.objects]);
}

- (void)doTestMigrate
{
    BOOL done = NO;
    BOOL succeed;
    do {
        succeed = [self.database stepMigrationOrDone:done];
    } while (succeed && !done);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(done);

    TestCaseAssertTrue([[self.table getObjectsOrders:MigrationObject.identifier.asOrder(WCTOrderedAscending)] isEqualToArray:self.objects]);

    // check source table is already dropped.
    // It's not a good practice.
    TestCaseAssertFalse([self.database tableExists:self.sourceTable]);
}

- (void)doTestNotification
{
    __block BOOL tableMigrated = NO;
    __block BOOL migrated = NO;
    NSString *expectedTableName = self.tableName;
    [self.database setNotificationWhenMigrated:^(WCTMigrationBaseInfo *info) {
        if (info == nil) {
            migrated = YES;
        } else if ([info.table isEqualToString:expectedTableName]) {
            tableMigrated = YES;
        }
    }];

    BOOL done = NO;
    BOOL succeed;
    do {
        succeed = [self.database stepMigrationOrDone:done];
    } while (succeed && !done);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(done);
    TestCaseAssertTrue(tableMigrated);
    TestCaseAssertTrue(migrated);
}

- (void)doTestFeatureInterruptMigrate
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    TestCaseResult *write = [TestCaseResult yes];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (write.isYES) {
            TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(1)]);
        }
    });

    TestCaseResult *tested = [TestCaseResult no];
    __weak typeof(self) weakSelf = self;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (weakSelf != nil
            && error.code == WCTErrorCodeInterrupt
            && error.level == WCTErrorLevelIgnore
            && error.tag == weakSelf.database.tag) {
            [tested makeYES];
        }
    }];

    BOOL done = NO;
    do {
        TestCaseAssertTrue([self.database stepMigrationOrDone:done]);
    } while (!done && tested.isNO);

    [WCTDatabase resetGlobalErrorTracer];
    [write makeNO];

    TestCaseAssertResultYES(tested);
    TestCaseAssertFalse(done);
}

- (void)doTestAutoMigrate
{
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
    NSUInteger fileSize = [self.database getFilesSize];
    if (self.isCrossDatabaseMigration) {
        fileSize += [self.sourceDatabase getFilesSize];
    }
    int pages = int(fileSize / 4096);

    TestCaseResult *tableMigrated = [TestCaseResult no];
    TestCaseResult *migrated = [TestCaseResult no];
    NSString *expectedTableName = self.tableName;
    [self.database setNotificationWhenMigrated:^(WCTMigrationBaseInfo *info) {
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

- (void)doTestFeatureAutoMigrateWillStopDueToError
{
    TestCaseCounter *numberOfFailures = [TestCaseCounter value:0];
    __weak typeof(self) weakSelf = self;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (weakSelf != nil
            && error.code == WCTErrorCodeIOError
            && error.level == WCTErrorLevelError
            && error.tag == weakSelf.database.tag) {
            [numberOfFailures increment];
        }
    }];

    [WCTDatabase disableSQLiteWrite];
    self.database.autoMigrate = YES;

    // wait until auto migrate stopped
    while (numberOfFailures.value < WCDB::MigrationQueueTolerableFailures)
        ;
    [WCTDatabase enableSQLiteWrite];

    [WCTDatabase resetGlobalErrorTracer];

    TestCaseResult *result = [TestCaseResult yes];
    [self.database traceSQL:^(NSString *sql) {
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
        [result makeYES];
    }];
    [handle invalidate];

    // wait to confirm migration still running.
    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];
    TestCaseAssertResultYES(result);
}

- (int)getUnmigratedCount
{
    // get count of unmigrated rows
    // It's not a good practice.
    return [self.sourceDatabase getValueFromStatement:WCDB::StatementSelect().select(MigrationObject.allProperties.count()).from(self.sourceTable)].numberValue.intValue;
}

- (void)doTestFeatureStepAsLeastAsPossibleButNotWaste
{
#warning TODO - wait for the answer of SQLite staff about the dirty page of ROLLBACK TO stmt.
    //    [self.database removeConfigForName:WCTConfigNameCheckpoint];
    //    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
    //
    //    BOOL done = NO;
    //    BOOL succeed;
    //    int lastWalFrameCount = [self getWalFrameCount];
    //    TestCaseAssertEqual(lastWalFrameCount, 0);
    //    do {
    //        succeed = [self.database stepMigrationOrDone:done];
    //
    //        int walFrameCount = [self getWalFrameCount];
    //        // <=3: 1. sqlite_sequence 2. source table 3. table
    //        TestCaseAssertTrue(walFrameCount - lastWalFrameCount <= 3);
    //        lastWalFrameCount = walFrameCount;
    //    } while (succeed && !done);
    //    TestCaseAssertTrue(succeed);
    //    TestCaseAssertTrue(done);
}

@end
