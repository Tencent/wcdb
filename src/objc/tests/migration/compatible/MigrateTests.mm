//
// Created by qiuwenchen on 2023/7/25.
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

#import "CoreConst.h"
#import "MigrationTestCase.h"
#import <Foundation/Foundation.h>

@interface MigrateTests : MigrationTestCase

@end

@implementation MigrateTests

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
}

- (void)doTestMigrate:(void (^)(void))operation
{
    NSArray<Class<MigrationTestSourceObject>> *sourceClasses = [MigrationTestCase allSourceClasses];
    NSArray<Class<MigrationTestObject>> *targetClasses = [MigrationTestCase allClasses];
    for (Class<MigrationTestSourceObject> sourceClass in sourceClasses) {
        for (Class<MigrationTestObject> targetClass in targetClasses) {
            self.sourceClass = sourceClass;
            self.targetClass = targetClass;

            for (int i = 0; i < 4; i++) {
                self.isCrossDatabaseMigration = (i & 1) > 0;
                self.needFilter = (i & 2) > 0;
                TestCaseLog(@"Test migrate: sourceClass %@, targetClass %@, isCrossDatabase %d, needFilter %d",
                            self.sourceClass,
                            self.targetClass,
                            self.isCrossDatabaseMigration,
                            self.needFilter);
                operation();
            }
        }
    }
}

- (void)doTestTableAndDatabaseMigrate:(void (^)(void))operation
{
    self.sourceClass = [MigrationTestCase allSourceClasses].firstObject;
    self.targetClass = [MigrationTestCase allClasses].firstObject;

    for (int i = 0; i < 2; i++) {
        self.isCrossDatabaseMigration = i > 0;
        [self configMigration];
        operation();
    }
}

- (void)test_filter
{
    [self doTestMigrate:^{
        [self clearData];
        [self createTable];
        __block int tested = 0;
        [self.database addMigration:self.sourcePath
                   withSourceCipher:nil
                         withFilter:^(WCTMigrationUserInfo *info) {
                             if ([info.table isEqualToString:self.tableName]) {
                                 [info setSourceTable:self.sourceTableName];
                                 if (self.needFilter) {
                                     [info setFilterCondition:[self.sourceClass classification] == MigrationClassificationB];
                                 }
                                 tested++;
                             }
                         }];
        WCTValue *count = [self.table getValueOnResultColumn:WCDB::Column::all().count()];
        TestCaseAssertTrue(count != nil && count.numberValue.intValue == 0);
        NSArray *objects = [self.table getObjects];
        TestCaseAssertTrue(objects != nil && objects.count == 0);

        TestCaseAssertTrue(tested == 1);
    }];
}

- (void)test_migrate
{
    [self doTestMigrate:^{
        [self configMigration];
        TestCaseAssertFalse(self.database.isMigrated);

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
        TestCaseAssertTrue(self.database.isMigrated);

        TestCaseAssertTrue([[self.table getObjectsOrders:[self.targetClass identifier].asOrder(WCTOrderedAscending)] isEqualToArray:self.filterObjects]);

        // check source table is already dropped.
        // It's not a good practice.
        TestCaseAssertOptionalEqual([self.sourceDatabase tableExists:self.sourceTableName], self.needFilter);

        TestCaseAssertTrue(tableMigrated);
        TestCaseAssertEqual(migrated, 1);

        // Notification will receive multiple times.
        succeed = [self.database stepMigration];
        TestCaseAssertTrue(succeed);
        TestCaseAssertTrue([self.database isMigrated]);
        TestCaseAssertEqual(migrated, 2);
    }];
}

- (void)test_auto_migrate
{
    [self doTestTableAndDatabaseMigrate:^{
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
    }];
}

- (void)test_feature_closed_database_will_not_perform_auto_migrate
{
    [self doTestTableAndDatabaseMigrate:^{
        [self.database enableAutoMigration:YES];

        TestCaseAssertTrue([self.database canOpen]);

        [self.database close];

        [NSThread sleepForTimeInterval:self.delayForTolerance + WCDB::OperationQueueTimeIntervalForMigration];

        TestCaseAssertFalse(self.database.isOpened);
    }];
}

- (void)test_feature_auto_migrate_will_stop_due_to_error
{
    [self doTestTableAndDatabaseMigrate:^{
        TestCaseAssertTrue([self.database canOpen]);

        TestCaseCounter *numberOfFailures = [TestCaseCounter value:0];
        weakify(self);
        [self.database traceError:nil];
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
    }];
}

- (void)test_feature_migrate_newly_created_table_after_migrated
{
    [self doTestTableAndDatabaseMigrate:^{
        NSString *anotherSourceTable = @"testAnotherSourceTable";
        NSString *anotherTable = @"testAnotherTable";
        TestCaseAssertTrue([self.sourceDatabase createTable:anotherSourceTable withClass:self.sourceClass]);
        [self.database addMigration:self.sourcePath
                         withFilter:^(WCTMigrationUserInfo *info) {
                             if ([info.table isEqualToString:anotherTable]) {
                                 [info setSourceTable:anotherSourceTable];
                             }
                         }];
        do {
            TestCaseAssertTrue([self.database stepMigration]);
        } while (![self.database isMigrated]);
        TestCaseAssertTrue(self.database.isMigrated);

        TestCaseAssertTrue([self.database createTable:anotherTable withClass:self.targetClass]);

        // migrated status is reset.
        TestCaseAssertFalse(self.database.isMigrated);

        do {
            TestCaseAssertTrue([self.database stepMigration]);
        } while (![self.database isMigrated]);
        TestCaseAssertTrue(self.database.isMigrated);
    }];
}

@end
