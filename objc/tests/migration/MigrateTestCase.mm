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
#import <WCDB/CoreConst.h>

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
    TestCaseAssertTrue([filter getObjectOfClass:TestCaseObject.class fromTable:self.tableName] == nil);
    TestCaseAssertTrue([filter getObjectOfClass:TestCaseObject.class fromTable:self.tableName] == nil);
    TestCaseAssertTrue(tested == 1);
}

- (void)doTestStepMigrate
{
    BOOL done = NO;
    BOOL succeed = [self.database stepMigration:YES done:done];
    TestCaseAssertTrue(succeed);
    TestCaseAssertFalse(done);

    // check source table migration is not started
    // It's not a good practice.
    TestCaseAssertFalse([[self.table getObjects] isEqualToArray:self.objects]);
}

- (void)doTestMigrate
{
    BOOL done = NO;
    BOOL succeed;
    do {
        succeed = [self.database stepMigration:YES done:done];
    } while (succeed && !done);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(done);

    TestCaseAssertTrue([[self.table getObjects] isEqualToArray:self.objects]);

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
        succeed = [self.database stepMigration:YES done:done];
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

    BOOL done;
    TestCaseAssertTrue([self.database stepMigration:YES done:done]);
    TestCaseAssertFalse(done);

    // check source table migration is not started.
    // It's not a good practice.
    TestCaseAssertTrue([[self.table getObjects] isEqualToArray:self.objects]);
    [handle invalidate];
}

- (void)doTestFeatureForceMigrate
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    BOOL done;
    TestCaseAssertTrue([self.database stepMigration:NO done:done]);
    TestCaseAssertFalse(done);

    // check source table migration is started.
    // It's not a good practice.
    TestCaseAssertFalse([[self.table getObjects] isEqualToArray:self.objects]);
    [handle invalidate];
}

- (void)doTestAutoMigrate
{
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
    NSUInteger fileSize = [self.database getFilesSize];
    int pages = int(fileSize / 4096);

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
    self.database.autoMigrate = YES;

    // wait until auto migrate done
    [NSThread sleepForTimeInterval:(pages + 2) * WCDB::MigrationQueueTimeIntervalForMigrating];

    TestCaseAssertTrue(tableMigrated);
    TestCaseAssertTrue(migrated);
}

- (void)doTestFeatureAutoMigrateWillStopDueToError
{
    __block int failures = 0;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (error.code == WCTErrorCodeIOError) {
            OSAtomicIncrement32(&failures);
        }
    }];

    [self.console disableSQLiteWrite];
    self.database.autoMigrate = YES;

    // wait until auto migrate stopped
    while (failures < WCDB::MigrationQueueTolerableFailures)
        ;
    [self.console enableSQLiteWrite];

    __block BOOL tested = YES;
    [self.database traceSQL:^(NSString *sql) {
        tested = NO;
    }];
    // wait to confirm migration is stopped.
    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];
    TestCaseAssertTrue(tested);
}

- (void)doTestFeatureAutoMigrateWillNotStopDueToInterrupt
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    self.database.autoMigrate = YES;

    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];

    __block BOOL tested = NO;
    [self.database traceSQL:^(NSString *sql) {
        tested = YES;
    }];
    [handle invalidate];

    // wait to confirm migration still running.
    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];
    TestCaseAssertTrue(tested);
}

- (int)getUnmigratedCount
{
    // get count of unmigrated rows
    // It's not a good practice.
    return [self.sourceDatabase getValueFromStatement:WCDB::StatementSelect().select(TestCaseObject.allProperties.count()).from(self.sourceTable)].numberValue.intValue;
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
    //        succeed = [self.database stepMigration:YES done:done];
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
