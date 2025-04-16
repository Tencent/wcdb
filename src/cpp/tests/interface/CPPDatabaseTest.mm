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

@interface CPPDatabaseTests : CPPCRUDTestCase

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

- (void)test_get_error
{
    TestCaseAssertFalse(self.database->insertObject(CPPTestCaseObject(), "notExistTable"));
    TestCaseAssertFalse(self.database->getError().isOK());
    TestCaseAssertTrue(self.database->getError().getMessage().contain("notExistTable"));
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

- (void)test_backup
{
    [self insertPresetObjects];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:[self firstMaterialPath]]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:[self lastMaterialPath]]);

    TestCaseAssertTrue(self.database->backup());
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:[self firstMaterialPath]]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:[self lastMaterialPath]]);

    TestCaseAssertTrue(self.database->backup());
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:[self firstMaterialPath]]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:[self lastMaterialPath]]);
}

- (void)test_retrive
{
    [self insertPresetObjects];
    TestCaseAssertTrue(self.database->retrieve(nullptr) == 1);
    [self check:CPPMultiRowValueExtract(self.objects)
      isEqualTo:CPPMultiRowValueExtract([self getAllObjects])];
}

- (void)test_vacuum
{
    [self insertPresetObjects];
    TestCaseAssertTrue(self.database->vacuum(nullptr));
    [self check:CPPMultiRowValueExtract(self.objects)
      isEqualTo:CPPMultiRowValueExtract([self getAllObjects])];
}

- (void)test_auto_vacuum
{
    self.database->enableAutoVacuum(false);
    auto autoVacuum = self.database->getValueFromStatement(WCDB::StatementPragma().pragma(WCDB::Pragma::autoVacuum()));
    TestCaseAssertTrue(autoVacuum.succeed() && autoVacuum.value() == 1);

    self.database->enableAutoVacuum(true);
    autoVacuum = self.database->getValueFromStatement(WCDB::StatementPragma().pragma(WCDB::Pragma::autoVacuum()));
    TestCaseAssertTrue(autoVacuum.succeed() && autoVacuum.value() == 2);
}

- (void)test_incremental_vacuum
{
    self.database->enableAutoVacuum(true);
    [self insertPresetRows];
    TestCaseAssertTrue(self.database->truncateCheckpoint());
    TestCaseAssertTrue(self.database->dropTable(self.tableName.UTF8String));
    TestCaseAssertTrue(self.database->truncateCheckpoint());
    auto freelist = self.database->getValueFromStatement(WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount()));
    TestCaseAssertTrue(freelist.succeed() && freelist.value().intValue() > 0);

    TestCaseAssertTrue(self.database->incrementalVacuum(0));

    freelist = self.database->getValueFromStatement(WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount()));
    TestCaseAssertTrue(freelist.succeed() && freelist.value().intValue() == 0);
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
    TestCaseAssertTrue(newObject.valueOrDefault().content.compare("d") == 0);
    TestCaseAssertTrue(sourceTable.selectValue(WCDB::Column::all().count()).value() == 2);
    TestCaseAssertTrue(targetTable.selectValue(WCDB::Column::all().count()).value() == 1);

    targetDatabase.close();
    TestCaseAssertFalse(targetDatabase.isMigrated());

    WCDB::StringView migratedTable;
    targetDatabase.setNotificationWhenMigrated([&migratedTable, self](WCDB::Database&, WCDB::Optional<WCDB::Database::MigrationInfo> info) {
        if (info.valueOrDefault().table.compare(self.tableName.UTF8String) == 0) {
            migratedTable = info.value().sourceTable;
        }
    });
    while (!targetDatabase.isMigrated()) {
        TestCaseAssertTrue(targetDatabase.stepMigration());
    }
    TestCaseAssertCPPStringEqual(migratedTable.data(), sourceTableName.UTF8String);
}

- (void)test_normal_compress
{
    [[Random shared] setStringType:RandomStringType_English];
    TestCaseAssertTrue([self createObjectTable]);
    auto preInsertObjects = [[Random shared] testCaseObjectsWithCount:2 startingFromIdentifier:1];
    auto newInsertObjects = [[Random shared] testCaseObjectsWithCount:2 startingFromIdentifier:3];
    TestCaseAssertTrue(self.table.insertObjects(preInsertObjects));

    self.database->setCompression([](WCDB::Database::CompressionInfo& info) {
        info.addZSTDNormalCompressField(WCDB_FIELD(CPPTestCaseObject::content));
    });

    bool tableCompressed = false;
    bool databaseCompressed = false;
    self.database->setNotificationWhenCompressed([&](WCDB::Database& database, WCDB::Optional<WCDB::StringView> table) {
        TestCaseAssertEqual(database.getTag(), self.database->getTag());
        TestCaseAssertCPPStringEqual(database.getPath().data(), self.database->getPath().data());
        if (table.hasValue()) {
            if (table.value().equal(self.tableName.UTF8String)) {
                tableCompressed = true;
            }
        } else {
            databaseCompressed = true;
        }
    });

    TestCaseAssertFalse(self.database->isCompressed());
    TestCaseAssertTrue(self.database->stepCompression());
    TestCaseAssertTrue(self.database->stepCompression());
    TestCaseAssertTrue(self.database->isCompressed());
    TestCaseAssertTrue(tableCompressed && databaseCompressed);

    TestCaseAssertTrue(self.table.insertObjects(newInsertObjects));
    auto count = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column().count()).from(self.tableName.UTF8String).where(WCDB::Column("WCDB_CT_content") == 4));
    TestCaseAssertTrue(count.value() == 4);

    preInsertObjects.insert(preInsertObjects.end(), newInsertObjects.begin(), newInsertObjects.end());

    [self check:CPPMultiRowValueExtract(preInsertObjects)
      isEqualTo:CPPMultiRowValueExtract([self getAllObjects])];

    [[Random shared] setStringType:RandomStringType_Default];
}

- (void)test_dict_compress
{
    [[Random shared] setStringType:RandomStringType_English];
    TestCaseAssertTrue([self createObjectTable]);
    auto preInsertObjects = [[Random shared] testCaseObjectsWithCount:2 startingFromIdentifier:1];
    auto newInsertObjects = [[Random shared] testCaseObjectsWithCount:2 startingFromIdentifier:3];
    TestCaseAssertTrue(self.table.insertObjects(preInsertObjects));

    std::vector<std::string> samples;
    for (int i = 0; i < 1000; i++) {
        samples.push_back(std::string([[Random shared] string].UTF8String));
    }
    auto dict = WCDB::Database::trainDict(samples, 1);
    TestCaseAssertTrue(dict.succeed());
    TestCaseAssertTrue(WCDB::Database::registerZSTDDict(dict.value(), 1));

    self.database->setCompression([](WCDB::Database::CompressionInfo& info) {
        info.addZSTDDictCompressField(WCDB_FIELD(CPPTestCaseObject::content), 1);
    });

    TestCaseAssertFalse(self.database->isCompressed());
    TestCaseAssertTrue(self.database->stepCompression());
    TestCaseAssertTrue(self.database->stepCompression());
    TestCaseAssertTrue(self.database->isCompressed());

    TestCaseAssertTrue(self.table.insertObjects(newInsertObjects));
    auto count = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column().count()).from(self.tableName.UTF8String).where(WCDB::Column("WCDB_CT_content") == 2));
    TestCaseAssertTrue(count.value() == 4);

    preInsertObjects.insert(preInsertObjects.end(), newInsertObjects.begin(), newInsertObjects.end());

    [self check:CPPMultiRowValueExtract(preInsertObjects)
      isEqualTo:CPPMultiRowValueExtract([self getAllObjects])];

    [[Random shared] setStringType:RandomStringType_Default];
}

- (void)test_multi_dict_compress
{
    [[Random shared] setStringType:RandomStringType_English];
    TestCaseAssertTrue([self createObjectTable]);
    auto preInsertObjects = [[Random shared] testCaseObjectsWithCount:2 startingFromIdentifier:1];
    auto newInsertObjects = [[Random shared] testCaseObjectsWithCount:2 startingFromIdentifier:3];
    TestCaseAssertTrue(self.table.insertObjects(preInsertObjects));

    std::vector<std::string> samples;
    for (int i = 0; i < 1000; i++) {
        samples.push_back(std::string([[Random shared] string].UTF8String));
    }
    auto dict = WCDB::Database::trainDict(samples, 2);
    TestCaseAssertTrue(dict.succeed());
    TestCaseAssertTrue(WCDB::Database::registerZSTDDict(dict.value(), 2));

    samples.clear();
    for (int i = 0; i < 1000; i++) {
        samples.push_back(std::string([[Random shared] string].UTF8String));
    }
    auto dict2 = WCDB::Database::trainDict(samples, 3);
    TestCaseAssertTrue(dict2.succeed());
    TestCaseAssertTrue(WCDB::Database::registerZSTDDict(dict2.value(), 3));

    samples.clear();
    for (int i = 0; i < 1000; i++) {
        samples.push_back(std::string([[Random shared] string].UTF8String));
    }
    auto dict3 = WCDB::Database::trainDict(samples, 4);
    TestCaseAssertTrue(dict3.succeed());
    TestCaseAssertTrue(WCDB::Database::registerZSTDDict(dict3.value(), 4));

    self.database->setCompression([](WCDB::Database::CompressionInfo& info) {
        info.addZSTDDictCompressField(WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier), { { 1, 2 }, { 2, 3 }, { WCDB::Database::DictDefaultMatchValue, 4 } });
    });

    TestCaseAssertFalse(self.database->isCompressed());
    TestCaseAssertTrue(self.database->stepCompression());
    TestCaseAssertTrue(self.database->stepCompression());
    TestCaseAssertTrue(self.database->isCompressed());

    TestCaseAssertTrue(self.table.insertObjects(newInsertObjects));
    auto count = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Column().count()).from(self.tableName.UTF8String).where(WCDB::Column("WCDB_CT_content") == 2));
    TestCaseAssertTrue(count.value() == 4);

    preInsertObjects.insert(preInsertObjects.end(), newInsertObjects.begin(), newInsertObjects.end());

    [self check:CPPMultiRowValueExtract(preInsertObjects)
      isEqualTo:CPPMultiRowValueExtract([self getAllObjects])];

    [[Random shared] setStringType:RandomStringType_Default];
}

- (void)test_load_all_data
{
    TestCaseAssertTrue([self createObjectTable]);
    int objectCount = 0;
    size_t minDBSize = 20 * 1024 * 1024;
    size_t curDBSize = 0;
    int batchCount = 10000;
    auto des = WCDB_FIELD(CPPTestCaseObject::content).substr(0, 1).getDescription();
    do {
        TestCaseAssertTrue(self.table.insertObjects([Random.shared autoIncrementTestCaseObjectsWithCount:batchCount]));
        TestCaseAssertTrue(self.database->truncateCheckpoint());
        curDBSize = [self.fileManager getFileSizeIfExists:[NSString stringWithCString:self.database->getPath().data() encoding:NSUTF8StringEncoding]];
        objectCount += batchCount;
    } while (curDBSize < minDBSize);

    self.database->close();
    WCDB::Database::purgeAll();
    TestCaseAssertTrue(self.database->canOpen());

    NSDate* start = [NSDate date];
    auto contents = self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::content).substr(0, 0));
    TestCaseAssertTrue(contents.hasValue() && contents.value().size() == objectCount);
    double cost1 = [[NSDate date] timeIntervalSinceDate:start];

    self.database->close();
    WCDB::Database::purgeAll();

    self.database->setConfig("LoadAllData", [](WCDB::Handle& handle) {
        handle.tryPreloadAllPages();
        return true;
    });
    TestCaseAssertTrue(self.database->canOpen());

    start = [NSDate date];

    contents = self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::content).substr(0, 0));
    TestCaseAssertTrue(contents.hasValue() && contents.value().size() == objectCount);

    double cost2 = [[NSDate date] timeIntervalSinceDate:start];

    TestCaseAssertTrue(cost1 > cost2);
}

- (void)test_in_memory_db
{
    NSString* tableName = @"testTable";
    {
        auto database = WCDB::Database::createInMemoryDatabase();

        auto object = [[Random shared] autoIncrementTestCaseObject];

        TestCaseAssertTrue(database.createTable<CPPTestCaseObject>(tableName.UTF8String));
        for (int i = 0; i < 100; i++) {
            TestCaseAssertTrue(database.insertObject(object, tableName.UTF8String));
        }
        TestCaseAssertTrue(database.selectValue(CPPTestCaseObject::allFields().count(), tableName.UTF8String).value().intValue() == 100);
        TestCaseAssertTrue(database.passiveCheckpoint());
        database.close();
        TestCaseAssertFalse([self.fileManager fileExistsAtPath:[NSString stringWithCString:database.getPath().data() encoding:NSUTF8StringEncoding]]);
    }
    {
        auto database = WCDB::Database::createInMemoryDatabase();
        TestCaseAssertTrue(database.selectValue(CPPTestCaseObject::allFields().count(), tableName.UTF8String).failed());
    }
}

@end
