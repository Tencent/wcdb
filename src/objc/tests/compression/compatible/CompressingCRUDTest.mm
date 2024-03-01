//
// Created by qiuwenchen on 2023/12/4.
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

#import "CompressionTestCase.h"
#import "Random+CompressionTestObject.h"
#import <Foundation/Foundation.h>

@interface CompressingCRUDTest : CompressionTestCase

@end

@implementation CompressingCRUDTest

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
    self.skipFullSQLTrace = YES;
    self.expectMode = DatabaseTestCaseExpectSomeSQLs;
}

- (void)testInsert
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        NSString* schema = self.migrationStatus == MigrationStatus_noMigration ? @"" : @"main.";
        if (!self.compressTwoColumn) {
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId, WCDB_CT_text) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7)", schema]];
        } else {
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId, WCDB_CT_text, WCDB_CT_blob) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8)", schema]];
        }
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls insertObject:@"BEGIN IMMEDIATE" atIndex:0];
            [sqls insertObject:[NSString stringWithFormat:@"INSERT INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId) VALUES(?1, ?2, ?3, ?4, ?5, ?6)", self.sourceSchema] atIndex:1];
            [sqls insertObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE rowid == ?1", self.sourceSchema] atIndex:2];
            [sqls addObject:@"COMMIT"];
        }
        CompressionTestObject* obj = [[Random shared] autoIncrementCompressionObject];
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table insertObject:obj];
        }
              andSQLs:sqls];
    }];
}

- (void)testInsertOrReplace
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        NSString* schema = self.migrationStatus == MigrationStatus_noMigration ? @"" : @"main.";
        if (!self.compressTwoColumn) {
            [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId, WCDB_CT_text) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7)", schema]];
        } else {
            [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId, WCDB_CT_text, WCDB_CT_blob) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8)", schema]];
        }
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls insertObject:@"BEGIN IMMEDIATE" atIndex:0];
            [sqls insertObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId) VALUES(?1, ?2, ?3, ?4, ?5, ?6)", self.sourceSchema] atIndex:1];
            [sqls insertObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE rowid == ?1", self.sourceSchema] atIndex:2];
            [sqls addObject:@"COMMIT"];
        }
        CompressionTestObject* obj = [[Random shared] compressionObjectWithId:1];
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table insertOrReplaceObject:obj];
        }
              andSQLs:sqls];
    }];
}

- (void)testInsertOrIgnore
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        NSString* schema = self.migrationStatus == MigrationStatus_noMigration ? @"" : @"main.";
        if (!self.compressTwoColumn) {
            [sqls addObject:[NSString stringWithFormat:@"INSERT OR IGNORE INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId, WCDB_CT_text) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7)", schema]];
        } else {
            [sqls addObject:[NSString stringWithFormat:@"INSERT OR IGNORE INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId, WCDB_CT_text, WCDB_CT_blob) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8)", schema]];
        }
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls insertObject:@"BEGIN IMMEDIATE" atIndex:0];
            if (self.migrationStatus == MigrationStatus_startMigrate) {
                [sqls insertObject:[NSString stringWithFormat:@"INSERT OR IGNORE INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId) VALUES(?1, ?2, ?3, ?4, ?5, ?6)", self.sourceSchema] atIndex:1];
                [sqls insertObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE rowid == ?1", self.sourceSchema] atIndex:2];
            }
            [sqls addObject:@"COMMIT"];
        }
        CompressionTestObject* obj = [[Random shared] compressionObjectWithId:1999];
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table insertOrIgnoreObject:obj];
        }
              andSQLs:sqls];
    }];
}

- (void)testInsertWithUpsert
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        NSString* schema = self.migrationStatus == MigrationStatus_noMigration ? @"" : @"main.";
        if (!self.compressTwoColumn) {
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId) VALUES(?1, ?2, ?3, ?4, ?5, ?6) ON CONFLICT(mainId) DO UPDATE SET text = ?3, WCDB_CT_text = NULL WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%%a%%'", schema]];
            if (self.mode == CompressionMode_VariousDict) {
                [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId FROM testTable WHERE rowid == ?"];
            } else {
                [sqls addObject:@"SELECT text, WCDB_CT_text FROM testTable WHERE rowid == ?"];
            }
            [sqls addObject:@"UPDATE testTable SET text = ?2, WCDB_CT_text = ?3 WHERE rowid == ?1"];

        } else {
            [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId) VALUES(?1, ?2, ?3, ?4, ?5, ?6) ON CONFLICT(mainId) DO UPDATE SET text = ?3, WCDB_CT_text = NULL WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%%a%%'", schema]];
            if (self.mode == CompressionMode_VariousDict) {
                [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId, blob, WCDB_CT_blob, blobMatchId FROM testTable WHERE rowid == ?"];
            } else {
                [sqls addObject:@"SELECT text, WCDB_CT_text, blob, WCDB_CT_blob FROM testTable WHERE rowid == ?"];
            }
            [sqls addObject:@"UPDATE testTable SET text = ?2, WCDB_CT_text = ?3, blob = ?4, WCDB_CT_blob = ?5 WHERE rowid == ?1"];
        }
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls insertObject:@"BEGIN IMMEDIATE" atIndex:0];
            [sqls insertObject:[NSString stringWithFormat:@"INSERT INTO %@testTable(mainId, subId, text, textMatchId, blob, blobMatchId) VALUES(?1, ?2, ?3, ?4, ?5, ?6) ON CONFLICT(mainId) DO UPDATE SET text = ?3 WHERE text LIKE '%%a%%'", self.sourceSchema] atIndex:1];
            [sqls insertObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE rowid == ?1", self.sourceSchema] atIndex:2];
            [sqls addObject:@"COMMIT"];
        }

        CompressionTestObject* obj = [[Random shared] compressionObjectWithId:1999];
        __block WCDB::StatementInsert insert;
        insert.columns(CompressionTestObject.allProperties).values(WCDB::BindParameter::bindParameters(CompressionTestObject.allProperties.size()));
        WCDB::Upsert upsert = WCDB::Upsert().onConflict().indexed(CompressionTestObject.mainId).doUpdate().set(CompressionTestObject.text).to(WCDB::BindParameter(3));
        upsert.where(CompressionTestObject.text.like(@"%a%"));
        insert.upsert(upsert);
        [self
        executeModify:^BOOL(WCTTable* table) {
            insert.insertIntoTable(table.tableName);
            WCTHandle* handle = [table.database getHandle];
            TestCaseAssertTrue([handle prepare:insert]);
            [handle bindProperties:CompressionTestObject.allProperties ofObject:obj];
            TestCaseAssertTrue([handle step]);
            [handle finalizeStatement];
            [handle invalidate];
            return true;
        }
              andSQLs:sqls];
    }];
}

- (void)testInsertWithSelect
{
    [self doTestCompression:^{
        if (self.migrationStatus != MigrationStatus_noMigration) {
            return;
        }
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.compressTwoColumn) {
            [sqls addObject:@"INSERT OR REPLACE INTO testTable(mainId, subId, text, textMatchId, blob, blobMatchId) SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, wcdb_decompress(blob, WCDB_CT_blob) AS blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 1"];
            if (self.mode == CompressionMode_VariousDict) {
                [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId, blob, WCDB_CT_blob, blobMatchId FROM testTable WHERE rowid == ?"];
            } else {
                [sqls addObject:@"SELECT text, WCDB_CT_text, blob, WCDB_CT_blob FROM testTable WHERE rowid == ?"];
            }
            [sqls addObject:@"UPDATE testTable SET text = ?2, WCDB_CT_text = ?3, blob = ?4, WCDB_CT_blob = ?5 WHERE rowid == ?1"];
        } else {
            [sqls addObject:@"INSERT OR REPLACE INTO testTable(mainId, subId, text, textMatchId, blob, blobMatchId) SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 1"];
            if (self.mode == CompressionMode_VariousDict) {
                [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId FROM testTable WHERE rowid == ?"];
            } else {
                [sqls addObject:@"SELECT text, WCDB_CT_text FROM testTable WHERE rowid == ?"];
            }
            [sqls addObject:@"UPDATE testTable SET text = ?2, WCDB_CT_text = ?3 WHERE rowid == ?1"];
        }

        WCDB::StatementSelect select;
        select.select(CompressionTestObject.allProperties).from(self.tableName).where(CompressionTestObject.text.like(@"%a%")).order(CompressionTestObject.mainId.asOrder(WCDB::Order::DESC)).limit(1);
        __block WCDB::StatementInsert insert;
        insert.orReplace().columns(CompressionTestObject.allProperties).values(select);
        [self
        executeModify:^BOOL(WCTTable* table) {
            insert.insertIntoTable(table.tableName);
            return [table.database execute:insert];
        }
              andSQLs:sqls];
    }];
}

- (void)testUpdate
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE mainId == 1"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@testTable SET subId = ?1 WHERE mainId == ?2", self.sourceSchema]];
            [sqls addObject:@"UPDATE main.testTable SET subId = ?1 WHERE mainId == ?2"];
            [sqls addObject:@"COMMIT"];
        } else {
            [sqls addObject:@"UPDATE testTable SET subId = ?1 WHERE mainId == 1"];
        }
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table updateProperty:CompressionTestObject.subId toValue:@1 where:CompressionTestObject.mainId == 1];
        }
              andSQLs:sqls];
    }];
}

- (void)testUpdateCompressingColumn
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE text LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@testTable SET text = ?1, blob = ?2 WHERE mainId == ?3", self.sourceSchema]];
            if (self.compressTwoColumn) {
                if (self.mode == CompressionMode_VariousDict) {
                    [sqls addObject:@"SELECT rowid, textMatchId, blobMatchId FROM testTable WHERE mainId == ?3"];
                    if (self.migrationStatus == MigrationStatus_startMigrate) {
                        [sqls addObject:@"UPDATE main.testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?4, WCDB_CT_blob = ?5 WHERE rowid == ?6"];
                    }
                } else {
                    [sqls addObject:@"UPDATE main.testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?4, WCDB_CT_blob = ?5 WHERE mainId == ?3"];
                }
            } else {
                if (self.mode == CompressionMode_VariousDict) {
                    [sqls addObject:@"SELECT rowid, textMatchId FROM testTable WHERE mainId == ?3"];
                    if (self.migrationStatus == MigrationStatus_startMigrate) {
                        [sqls addObject:@"UPDATE main.testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?4 WHERE rowid == ?5"];
                    }
                } else {
                    [sqls addObject:@"UPDATE main.testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?4 WHERE mainId == ?3"];
                }
            }
            [sqls addObject:@"COMMIT"];
        } else {
            if (self.compressTwoColumn) {
                if (self.mode == CompressionMode_VariousDict) {
                    [sqls addObject:@"SELECT rowid, textMatchId, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
                    [sqls addObject:@"UPDATE testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?3, WCDB_CT_blob = ?4 WHERE rowid == ?5"];
                } else {
                    [sqls addObject:@"UPDATE testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?3, WCDB_CT_blob = ?4 WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
                }
            } else {
                if (self.mode == CompressionMode_VariousDict) {
                    [sqls addObject:@"SELECT rowid, textMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
                    [sqls addObject:@"UPDATE testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?3 WHERE rowid == ?4"];
                } else {
                    [sqls addObject:@"UPDATE testTable SET text = ?1, blob = ?2, WCDB_CT_text = ?3 WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
                }
            }
        }
        CompressionTestObject* obj = [Random.shared autoIncrementCompressionObject];
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table updateProperties:{ CompressionTestObject.text, CompressionTestObject.blob } toObject:obj where:CompressionTestObject.text.like(@"%a%") orders:CompressionTestObject.mainId.asOrder(WCTOrderedDescending) limit:20 offset:5];
        }
              andSQLs:sqls];
    }];
}

- (void)testUpdateCompressingColumnWithMatchType
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE text LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@testTable SET text = ?1, blob = ?2, textMatchId = ?3, blobMatchId = ?4 WHERE mainId == ?5", self.sourceSchema]];
            if (self.compressTwoColumn) {
                [sqls addObject:@"UPDATE main.testTable SET text = ?1, blob = ?2, textMatchId = ?3, blobMatchId = ?4, WCDB_CT_text = ?6, WCDB_CT_blob = ?7 WHERE mainId == ?5"];
            } else {
                [sqls addObject:@"UPDATE main.testTable SET text = ?1, blob = ?2, textMatchId = ?3, blobMatchId = ?4, WCDB_CT_text = ?6 WHERE mainId == ?5"];
            }
            [sqls addObject:@"COMMIT"];
        } else {
            if (self.compressTwoColumn) {
                [sqls addObject:@"UPDATE testTable SET text = ?1, blob = ?2, textMatchId = ?3, blobMatchId = ?4, WCDB_CT_text = ?5, WCDB_CT_blob = ?6 WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            } else {
                [sqls addObject:@"UPDATE testTable SET text = ?1, blob = ?2, textMatchId = ?3, blobMatchId = ?4, WCDB_CT_text = ?5 WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%a%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            }
        }
        CompressionTestObject* obj = [Random.shared autoIncrementCompressionObject];
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table updateProperties:{ CompressionTestObject.text,
                                             CompressionTestObject.blob,
                                             CompressionTestObject.textMatchId,
                                             CompressionTestObject.blobMatchId }
                                  toObject:obj
                                     where:CompressionTestObject.text.like(@"%a%")
                                    orders:CompressionTestObject.mainId.asOrder(WCTOrderedDescending)
                                     limit:20
                                    offset:5];
        }
              andSQLs:sqls];
    }];
}

- (void)testUpdateCompressingColumnWithValue
{
    __block WCDB::StatementUpdate update;
    update.set(CompressionTestObject.text).to("abc").where(CompressionTestObject.text.like("%without%")).order(CompressionTestObject.mainId.asOrder(WCDB::Order::DESC)).limit(20).offset(5);
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE text LIKE '%without%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@testTable SET text = 'abc' WHERE mainId == ?1", self.sourceSchema]];
            [sqls addObject:@"SELECT rowid FROM testTable WHERE mainId == ?1"];
            if (self.migrationStatus == MigrationStatus_startMigrate) {
                [sqls addObject:@"UPDATE main.testTable SET text = 'abc', WCDB_CT_text = NULL WHERE rowid == ?2"];
                if (self.mode == CompressionMode_VariousDict) {
                    [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId FROM testTable WHERE rowid == ?"];
                } else {
                    [sqls addObject:@"SELECT text, WCDB_CT_text FROM testTable WHERE rowid == ?"];
                }
            }
            [sqls addObject:@"COMMIT"];
        } else {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT rowid FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%without%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            [sqls addObject:@"UPDATE testTable SET text = 'abc', WCDB_CT_text = NULL WHERE rowid == ?1"];
            if (self.mode == CompressionMode_VariousDict) {
                [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId FROM testTable WHERE rowid == ?"];
            } else {
                [sqls addObject:@"SELECT text, WCDB_CT_text FROM testTable WHERE rowid == ?"];
            }
            [sqls addObject:@"COMMIT"];
        }

        [self
        executeModify:^BOOL(WCTTable* table) {
            update.update(table.tableName);
            return [table.database execute:update];
        }
              andSQLs:sqls];
    }];
}

- (void)testUpdateCompressingColumnWithMultiValue
{
    __block WCDB::StatementUpdate update;
    update.set({ CompressionTestObject.text, CompressionTestObject.textMatchId }).to(WCDB::Expression({ "abc", 1 })).where(CompressionTestObject.text.like("%without%")).order(CompressionTestObject.mainId.asOrder(WCDB::Order::DESC)).limit(20).offset(5);
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE text LIKE '%without%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            [sqls addObject:[NSString stringWithFormat:@"UPDATE %@testTable SET (text, textMatchId) = ('abc', 1) WHERE mainId == ?1", self.sourceSchema]];
            [sqls addObject:@"SELECT rowid FROM testTable WHERE mainId == ?1"];
            if (self.migrationStatus == MigrationStatus_startMigrate) {
                [sqls addObject:@"UPDATE main.testTable SET (text, textMatchId) = ('abc', 1), WCDB_CT_text = NULL WHERE rowid == ?2"];
                if (self.mode == CompressionMode_VariousDict) {
                    [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId FROM testTable WHERE rowid == ?"];
                } else {
                    [sqls addObject:@"SELECT text, WCDB_CT_text FROM testTable WHERE rowid == ?"];
                }
            }
            [sqls addObject:@"COMMIT"];
        } else {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT rowid FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%without%' ORDER BY mainId DESC LIMIT 20 OFFSET 5"];
            [sqls addObject:@"UPDATE testTable SET (text, textMatchId) = ('abc', 1), WCDB_CT_text = NULL WHERE rowid == ?1"];
            if (self.mode == CompressionMode_VariousDict) {
                [sqls addObject:@"SELECT text, WCDB_CT_text, textMatchId FROM testTable WHERE rowid == ?"];
            } else {
                [sqls addObject:@"SELECT text, WCDB_CT_text FROM testTable WHERE rowid == ?"];
            }
            [sqls addObject:@"COMMIT"];
        }

        [self
        executeModify:^BOOL(WCTTable* table) {
            update.update(table.tableName);
            return [table.database execute:update];
        }
              andSQLs:sqls];
    }];
}

- (void)testDelete
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE textMatchId == 2 ORDER BY subId ASC LIMIT 10 OFFSET 10"];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE mainId == ?1", self.sourceSchema]];
            [sqls addObject:@"DELETE FROM main.testTable WHERE mainId == ?1"];
            [sqls addObject:@"COMMIT"];
        } else {
            [sqls addObject:@"DELETE FROM testTable WHERE textMatchId == 2 ORDER BY subId ASC LIMIT 10 OFFSET 10"];
        }

        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table deleteObjectsWhere:CompressionTestObject.textMatchId == 2 orders:CompressionTestObject.subId.asOrder(WCDB::Order::ASC) limit:10 offset:10];
        }
              andSQLs:sqls];
    }];
}

- (void)testDeleteWithCompressingColumn
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"SELECT mainId FROM temp.wcdb_union_testTable WHERE text LIKE '%because%' ORDER BY subId ASC LIMIT 10 OFFSET 10"];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE mainId == ?1", self.sourceSchema]];
            [sqls addObject:@"DELETE FROM main.testTable WHERE mainId == ?1"];
            [sqls addObject:@"COMMIT"];
        } else {
            [sqls addObject:@"DELETE FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%because%' ORDER BY subId ASC LIMIT 10 OFFSET 10"];
        }
        [self
        executeModify:^BOOL(WCTTable* table) {
            return [table deleteObjectsWhere:CompressionTestObject.text.like("%because%") orders:CompressionTestObject.subId.asOrder(WCDB::Order::ASC) limit:10 offset:10];
        }
              andSQLs:sqls];
    }];
}

- (void)testDeleteWithCommonTableExpression
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"BEGIN IMMEDIATE"];
            [sqls addObject:@"WITH testAliasTable(text) AS(SELECT text FROM temp.wcdb_union_testTable WHERE text LIKE '%without%' ORDER BY mainId LIMIT 100) SELECT mainId FROM temp.wcdb_union_testTable WHERE EXISTS(SELECT 1 FROM testAliasTable WHERE temp.wcdb_union_testTable.text == testAliasTable.text)"];
            [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@testTable WHERE mainId == ?1", self.sourceSchema]];
            [sqls addObject:@"DELETE FROM main.testTable WHERE mainId == ?1"];
            [sqls addObject:@"COMMIT"];
        } else {
            [sqls addObject:@"WITH testAliasTable(text) AS(SELECT wcdb_decompress(text, WCDB_CT_text) AS text FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%without%' ORDER BY mainId LIMIT 100) DELETE FROM testTable WHERE EXISTS(SELECT 1 FROM testAliasTable WHERE wcdb_decompress(testTable.text, testTable.WCDB_CT_text) == testAliasTable.text)"];
        }
        [self
        executeModify:^BOOL(WCTTable* table) {
            WCDB::CommonTableExpression cte("testAliasTable");
            cte.column(CompressionTestObject.text);
            cte.as(WCDB::StatementSelect()
                   .select(CompressionTestObject.text)
                   .from(table.tableName)
                   .where(CompressionTestObject.text.like("%without%"))
                   .order(CompressionTestObject.mainId.asOrder())
                   .limit(100));
            WCDB::StatementSelect innerSelect = WCDB::StatementSelect().select(1).from("testAliasTable").where(CompressionTestObject.text.table(table.tableName) == CompressionTestObject.text.table("testAliasTable"));
            WCTDelete* delete_ = [[table prepareDelete] where:WCDB::Expression::exists(innerSelect)];
            delete_.statement.with(cte);
            return [delete_ execute];
        }
              andSQLs:sqls];
    }];
}

- (void)testSelect
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM temp.wcdb_union_testTable WHERE text LIKE '%two%' ORDER BY text ASC LIMIT 100 OFFSET 100"];
        } else {
            if (self.compressTwoColumn) {
                [sqls addObject:@"SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, wcdb_decompress(blob, WCDB_CT_blob) AS blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%two%' ORDER BY wcdb_decompress(text, WCDB_CT_text) ASC LIMIT 100 OFFSET 100"];
            } else {
                [sqls addObject:@"SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%two%' ORDER BY wcdb_decompress(text, WCDB_CT_text) ASC LIMIT 100 OFFSET 100"];
            }
        }
        [self
        executeSelect:^NSArray*(WCTTable* table) {
            return [table getObjectsWhere:CompressionTestObject.text.like("%two%") orders:CompressionTestObject.text.asOrder(WCDB::Order::ASC) limit:100 offset:100];
        }
              andSQLs:sqls];
    }];
}

- (void)testSelectWithCommonTableExpression
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"WITH testAliasTable(mainId, subId, text, textMatchId, blob, blobMatchId) AS(SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM temp.wcdb_union_testTable WHERE text LIKE '%two%' LIMIT 100) SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM testAliasTable ORDER BY subId"];
        } else {
            if (self.compressTwoColumn) {
                [sqls addObject:@"WITH testAliasTable(mainId, subId, text, textMatchId, blob, blobMatchId) AS(SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, wcdb_decompress(blob, WCDB_CT_blob) AS blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%two%' LIMIT 100) SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM testAliasTable ORDER BY subId"];
            } else {
                [sqls addObject:@"WITH testAliasTable(mainId, subId, text, textMatchId, blob, blobMatchId) AS(SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) LIKE '%two%' LIMIT 100) SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM testAliasTable ORDER BY subId"];
            }
        }
        [self
        executeSelect:^NSArray*(WCTTable* table) {
            WCTSelect* select = [[[table prepareSelect] fromTable:@"testAliasTable"] orders:CompressionTestObject.subId.asOrder()];
            WCDB::CommonTableExpression cte("testAliasTable");
            cte.column(CompressionTestObject.mainId);
            cte.column(CompressionTestObject.subId);
            cte.column(CompressionTestObject.text);
            cte.column(CompressionTestObject.textMatchId);
            cte.column(CompressionTestObject.blob);
            cte.column(CompressionTestObject.blobMatchId);
            cte.as(WCDB::StatementSelect()
                   .select(CompressionTestObject.allProperties)
                   .from(table.tableName)
                   .where(CompressionTestObject.text.like("%two%"))
                   .limit(100));
            select.statement.with(cte);

            return select.allObjects;
        }
              andSQLs:sqls];
    }];
}

- (void)testSelectFromSubquery
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM (SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM temp.wcdb_union_testTable WHERE (text LIKE '%two%') AND (blobMatchId == 2)) AS subquery ORDER BY subId"];
        } else {
            if (self.compressTwoColumn) {
                [sqls addObject:@"SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM (SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, wcdb_decompress(blob, WCDB_CT_blob) AS blob, blobMatchId FROM testTable WHERE (wcdb_decompress(text, WCDB_CT_text) LIKE '%two%') AND (blobMatchId == 2)) AS subquery ORDER BY subId"];
            } else {
                [sqls addObject:@"SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM (SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, blob, blobMatchId FROM testTable WHERE (wcdb_decompress(text, WCDB_CT_text) LIKE '%two%') AND (blobMatchId == 2)) AS subquery ORDER BY subId"];
            }
        }
        [self
        executeSelect:^NSArray*(WCTTable* table) {
            WCTSelect* select = [[[table prepareSelect] fromTable:@"subquery"] orders:CompressionTestObject.subId.asOrder()];
            WCDB::StatementSelect subquery;
            subquery.select(CompressionTestObject.allProperties).from(table.tableName).where(CompressionTestObject.text.like("%two%") && CompressionTestObject.blobMatchId == 2);
            select.statement.from(subquery.as(@"subquery"));
            return select.allObjects;
        }
              andSQLs:sqls];
    }];
}

- (void)testSelectWithSubquery
{
    [self doTestCompression:^{
        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"SELECT mainId, subId, text, textMatchId, blob, blobMatchId FROM temp.wcdb_union_testTable WHERE text IN(SELECT text FROM temp.wcdb_union_testTable WHERE (text LIKE '%two%') AND (blobMatchId == 2)) ORDER BY subId"];
        } else {
            if (self.compressTwoColumn) {
                [sqls addObject:@"SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, wcdb_decompress(blob, WCDB_CT_blob) AS blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) IN(SELECT wcdb_decompress(text, WCDB_CT_text) AS text FROM testTable WHERE (wcdb_decompress(text, WCDB_CT_text) LIKE '%two%') AND (blobMatchId == 2)) ORDER BY subId"];
            } else {
                [sqls addObject:@"SELECT mainId, subId, wcdb_decompress(text, WCDB_CT_text) AS text, textMatchId, blob, blobMatchId FROM testTable WHERE wcdb_decompress(text, WCDB_CT_text) IN(SELECT wcdb_decompress(text, WCDB_CT_text) AS text FROM testTable WHERE (wcdb_decompress(text, WCDB_CT_text) LIKE '%two%') AND (blobMatchId == 2)) ORDER BY subId"];
            }
        }
        [self
        executeSelect:^NSArray*(WCTTable* table) {
            WCTSelect* select = [[table prepareSelect] orders:CompressionTestObject.subId.asOrder()];
            WCDB::StatementSelect subquery;
            subquery.select(CompressionTestObject.text).from(table.tableName).where(CompressionTestObject.text.like("%two%") && CompressionTestObject.blobMatchId == 2);
            select.statement.where(CompressionTestObject.text.in(subquery));
            return select.allObjects;
        }
              andSQLs:sqls];
    }];
}

- (void)testSelectWithMultiTables
{
    [self doTestCompression:^{
        NSString* newTable = @"newTable";
        TestCaseAssertTrue([self.database createTable:newTable withClass:CompressionTestObject.class]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementInsert().insertIntoTable(newTable).columns(CompressionTestObject.allProperties).values(WCDB::StatementSelect().select(CompressionTestObject.allProperties).from(self.uncompressTableName))]);

        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.migrationStatus != MigrationStatus_noMigration) {
            [sqls addObject:@"SELECT newTable.text, temp.wcdb_union_testTable.blob FROM temp.wcdb_union_testTable, newTable WHERE (newTable.text == temp.wcdb_union_testTable.text) AND (newTable.blobMatchId == 2) ORDER BY temp.wcdb_union_testTable.text"];
        } else {
            if (self.compressTwoColumn) {
                [sqls addObject:@"SELECT newTable.text, wcdb_decompress(testTable.blob, testTable.WCDB_CT_blob) AS blob FROM testTable, newTable WHERE (newTable.text == wcdb_decompress(testTable.text, testTable.WCDB_CT_text)) AND (newTable.blobMatchId == 2) ORDER BY wcdb_decompress(testTable.text, testTable.WCDB_CT_text)"];
            } else {
                [sqls addObject:@"SELECT newTable.text, testTable.blob FROM testTable, newTable WHERE (newTable.text == wcdb_decompress(testTable.text, testTable.WCDB_CT_text)) AND (newTable.blobMatchId == 2) ORDER BY wcdb_decompress(testTable.text, testTable.WCDB_CT_text)"];
            }
        }
        [self
        executeSelect:^NSArray*(WCTTable* table) {
            WCTSelect* select = [[table prepareSelect] orders:CompressionTestObject.text.table(table.tableName).asOrder()];
            [select onResultColumns:{ CompressionTestObject.text.table(newTable), CompressionTestObject.blob.table(table.tableName) }];
            select.statement.from({ table.tableName, newTable }).where(CompressionTestObject.text.table(newTable) == CompressionTestObject.text.table(table.tableName) && CompressionTestObject.blobMatchId.table(newTable) == 2);
            return select.allObjects;
        }
              andSQLs:sqls];
    }];
}

- (void)testSelectWithJoin
{
    [self doTestCompression:^{
        if (self.migrationStatus != MigrationStatus_noMigration) {
            return;
        }
        NSString* newTable = @"newTable";
        TestCaseAssertTrue([self.database createTable:newTable withClass:CompressionTestObject.class]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementInsert().insertIntoTable(newTable).columns(CompressionTestObject.allProperties).values(WCDB::StatementSelect().select(CompressionTestObject.allProperties).from(self.uncompressTableName))]);

        NSMutableArray* sqls = [[NSMutableArray alloc] init];
        if (self.compressTwoColumn) {
            [sqls addObject:@"SELECT testTable.text, newTable.blob FROM (SELECT wcdb_decompress(text, WCDB_CT_text) AS text, wcdb_decompress(blob, WCDB_CT_blob) AS blob FROM testTable WHERE textMatchId == 1) AS testTable JOIN (SELECT text, blob FROM newTable WHERE blobMatchId == 1) AS newTable ON testTable.text == newTable.text ORDER BY testTable.blob"];
        } else {
            [sqls addObject:@"SELECT testTable.text, newTable.blob FROM (SELECT wcdb_decompress(text, WCDB_CT_text) AS text, blob FROM testTable WHERE textMatchId == 1) AS testTable JOIN (SELECT text, blob FROM newTable WHERE blobMatchId == 1) AS newTable ON testTable.text == newTable.text ORDER BY testTable.blob"];
        }
        [self
        executeSelect:^NSArray*(WCTTable* table) {
            WCTSelect* select = [[[table prepareSelect] onResultColumns:{ CompressionTestObject.text.table(table.tableName), CompressionTestObject.blob.table(newTable) }] orders:CompressionTestObject.blob.table(table.tableName)];
            auto select1 = WCDB::StatementSelect().select({ CompressionTestObject.text, CompressionTestObject.blob }).from(table.tableName).where(CompressionTestObject.textMatchId == 1);
            auto select2 = WCDB::StatementSelect().select({ CompressionTestObject.text, CompressionTestObject.blob }).from(newTable).where(CompressionTestObject.blobMatchId == 1);
            WCDB::Join join;
            join.table(select1.as(table.tableName)).join(select2.as(newTable)).on(CompressionTestObject.text.table(table.tableName) == CompressionTestObject.text.table(newTable));
            select.statement.from(join);
            return select.allObjects;
        }
              andSQLs:sqls];
    }];
}

@end
