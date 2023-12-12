//
// Created by qiuwenchen on 2023/12/7.
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

#import "CompressionRecord.hpp"
#import "CompressionTestCase.h"
#import "CoreConst.h"
#import "Random+CompressionTestObject.h"
#import <Foundation/Foundation.h>
#import <zstd/zstd.h>

@interface CompressionTests : CompressionTestCase {
    NSString* m_recordTable;
}

@end

@implementation CompressionTests

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
    self.skipFullSQLTrace = YES;
    self.expectMode = DatabaseTestCaseExpectSomeSQLs;
    m_recordTable = @"wcdb_builtin_compression_record";
}

- (void)doTestCompress:(void (^)(void))operation
{
    for (int i = 0; i < 6; i++) {
        self.anotherTable = nil;
        [self clearData];
        self.compressTwoColumn = (bool) (i % 2);
        self.mode = (CompressionMode) (i / 2);
        [self configCompression];
        [self log:@"excute compress test %d: mode %d, two column %d, compression status %d",
                  i,
                  self.mode,
                  self.compressTwoColumn,
                  self.compressionStatus];
        operation();
    }
}

- (void)testDropTable
{
    [self clearData];
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self configCompression];
    WCTOneRow* record = [self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Column::all()).from(m_recordTable)];
    TestCaseAssertTrue(record.count == 3);
    TestCaseAssertTrue([record[0].stringValue isEqualToString:self.tableName]);
    TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text"]);
    TestCaseAssertTrue(record[2].numberValue.intValue == 0);
    [self doTestSQLs:@[
        @"DROP TABLE IF EXISTS testTable",
        @"DELETE FROM wcdb_builtin_compression_record WHERE tableName == 'testTable'"
    ]
         inOperation:^BOOL {
             return [self.database dropTable:self.table.tableName];
         }];
    WCTValue* count = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column::all().count()).from(m_recordTable)];
    TestCaseAssertTrue(count.numberValue.intValue == 0);
}

- (void)testAlterTable
{
    [self clearData];
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self configCompression];
    WCTOneRow* record = [self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Column::all()).from(m_recordTable)];
    TestCaseAssertTrue(record.count == 3);
    TestCaseAssertTrue([record[0].stringValue isEqualToString:self.tableName]);
    TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text"]);
    TestCaseAssertTrue(record[2].numberValue.intValue == 0);
    [self doTestSQLs:@[
        @"ALTER TABLE testTable RENAME TO newTable",
        @"UPDATE wcdb_builtin_compression_record SET tableName = 'newTable' WHERE tableName == 'testTable'"
    ]
         inOperation:^BOOL {
             return [self.database execute:WCDB::StatementAlterTable().alterTable(self.tableName).renameToTable(@"newTable")];
         }];
    record = [self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Column::all()).from(m_recordTable)];
    TestCaseAssertTrue(record.count == 3);
    TestCaseAssertTrue([record[0].stringValue isEqualToString:@"newTable"]);
    TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text"]);
    TestCaseAssertTrue(record[2].numberValue.intValue == 0);
}

- (void)testCompressionType
{
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self doTestCompress:^{
        WCTOneColumn* textCompressedTypes = [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_text")).distinct().from(self.tableName).order(WCDB::Column("WCDB_CT_text").asOrder(WCDB::Order::ASC))];
        TestCaseAssertTrue([textCompressedTypes count] == 2);
        TestCaseAssertTrue(textCompressedTypes[0].numberValue.intValue == 0);
        if (self.mode != CompressionMode_Normal) {
            TestCaseAssertTrue(textCompressedTypes[1].numberValue.intValue == 2);
        } else {
            TestCaseAssertTrue(textCompressedTypes[1].numberValue.intValue == 4);
        }
        WCTValue* uncompressedTextCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_text").count()).from(self.tableName).where(WCDB::Column("WCDB_CT_text").isNull())];
        TestCaseAssertTrue(uncompressedTextCount.numberValue.intValue == 0);
        if (!self.compressTwoColumn) {
            return;
        }
        WCTOneColumn* blobCompressedTypes = [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_blob")).distinct().from(self.tableName).order(WCDB::Column("WCDB_CT_blob").asOrder(WCDB::Order::ASC))];
        TestCaseAssertTrue([blobCompressedTypes count] == 2);
        TestCaseAssertTrue(blobCompressedTypes[0].numberValue.intValue == 1);
        if (self.mode != CompressionMode_Normal) {
            TestCaseAssertTrue(blobCompressedTypes[1].numberValue.intValue == 3);
        } else {
            TestCaseAssertTrue(blobCompressedTypes[1].numberValue.intValue == 5);
        }
        WCTValue* uncompressedBlobCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_blob").count()).from(self.tableName).where(WCDB::Column("WCDB_CT_blob").isNull())];
        TestCaseAssertTrue(uncompressedBlobCount.numberValue.intValue == 0);
    }];
}

- (void)testCompressDict
{
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self doTestCompress:^{
        [self.database setCompressionWithFilter:nil];
        WCDB::ResultColumns textColumns = { CompressionTestObject.text, WCDB::Column("WCDB_CT_text") };
        if (self.mode == CompressionMode_VariousDict) {
            textColumns.push_back(CompressionTestObject.textMatchId);
        }
        WCTColumnsXRows* allTexts = [self.database getRowsFromStatement:WCDB::StatementSelect().select(textColumns).from(self.tableName)];
        TestCaseAssertTrue(allTexts.count == self.originObjects.count);
        for (WCTOneRow* row in allTexts) {
            TestCaseAssertTrue(row.count >= 2);
            TestCaseAssertTrue(row[1] != [NSNull null]);
            if (row[1].numberValue.intValue == 0) {
                continue;
            }
            TestCaseAssertTrue([row[0] isKindOfClass:NSData.class]);
            int dictId = ZSTD_getDictID_fromFrame(row[0].dataValue.bytes, row[0].dataValue.length);
            if (self.mode == CompressionMode_Normal) {
                TestCaseAssertTrue(dictId == 0);
            } else if (self.mode == CompressionMode_Dict) {
                TestCaseAssertTrue(dictId == 1);
            } else {
                TestCaseAssertTrue(row.count == 3);
                NSNumber* expectedDictId = [self.matchDictForText objectForKey:row[2].numberValue];
                if (expectedDictId == nil) {
                    expectedDictId = [self.matchDictForText objectForKey:@WCTDictDefaultMatchValue];
                }
                TestCaseAssertTrue(expectedDictId.intValue == dictId);
            }
        }

        if (!self.compressTwoColumn) {
            return;
        }
        WCDB::ResultColumns blobColumns = { CompressionTestObject.blob, WCDB::Column("WCDB_CT_blob") };
        if (self.mode == CompressionMode_VariousDict) {
            blobColumns.push_back(CompressionTestObject.blobMatchId);
        }
        WCTColumnsXRows* allBlobs = [self.database getRowsFromStatement:WCDB::StatementSelect().select(blobColumns).from(self.tableName)];
        TestCaseAssertTrue(allBlobs.count == self.originObjects.count);
        for (WCTOneRow* row in allBlobs) {
            TestCaseAssertTrue(row.count >= 2);
            TestCaseAssertTrue(row[1] != [NSNull null]);
            if (row[1].numberValue.intValue == 1) {
                continue;
            }
            TestCaseAssertTrue([row[0] isKindOfClass:NSData.class]);
            int dictId = ZSTD_getDictID_fromFrame(row[0].dataValue.bytes, row[0].dataValue.length);
            if (self.mode == CompressionMode_Normal) {
                TestCaseAssertTrue(dictId == 0);
            } else if (self.mode == CompressionMode_Dict) {
                TestCaseAssertTrue(dictId == 1);
            } else {
                TestCaseAssertTrue(row.count == 3);
                NSNumber* expectedDictId = [self.matchDictForBLOB objectForKey:row[2].numberValue];
                if (expectedDictId == nil) {
                    expectedDictId = [self.matchDictForBLOB objectForKey:@WCTDictDefaultMatchValue];
                }
                TestCaseAssertTrue(expectedDictId.intValue == dictId);
            }
        }
    }];
}

- (void)testCompress
{
    self.compressionStatus = CompressionStatus_uncompressed;
    [self doTestCompress:^{
        TestCaseAssertFalse([self.database isCompressed]);
        __block BOOL tableCompressed;
        __block int databaseCompressed;
        NSString* expectedTableName = self.tableName;
        WCTTag tag = self.database.tag;
        [self.database setNotificationWhenCompressd:^(WCTDatabase* database, WCTCompressionBaseInfo* info) {
            TestCaseAssertEqual(tag, database.tag);
            if (info == nil) {
                ++databaseCompressed;
                TestCaseAssertTrue(database.isCompressed);
            } else if ([info.table isEqualToString:expectedTableName]) {
                TestCaseAssertFalse(tableCompressed);
                tableCompressed = YES;
            }
            BOOL succeed;
            do {
                succeed = [self.database stepCompression];
            } while (succeed && ![self.database isCompressed]);
            TestCaseAssertTrue(succeed);
            TestCaseAssertTrue(self.database.isCompressed);

            TestCaseAssertTrue([[self.table getObjects] isEqualToArray:[self.uncompressTable getObjects]]);

            // Compression record
            WCTOneRow* record = [self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Column::all()).from(self->m_recordTable)];
            TestCaseAssertTrue(record.count == 3);
            TestCaseAssertTrue([record[0].stringValue isEqualToString:self.tableName]);
            if (self.compressTwoColumn) {
                TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text blob"]);
            } else {
                TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text"]);
            }
            TestCaseAssertTrue(record[2].numberValue.intValue == 0);

            TestCaseAssertTrue(tableCompressed);
            TestCaseAssertEqual(databaseCompressed, 1);

            // Notification will receive multiple times.
            succeed = [self.database stepCompression];
            TestCaseAssertTrue(succeed);
            TestCaseAssertTrue([self.database isCompressed]);
            TestCaseAssertEqual(databaseCompressed, 2);
        }];
    }];
}

- (void)testAutoCompress
{
    [self doTestCompress:^{
        TestCaseResult* tableCompressed = [TestCaseResult no];
        TestCaseResult* compressed = [TestCaseResult no];
        NSString* expectedTableName = self.tableName;
        [self.database setNotificationWhenCompressd:^(WCTDatabase*, WCTCompressionBaseInfo* tableInfo) {
            if (tableInfo == nil) {
                [compressed makeYES];
            } else if ([tableInfo.table isEqualToString:expectedTableName]) {
                [tableCompressed makeYES];
            }
        }];
        [self.database enableAutoCompression:YES];
        TestCaseAssertTrue([self.database canOpen]);

        while (!tableCompressed.value) {
            [NSThread sleepForTimeInterval:self.delayForTolerance];
        }
        while (!compressed.value) {
            [NSThread sleepForTimeInterval:self.delayForTolerance];
        }
        TestCaseAssertResultYES(tableCompressed);
        TestCaseAssertResultYES(compressed);
        TestCaseAssertTrue(self.database.isCompressed);
    }];
}

- (void)test_feature_closed_database_will_not_perform_auto_compress
{
    [self doTestCompress:^{
        [self.database enableAutoCompression:YES];

        TestCaseAssertTrue([self.database canOpen]);

        [self.database close];

        [NSThread sleepForTimeInterval:self.delayForTolerance + WCDB::OperationQueueTimeIntervalForCompression];

        TestCaseAssertFalse(self.database.isOpened);
    }];
}

- (void)test_feature_auto_compress_will_stop_due_to_error
{
    [self.database enableAutoCheckpoint:NO];
    [self doTestCompress:^{
        TestCaseAssertTrue([self.database canOpen]);

        TestCaseCounter* numberOfFailures = [TestCaseCounter value:0];
        weakify(self);
        [self.database traceError:nil];
        [self.database traceError:^(WCTError* error) {
            strongify_or_return(self);
            if (error.code == WCTErrorCodeIOError
                && error.level == WCTErrorLevelWarning) {
                [numberOfFailures increment];
            }
        }];

        [self.database enableAutoCompression:YES];
        TestCaseAssertTrue([self.database canOpen]);
        [WCTDatabase simulateIOError:WCTSimulateWriteIOError];

        // wait until auto migrate stopped
        while (numberOfFailures.value < WCDB::OperationQueueTolerableFailuresForCompression)
            ;

        // wait to confirm migration is stopped.
        [NSThread sleepForTimeInterval:2 * WCDB::OperationQueueTolerableFailuresForCompression];

        TestCaseAssertTrue(numberOfFailures.value == WCDB::OperationQueueTolerableFailuresForCompression);

        [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
    }];
    [self.database enableAutoCheckpoint:YES];
}

- (void)test_feature_compress_newly_created_table_after_compressed
{
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self doTestCompress:^{
        TestCaseAssertTrue(self.database.isCompressed);

        self.anotherTable = @"testAnotherTable";
        TestCaseAssertTrue([self.database createTable:self.anotherTable withClass:CompressionTestObject.class]);

        [self.database insertObjects:[self.table getObjects] intoTable:self.anotherTable];

        [self configCompressFilter];

        // migrated status is reset.
        TestCaseAssertFalse(self.database.isCompressed);

        do {
            TestCaseAssertTrue([self.database stepCompression]);
        } while (!self.database.isCompressed);

        WCTValue* uncompressedTextCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_text").count()).from(self.anotherTable).where(WCDB::Column("WCDB_CT_text").isNull())];
        TestCaseAssertTrue(uncompressedTextCount.numberValue.intValue == 0);
        if (!self.compressTwoColumn) {
            return;
        }

        WCTValue* uncompressedBlobCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_blob").count()).from(self.tableName).where(WCDB::Column("WCDB_CT_blob").isNull())];
        TestCaseAssertTrue(uncompressedBlobCount.numberValue.intValue == 0);
    }];
}

- (void)test_feature_compress_new_column_after_compressed
{
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self doTestCompress:^{
        if (self.compressTwoColumn) {
            return;
        }

        TestCaseAssertTrue(self.database.isCompressed);

        self.compressTwoColumn = YES;

        [self configCompressFilter];

        // migrated status is reset.
        TestCaseAssertFalse(self.database.isCompressed);

        do {
            TestCaseAssertTrue([self.database stepCompression]);
        } while (!self.database.isCompressed);

        WCTValue* uncompressedBlobCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_blob").count()).from(self.tableName).where(WCDB::Column("WCDB_CT_blob").isNull())];
        TestCaseAssertTrue(uncompressedBlobCount.numberValue.intValue == 0);
    }];
}

- (void)test_compressed_content_will_persist_after_retrieving
{
    self.compressionStatus = CompressionStatus_finishCompressed;
    [self doTestCompress:^{
        [self.database retrieve:nil];
        WCTOneRow* record = [self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Column::all()).from(WCDB::CompressionRecord::tableName).where(WCDB::Column(WCDB::CompressionRecord::columnTable) == self.tableName)];

        TestCaseAssertTrue(record != nil && record[2].numberValue.intValue == 0);
        if (!self.compressTwoColumn) {
            TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text"]);
        } else {
            TestCaseAssertTrue([record[1].stringValue isEqualToString:@"text blob"]);
        }

        NSArray* objects = [self.table getObjects];
        NSArray* originObjects = [self.uncompressTable getObjects];
        TestCaseAssertTrue([originObjects isEqualTo:objects]);

        WCTValue* uncompressedTextCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_text").count()).from(self.tableName).where(WCDB::Column("WCDB_CT_text").isNull())];
        TestCaseAssertTrue(uncompressedTextCount != nil && uncompressedTextCount.numberValue.intValue == 0);
        if (!self.compressTwoColumn) {
            return;
        }
        WCTValue* uncompressedBlobCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column("WCDB_CT_blob").count()).from(self.tableName).where(WCDB::Column("WCDB_CT_blob").isNull())];
        TestCaseAssertTrue(uncompressedBlobCount != nil && uncompressedBlobCount.numberValue.intValue == 0);
    }];
}

@end
