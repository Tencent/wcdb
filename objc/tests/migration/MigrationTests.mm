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

#import "MigrationTestCase.h"
#import <WCDB/CoreConst.h>

@interface MigrationTests : MigrationTestCase

@end

@implementation MigrationTests

- (void)setUp
{
    self.isCrossDatabaseMigration = NO;
    [super setUp];
}

- (void)test_filter
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

- (void)test_step_migrate
{
    BOOL done = NO;
    BOOL succeed;
    do {
        succeed = [self.database stepMigration:YES done:done];
    } while (succeed && !done);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(done);
    // check source table is already dropped.
    // It's not a good practice.
    TestCaseAssertFalse([self.database tableExists:self.sourceTable]);
}

- (void)test_notification
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
        TestCaseAssertFalse(tableMigrated);
        TestCaseAssertFalse(migrated);
        succeed = [self.database stepMigration:YES done:done];
    } while (succeed && !done);
    TestCaseAssertTrue(succeed);
    TestCaseAssertTrue(done);
    TestCaseAssertTrue(tableMigrated);
    TestCaseAssertTrue(migrated);
}

- (void)test_feature_interrupt_migrate
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    __block BOOL tested = NO;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (error.code == WCTErrorCodeInterrupt) {
            tested = YES;
        }
    }];

    BOOL done;
    TestCaseAssertTrue([self.database stepMigration:NO done:done]);
    TestCaseAssertTrue(tested);
    [WCTDatabase resetGlobalErrorTracer];
}

- (void)test_feature_force_migrate
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    __block BOOL tested = YES;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (error.code == WCTErrorCodeInterrupt) {
            tested = NO;
        }
    }];

    BOOL done;
    TestCaseAssertTrue([self.database stepMigration:YES done:done]);
    TestCaseAssertTrue(tested);

    [WCTDatabase resetGlobalErrorTracer];
    [handle invalidate];
}

- (void)test_auto_migrate
{
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]);
    NSUInteger fileSize = [self.database getFilesSize];
    int pages = int(fileSize / 4096);

    __block BOOL tested = NO;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (error.code == WCTErrorCodeInterrupt) {
            tested = YES;
        }
    }];

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

    [WCTDatabase resetGlobalErrorTracer];
}

- (void)test_feature_auto_migrate_will_stop_due_to_error
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

- (void)test_feature_auto_migrate_will_not_stop_due_to_interrupt
{
    WCTHandle *handle = [self.database getHandle];
    TestCaseAssertTrue([handle validate]);

    __block int failures = 0;
    [WCTDatabase globalTraceError:^(WCTError *error) {
        if (error.code == WCTErrorCodeInterrupt) {
            OSAtomicIncrement32(&failures);
        }
    }];

    self.database.autoMigrate = YES;

    // wait until auto migrate reach the normal failure threshold
    while (failures < WCDB::MigrationQueueTolerableFailures)
        ;

    __block BOOL tested = NO;
    [self.database traceSQL:^(NSString *sql) {
        tested = YES;
    }];
    // wait to confirm migration still running.
    [NSThread sleepForTimeInterval:2 * WCDB::MigrationQueueTimeIntervalForMigrating];
    TestCaseAssertTrue(tested);
}

@end
