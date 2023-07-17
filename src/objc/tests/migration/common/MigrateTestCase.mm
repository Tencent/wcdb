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

#import "MigrateTestCase.h"
#import "CoreConst.h"

@implementation MigrateTestCase

- (void)doTestFilter
{
    WCTDatabase *filter = [[WCTDatabase alloc] initWithPath:[self.path stringByAppendingString:@"_filter"]];
    __block int tested = 0;
    NSString *expectedTableName = self.tableName;
    [filter addMigration:nil
        withSourceCipher:nil
              withFilter:^(WCTMigrationUserInfo *userInfo) {
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
    TestCaseAssertOptionalFalse([self.sourceDatabase tableExists:self.sourceTable]);
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

- (void)doTestAutoMigrate
{
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
    [self.database enableAutoMigration:YES];

    TestCaseAssertTrue([self.database canOpen]);

    while (!tableMigrated.value) {
        [NSThread sleepForTimeInterval:self.delayForTolerance];
    }
    while (!migrated.value) {
        [NSThread sleepForTimeInterval:self.delayForTolerance];
    }

    TestCaseAssertResultYES(tableMigrated);
    TestCaseAssertResultYES(migrated);
    TestCaseAssertTrue(self.database.isMigrated);
}

- (void)doTestFeatureClosedDatabaseWillNotPerformAutoMigrate
{
    [self.database enableAutoMigration:YES];

    TestCaseAssertTrue([self.database canOpen]);

    [self.database close];

    [NSThread sleepForTimeInterval:self.delayForTolerance + WCDB::OperationQueueTimeIntervalForMigration];

    TestCaseAssertFalse(self.database.isOpened);
}

- (void)doTestFeatureAutoMigrateWillStopDueToError
{
    TestCaseAssertTrue([self.database canOpen]);

    TestCaseCounter *numberOfFailures = [TestCaseCounter value:0];
    weakify(self);
    [self.database traceError:^(WCTError *error) {
        strongify_or_return(self);
        if (error.code == WCTErrorCodeIOError
            && error.level == WCTErrorLevelError) {
            [numberOfFailures increment];
        }
    }];

    [self.database enableAutoMigration:YES];
    TestCaseAssertTrue([self.database canOpen]);
    [WCTDatabase simulateIOError:WCTSimulateWriteIOError];

    // wait until auto migrate stopped
    while (numberOfFailures.value < WCDB::OperationQueueTolerableFailuresForMigration)
        ;

    // wait to confirm migration is stopped.
    [NSThread sleepForTimeInterval:2 * WCDB::OperationQueueTimeIntervalForMigration];

    TestCaseAssertTrue(numberOfFailures.value == WCDB::OperationQueueTolerableFailuresForMigration);

    [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
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

@end
