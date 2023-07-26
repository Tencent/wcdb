//
// Created by qiuwenchen on 2022/8/14.
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

#import "CPPTestCase.h"

@interface CPPDatabaseTests : CPPTableTestCase

@end

@implementation CPPDatabaseTests

- (void)test_tag
{
    TestCaseAssertNotEqual(self.database->getTag(), WCDB::Tag::invalid());
    WCDB::Database another = WCDB::Database(self.path.UTF8String);
    TestCaseAssertEqual(self.database->getTag(), another.getTag());
}

- (void)test_path
{
    TestCaseAssertCPPStringEqual(self.database->getPath().data(), self.path.UTF8String);
}

- (void)test_open_and_close
{
    TestCaseAssertFalse(self.database->isOpened());
    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertTrue(self.database->isOpened());
    self.database->close();
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_blockade
{
    self.database->blockade();
    __block NSDate* subthread;
    [self.dispatch async:^{
        TestCaseAssertTrue(self.database->canOpen());
        subthread = [NSDate date];
    }];
    [NSThread sleepForTimeInterval:1];
    NSDate* main = [NSDate date];
    self.database->unblockade();
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_blockaded_close
{
    NSDate* main = [NSDate date];
    NSDate* subthread = [NSDate date];
    self.database->close([&]() {
        [self.dispatch async:^{
            TestCaseAssertTrue(self.database->canOpen());
            subthread = [NSDate date];
        }];
        [NSThread sleepForTimeInterval:1];
        main = [NSDate date];
    });
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_readonly
{
    WCDB::OneRowValue row = [Random.shared autoIncrementTestCaseValuesWithCount:1][0];
    TestCaseAssertTrue([self createValueTable]);
    TestCaseAssertTrue(self.database->insertRows(row, self.columns, self.tableName.UTF8String));

    self.database->close([=]() {
        for (const WCDB::UnsafeStringView path : self.database->getPaths()) {
            NSString* nsPath = [NSString stringWithUTF8String:path.data()];
            if ([self.fileManager fileExistsAtPath:nsPath]) {
                TestCaseAssertTrue([self.fileManager setAttributes:@{ NSFileImmutable : @(YES) } ofItemAtPath:nsPath error:nil]);
            }
        }
    });

    TestCaseAssertTrue(self.database->canOpen());
    auto values = self.database->selectAllRow(self.resultColumns, self.tableName.UTF8String);
    TestCaseAssertTrue(values.succeed() && values.value().size() == 1);
    XCTAssertFalse(self.database->insertRows(row, self.columns, self.tableName.UTF8String));

    // reset attribute
    for (const WCDB::UnsafeStringView path : self.database->getPaths()) {
        NSString* nsPath = [NSString stringWithUTF8String:path.data()];
        if ([self.fileManager fileExistsAtPath:nsPath]) {
            TestCaseAssertTrue([self.fileManager setAttributes:@{ NSFileImmutable : @(NO) } ofItemAtPath:nsPath error:nil]);
        }
    }
}

- (void)test_feature_run_while_closing
{
    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertTrue(self.database->isOpened());
    self.database->close([=]() {
        TestCaseAssertTrue(self.database->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123)));
    });
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_purge
{
    // acquire handle
    TestCaseAssertTrue(self.database->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())));

    TestCaseAssertTrue(self.database->isOpened());
    self.database->purge();
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_purge_all
{
    // acquire handle
    TestCaseAssertTrue(self.database->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())));

    TestCaseAssertTrue(self.database->isOpened());
    WCDB::Database::purgeAll();
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_checkpoint
{
    WCDB::MultiRowsValue rows = [Random.shared autoIncrementTestCaseValuesWithCount:100];

    TestCaseAssertTrue([self createValueTable]);

    unsigned long walFrameNum = 0;
    for (int i = 0; i < 100; i++) {
        auto value = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column::all().count()).from(self.tableName.UTF8String));
        TestCaseAssertTrue(value.succeed() && value.value().intValue() == i);
        TestCaseAssertTrue(self.database->insertRows(rows[i], self.columns, self.tableName.UTF8String));
        self.database->passiveCheckpoint();
        self.database->close([&]() {
            if (walFrameNum != 0) {
                TestCaseAssertTrue(walFrameNum == [self getNumberOfWalFrames]);
            } else {
                walFrameNum = [self getNumberOfWalFrames];
            }
            TestCaseAssertTrue(walFrameNum > 0);
            for (const WCDB::UnsafeStringView& path : self.database->getPaths()) {
                NSString* nsPath = [NSString stringWithUTF8String:path.data()];
                if (path.hasSuffix("wal")) {
                    TestCaseAssertTrue([self.fileManager fileExistsAtPath:nsPath]);
                    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:nsPath] > 32);
                } else if (path.hasSuffix("shm")) {
                    TestCaseAssertTrue([self.fileManager fileExistsAtPath:nsPath]);
                    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:nsPath] >= 32 * 1024);
                }
            }
        });
    }
}

- (void)test_open_fail
{
    auto database = WCDB::Database(self.directory.UTF8String);
    TestCaseAssertFalse(database.canOpen());
    TestCaseAssertFalse(database.selectValue(WCDB::Column::all().count(), WCDB::Master::tableName).succeed());
    WCDB::Handle handle = database.getHandle();
    TestCaseAssertFalse(handle.selectValue(WCDB::Column::all().count(), WCDB::Master::tableName).succeed());
    TestCaseAssertFalse(database.createTable<CPPTestCaseObject>(self.tableName.UTF8String));
    WCDB::Table<CPPTestCaseObject> table = database.getTable<CPPTestCaseObject>(self.tableName.UTF8String);
    TestCaseAssertFalse(table.selectValue(WCDB::Column::all().count()).succeed());
}

- (void)test_migration
{
    CPPTestCaseObject oldObject1 = CPPTestCaseObject(1, "a");
    CPPTestCaseObject oldObject2 = CPPTestCaseObject(2, "b");
    CPPTestCaseObject oldObject3 = CPPTestCaseObject(3, "c");

    NSData* sourceCipher = [Random.shared dataWithLength:101];
    NSData* targetCipher = [Random.shared dataWithLength:101];

    WCDB::Database sourceDatabase([self.path stringByAppendingString:@"_source"].UTF8String);
    sourceDatabase.setCipherKey(WCDB::UnsafeData((unsigned char*) sourceCipher.bytes, sourceCipher.length));

    NSString* sourceTableName = @"sourceTable";
    TestCaseAssertTrue(sourceDatabase.createTable<CPPTestCaseObject>(sourceTableName.UTF8String));
    WCDB::Table<CPPTestCaseObject> sourceTable = sourceDatabase.getTable<CPPTestCaseObject>(sourceTableName.UTF8String);

    TestCaseAssertTrue(sourceTable.insertObjects({ oldObject1, oldObject2, oldObject3 }));

    WCDB::Database targetDatabase(self.path.UTF8String);
    targetDatabase.setCipherKey(WCDB::UnsafeData((unsigned char*) targetCipher.bytes, targetCipher.length));

    targetDatabase.addMigration(sourceDatabase.getPath(),
                                WCDB::UnsafeData((unsigned char*) sourceCipher.bytes, sourceCipher.length),
                                [=](WCDB::Database::MigrationInfo& info) {
                                    if (info.table.compare(self.tableName.UTF8String) == 0) {
                                        info.sourceTable = sourceTableName.UTF8String;
                                        info.filterCondition = WCDB_FIELD(CPPTestCaseObject::identifier) > 2;
                                    }
                                });

    TestCaseAssertTrue(targetDatabase.createTable<CPPTestCaseObject>(self.tableName.UTF8String));
    WCDB::Table<CPPTestCaseObject> targetTable = targetDatabase.getTable<CPPTestCaseObject>(self.tableName.UTF8String);

    TestCaseAssertTrue(targetTable.selectValue(WCDB::Column::all().count()).value() == 1);

    TestCaseAssertTrue(targetTable.deleteObjects(WCDB_FIELD(CPPTestCaseObject::identifier) == 2));
    TestCaseAssertTrue(sourceTable.selectValue(WCDB::Column::all().count()).value() == 3);

    TestCaseAssertTrue(targetTable.updateRow({ "newContent" }, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) == 3));
    TestCaseAssertCPPStringEqual(targetTable.selectValue(WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) == 3).value().textValue().data(), "newContent");

    TestCaseAssertTrue(targetTable.deleteObjects(WCDB_FIELD(CPPTestCaseObject::identifier) == 3));
    TestCaseAssertTrue(targetTable.selectValue(WCDB::Column::all().count()).value() == 0);
    TestCaseAssertTrue(sourceTable.selectValue(WCDB::Column::all().count()).value() == 2);

    TestCaseAssertTrue(targetTable.insertObjects(CPPTestCaseObject(4, "d")));
    auto newObject = targetTable.getFirstObject(WCDB_FIELD(CPPTestCaseObject::identifier) == 4);
    TestCaseAssertTrue(newObject.hasValue() && newObject.value().content.compare("d") == 0);
    TestCaseAssertTrue(sourceTable.selectValue(WCDB::Column::all().count()).value() == 2);
    TestCaseAssertTrue(targetTable.selectValue(WCDB::Column::all().count()).value() == 1);

    targetDatabase.close();
    TestCaseAssertFalse(targetDatabase.isMigrated());

    WCDB::StringView migratedTable;
    targetDatabase.setNotificationWhenMigrated([&migratedTable, self](WCDB::Database&, WCDB::Optional<WCDB::Database::MigrationInfo> info) {
        if (info.hasValue() && info.value().table.compare(self.tableName.UTF8String) == 0) {
            migratedTable = info.value().sourceTable;
        }
    });
    while (!targetDatabase.isMigrated()) {
        TestCaseAssertTrue(targetDatabase.stepMigration());
    }
    TestCaseAssertCPPStringEqual(migratedTable.data(), sourceTableName.UTF8String);
}

@end
