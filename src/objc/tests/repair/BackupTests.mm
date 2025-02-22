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
#import "NormalRepairTestObject.h"
#import "Random+RepairTestObject.h"
@interface BackupTests : BackupTestCase

@end

@implementation BackupTests

- (void)test_insert
{
    __block NSArray *newObjects = nil;
    [self
    executeFullTest:^{
        newObjects = [[Random shared] repairObjectsWithClass:self.testClass andCount:100 startingFromIdentifier:self.objects.lastObject.identifier + 1];
        XCTAssertTrue([self.table insertObjects:newObjects]);
    }
    withCheck:^{
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        [self checkObjects:self.objects containedIn:allObjects];
        if (!self.corruptHeader || self.incrementalBackup) {
            [self checkObjects:newObjects containedIn:allObjects];
        }
    }];
}

- (void)test_update
{
    [self
    executeFullTest:^{
        XCTAssertTrue([self.table updateProperty:[self.testClass textValue] toValue:@"abc" where:[self.testClass identifier] < 50]);
    }
    withCheck:^{
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        if (!self.corruptHeader || self.incrementalBackup) {
            for (NSObject<RepairTestObject> *object in self.objects) {
                if (object.identifier < 50) {
                    object.textValue = @"abc";
                }
            }
        }
        [self checkObjects:self.objects containedIn:allObjects];
    }];
}

- (void)test_delete
{
    [self
    executeFullTest:^{
        XCTAssertTrue([self.table deleteObjectsWhere:[self.testClass identifier] > 50]);
    }
    withCheck:^{
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        if (!self.corruptHeader || self.incrementalBackup) {
            [self.objects removeObjectsInRange:NSMakeRange(50, 50)];
        }
        [self checkObjects:self.objects containedIn:allObjects];
    }];
}

- (void)test_delete_all
{
    [self
    executeFullTest:^{
        XCTAssertTrue([self.table deleteObjects]);
        XCTAssertTrue([self.database passiveCheckpoint]);
        usleep(10000);
        XCTAssertTrue([self.database backup]);
    }
    withCheck:^{
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        if (!self.incrementalBackup) {
            TestCaseAssertTrue(allObjects.count == 0);
        } else {
            TestCaseAssertTrue(allObjects.firstObject.identifier == 1000);
        }
    }];
}

- (void)test_rename_table
{
    NSString *newTable = @"newTable";
    [self
    executeFullTest:^{
        XCTAssertTrue([self.database execute:WCDB::StatementAlterTable().alterTable(self.tableName).renameToTable(newTable)]);
        self.table = [self.database getTable:newTable withClass:self.testClass];
    }
    withCheck:^{
        if (self.corruptHeader && !self.incrementalBackup) {
            self.table = [self.database getTable:self.tableName withClass:self.testClass];
        }
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        [self checkObjects:self.objects containedIn:allObjects];
    }];
}

- (void)test_add_new_column
{
    NSString *newColumn = @"newColumn";
    [self
    executeFullTest:^{
        XCTAssertTrue([self.database execute:WCDB::StatementAlterTable().alterTable(self.tableName).addColumn(WCDB::ColumnDef(newColumn, WCDB::ColumnType::Integer))]);
    }
    withCheck:^{
        if (self.incrementalBackup) {
            WCTOneRow *row = [self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Column(newColumn)).from(self.tableName)];
            XCTAssertTrue(row != nil && row.count == 1 && row[0] == [NSNull null]);
        }
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        [self checkObjects:self.objects containedIn:allObjects];
    }];
}

- (void)test_drop_table
{
    [self
    executeFullTest:^{
        XCTAssertTrue([self.database dropTable:self.tableName]);
    }
    withCheck:^{
        if (!self.incrementalBackup) {
            NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
            [self checkObjects:self.objects containedIn:allObjects];
        } else {
            XCTAssertFalse([self.database tableExists:self.tableName]);
        }
    }];
}

- (void)test_truncate_checkpoint
{
    [self
    executeFullTest:^{
        XCTAssertTrue([self.database truncateCheckpoint]);
    }
    withCheck:^{
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        if (!self.corruptHeader || self.incrementalBackup) {
            [self checkObjects:self.objects containedIn:allObjects];
        }
        XCTAssertTrue(allObjects.count > 0);
    }];
}

- (void)test_vacuum
{
    [self
    executeFullTest:^{
        XCTAssertTrue([self.database execute:WCDB::StatementVacuum().vacuum()]);
    }
    withCheck:^{
        NSArray<NSObject<RepairTestObject> *> *allObjects = [self.table getObjects];
        [self checkObjects:self.objects containedIn:allObjects];
    }];
}

- (void)test_create_table_and_backup
{
    [self.database enableAutoBackup:true];
    [self.database enableAutoCheckpoint:false];
    size_t firstMaterialSize = 0;
    for (int i = 0; i < 100; i++) {
        bool ret = [self.database createTable:[NSString stringWithFormat:@"testTable_%d", i] withClass:NormalRepairTestObject.class];
        XCTAssertTrue(ret);
        [self.database passiveCheckpoint];
        [self.database backup];
        if (firstMaterialSize == 0) {
            firstMaterialSize = [self.fileManager getFileSizeIfExists:self.database.firstMaterialPath];
        }
    }
    size_t lastMaterialSize = [self.fileManager getFileSizeIfExists:self.database.lastMaterialPath];

    TestCaseAssertTrue(lastMaterialSize < 200 * firstMaterialSize);
    TestCaseAssertTrue([self.database retrieve:nil] > 0);

    NSArray<WCTMaster *> *masters = [self.database getObjectsOfClass:WCTMaster.class fromTable:WCTMaster.tableName];
    // They are sqlite_sequence, 100 tables and 100 indexes.
    TestCaseAssertTrue(masters.count == 201);
}

- (void)test_dual_backup
{
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

    TestCaseAssertTrue([self.database backup]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

    [NSThread sleepForTimeInterval:0.1];
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

    [NSThread sleepForTimeInterval:0.1];
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
    [self executeTest:^{
        TestCaseAssertTrue([self.database removeFiles]);
        TestCaseAssertTrue([self.database backup]);
    }];
}

- (void)test_filter
{
    [self.database filterBackup:^BOOL(NSString *tableName) {
        if ([tableName isEqualToString:self.tableName] or [NSString stringWithFormat:@"'%@'", self.tableName]) {
            return false;
        }
        return true;
    }];
    [self
    executeFullTest:^{
    }
    withCheck:^{
        XCTAssertFalse([self.database tableExists:self.tableName]);
    }];
}

- (void)test_backup_fail
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    TestCaseAssertTrue([fileManager createDirectoryAtPath:self.database.firstMaterialPath withIntermediateDirectories:YES attributes:nil error:nil]);
    TestCaseAssertTrue([fileManager createDirectoryAtPath:self.database.lastMaterialPath withIntermediateDirectories:YES attributes:nil error:nil]);

    TestCaseAssertFalse([self.database backup]);
}

- (void)test_auto_backup_when_checkpointed
{
    [self.database enableAutoCheckpoint:NO];
    [self.database enableAutoBackup:YES];
    TestCaseObject *object = [Random.shared autoIncrementTestCaseObject];
    XCTAssertTrue([self createTable]);
    TestCaseAssertTrue([self.table insertObject:object]);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);
    TestCaseAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForBackup + self.delayForTolerance];
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);

    TestCaseAssertTrue([self.table insertObject:object]);
    TestCaseAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForBackup + self.delayForTolerance];

    TestCaseAssertTrue([self.table insertObject:object]);
    TestCaseAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);

    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.lastMaterialPath]);
}

- (void)test_incremental_backup_interrupted
{
    __block bool tested = false;
    [self.database traceError:^(WCTError *error) {
        if (error.level == WCTErrorLevelWarning && [error.message isEqualToString:@"Expired incremental Material"]) {
            tested = true;
        }
    }];
    [self.database enableAutoBackup:YES];
    [self.database enableAutoCheckpoint:NO];

    //Initialize backup matrial
    [self createTable];
    XCTAssertTrue([self.table insertObjects:[Random.shared autoIncrementTestCaseObjectsWithCount:2]]);
    XCTAssertTrue([self.database passiveCheckpoint]);
    XCTAssertTrue([self.database backup]);
    usleep(10000);
    XCTAssertFalse(tested);

    //Incremental backup
    XCTAssertTrue([self.table insertObjects:[Random.shared autoIncrementTestCaseObjectsWithCount:2]]);
    XCTAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);
    XCTAssertFalse(tested);

    [self.database enableAutoBackup:false];

    //Incremental backup interrupted
    XCTAssertTrue([self.table insertObjects:[Random.shared autoIncrementTestCaseObjectsWithCount:2]]);
    XCTAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);
    XCTAssertFalse(tested);

    [self.database enableAutoBackup:true];

    //Incremental backup matrial expired
    XCTAssertTrue([self.table insertObjects:[Random.shared autoIncrementTestCaseObjectsWithCount:2]]);
    XCTAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);
    XCTAssertTrue(tested);
}

@end
