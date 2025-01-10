//
// Created by qiuwenchen on 2023/12/3.
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

@interface CompressionTestCase () {
    WCTTable *m_uncompressedTable;
    WCTDatabase *m_sourceDatabase;
    NSData *m_cipher;
    NSArray<CompressionTestObject *> *m_objects;
    NSString *m_sourceSchema;
}
@end

@implementation CompressionTestCase

- (void)setUp
{
    [super setUp];
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        for (int i = 1; i < 7; i++) {
            NSArray<NSString *> *contents = [[Random shared] compressionStringWithCount:3000];
            NSData *dict = [WCTDatabase trainDictWithStrings:contents andDictId:(WCTDictId) i];
            XCTAssertNotNil(dict);
            TestCaseAssertTrue([WCTDatabase registerZSTDDict:dict andDictId:(WCTDictId) i]);
        }
    });
    m_sourceDatabase = [[WCTDatabase alloc] initWithPath:[self.path stringByAppendingString:@"_source"]];
    m_objects = [Random.shared autoIncrementCompressionObjectWithCount:1000];
    self.tableClass = CompressionTestObject.class;
}

- (NSString *)sourceSchema
{
    @synchronized(self) {
        if (m_sourceSchema == nil) {
            m_sourceSchema = [NSString stringWithFormat:@"wcdb_migration_%u.", WCDB::UnsafeStringView(m_sourceDatabase.path).hash()];
        }
        return m_sourceSchema;
    }
}

- (NSString *)uncompressTableName
{
    return m_uncompressedTable.tableName;
}

- (WCTTable *)uncompressTable
{
    return m_uncompressedTable;
}

- (NSDictionary<NSNumber *, NSNumber *> *)matchDictForText
{
    return @{
        @0 : @1,
        @1 : @2,
        @2 : @3,
        @3 : @4,
        @4 : @5,
        @WCTDictDefaultMatchValue : @6,
    };
}

- (NSDictionary<NSNumber *, NSNumber *> *)matchDictForBLOB
{
    return @{
        @0 : @1,
        @1 : @2,
        @2 : @3,
        @WCTDictDefaultMatchValue : @4,
    };
}

- (NSArray<CompressionTestObject *> *)originObjects
{
    return m_objects;
}

- (void)configCompression
{
    [self clearData];
    NSData *cipher = nil;
    if (_needCipher) {
        if (!m_cipher) {
            m_cipher = [Random.shared data];
        }
    }
    [self.database setCipherKey:cipher];

    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.database createTable:@"uncompressedTable" withClass:CompressionTestObject.class]);
    m_uncompressedTable = [self.database getTable:@"uncompressedTable" withClass:CompressionTestObject.class];
    TestCaseAssertTrue([m_uncompressedTable insertObjects:m_objects]);

    if (_migrationStatus != MigrationStatus_noMigration) {
        TestCaseAssertTrue([m_sourceDatabase createTable:self.tableName withClass:CompressionTestObject.class]);
        TestCaseAssertTrue([m_sourceDatabase insertObjects:m_objects intoTable:self.tableName]);
        [self.database addMigration:m_sourceDatabase.path
                         withFilter:^(WCTMigrationUserInfo *info) {
                             if ([info.table isEqualToString:self.tableName]) {
                                 [info setSourceTable:self.tableName];
                             }
                         }];
    } else {
        [self.table insertObjects:m_objects];
    }

    [self configCompressFilter];
    [self.database setNotificationWhenCompressed:nil];

    if (self.migrationStatus == MigrationStatus_startMigrate) {
        TestCaseAssertTrue([self.database stepMigration]);
        TestCaseAssertFalse([self.database isMigrated]);
    }
    if (self.compressionStatus == CompressionStatus_startCompressed) {
        TestCaseAssertTrue([self.database stepCompression]);
    } else if (self.compressionStatus == CompressionStatus_finishCompressed) {
        while (![self.database isCompressed]) {
            TestCaseAssertTrue([self.database stepCompression]);
        }
    }
}

- (void)configCompressFilter
{
    switch (self.mode) {
    case CompressionMode_Normal: {
        [self.database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName] && ![info.table isEqualToString:self.anotherTable]) {
                return;
            }
            [info addZSTDNormalCompressProperty:CompressionTestObject.text];
            if (self.compressTwoColumn) {
                [info addZSTDNormalCompressProperty:CompressionTestObject.blob];
            }
        }];
    } break;
    case CompressionMode_Dict: {
        [self.database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName] && ![info.table isEqualToString:self.anotherTable]) {
                return;
            }
            [info addZSTDDictCompressProperty:CompressionTestObject.text withDictId:1];
            if (self.compressTwoColumn) {
                [info addZSTDDictCompressProperty:CompressionTestObject.blob withDictId:1];
            }
        }];
    } break;
    case CompressionMode_VariousDict: {
        [self.database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName] && ![info.table isEqualToString:self.anotherTable]) {
                return;
            }
            [info addZSTDDictCompressProperty:CompressionTestObject.text
                            withMatchProperty:CompressionTestObject.textMatchId
                                andMatchDicts:[self matchDictForText]];
            if (self.compressTwoColumn) {
                [info addZSTDDictCompressProperty:CompressionTestObject.blob
                                withMatchProperty:CompressionTestObject.blobMatchId
                                    andMatchDicts:[self matchDictForBLOB]];
            }
        }];
    } break;
    }
}

- (void)reconfigCompressionFilter
{
    switch (self.mode) {
    case CompressionMode_Normal: {
        [self.database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName] && ![info.table isEqualToString:self.anotherTable]) {
                return;
            }
            [info addZSTDDictCompressProperty:CompressionTestObject.text withDictId:1];
            if (self.compressTwoColumn) {
                [info addZSTDDictCompressProperty:CompressionTestObject.blob withDictId:1];
            }
            [info enableReplaceCompression];
        }];
    } break;
    case CompressionMode_Dict: {
        [self.database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName] && ![info.table isEqualToString:self.anotherTable]) {
                return;
            }
            [info addZSTDDictCompressProperty:CompressionTestObject.text
                            withMatchProperty:CompressionTestObject.textMatchId
                                andMatchDicts:[self matchDictForText]];
            if (self.compressTwoColumn) {
                [info addZSTDDictCompressProperty:CompressionTestObject.blob
                                withMatchProperty:CompressionTestObject.blobMatchId
                                    andMatchDicts:[self matchDictForBLOB]];
            }
            [info enableReplaceCompression];
        }];
    } break;
    case CompressionMode_VariousDict: {
        [self.database setCompressionWithFilter:^(WCTCompressionUserInfo *info) {
            if (![info.table isEqualToString:self.tableName] && ![info.table isEqualToString:self.anotherTable]) {
                return;
            }
            [info addZSTDNormalCompressProperty:CompressionTestObject.text];
            if (self.compressTwoColumn) {
                [info addZSTDNormalCompressProperty:CompressionTestObject.blob];
            }
            [info enableReplaceCompression];
        }];
    } break;
    }
}

- (void)clearData
{
    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertTrue([m_sourceDatabase removeFiles]);
    [self.database addMigration:m_sourceDatabase.path withFilter:nil];
    [self.database setCompressionWithFilter:nil];
}

- (void)doTestCompression:(void (^)(void))operation
{
    for (int i = 0; i < 108; i++) {
        int config = i;
        [self clearData];

        self.mode = (CompressionMode) (config % 3);
        config /= 3;
        self.compressTwoColumn = (BOOL) (config % 2);
        config /= 2;
        self.compressionStatus = (CompressionStatus) (config % 3);
        config /= 3;
        self.migrationStatus = (MigrationStatus) (config % 3);
        config /= 3;
        self.needCipher = (BOOL) (config % 2);

        [self log:@"execute test %d: mode %d, two column %d, compression status %d, migration status %d, need cipher %d",
                  i,
                  self.mode,
                  self.compressTwoColumn,
                  self.compressionStatus,
                  self.migrationStatus,
                  self.needCipher];

        [self configCompression];
        operation();
    }
}

- (void)executeModify:(BOOL (^)(WCTTable *))modify andSQLs:(NSArray<NSString *> *)sqls
{
    TestCaseAssertTrue(modify(m_uncompressedTable));

    NSArray<CompressionTestObject *> *expectingObjs = [m_uncompressedTable getObjects];
    TestCaseAssertTrue(expectingObjs.count > 0);
    [self doTestObjects:expectingObjs
                andSQLs:sqls
      afterModification:^BOOL {
          return modify(self.table);
      }];
    if (self.migrationStatus != MigrationStatus_noMigration) {
        return;
    }
    if (expectingObjs.count < m_objects.count) {
        return;
    }
    if (expectingObjs.count == m_objects.count) {
        bool equal = true;
        for (int i = 0; i < m_objects.count; i++) {
            if (expectingObjs[i].text != m_objects[i].text && ![expectingObjs[i].text isEqualToString:m_objects[i].text]) {
                equal = NO;
                break;
            }
        }
        if (equal) {
            return;
        }
    }

    // Check new data is compressed
    WCTValue *compressedTextCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column::all().count()).from(self.tableName).where(WCDB::Column("WCDB_CT_text").notNull())];
    TestCaseAssertTrue(compressedTextCount.numberValue.intValue > 0);
    if (self.compressionStatus == CompressionStatus_finishCompressed) {
        TestCaseAssertTrue(compressedTextCount.numberValue.intValue == expectingObjs.count);
    }
    if (!self.compressTwoColumn) {
        return;
    }
    if (expectingObjs.count == m_objects.count) {
        bool equal = true;
        for (int i = 0; i < m_objects.count; i++) {
            if (expectingObjs[i].blob != m_objects[i].blob && ![expectingObjs[i].blob isEqualToData:m_objects[i].blob]) {
                equal = NO;
                break;
            }
        }
        if (equal) {
            return;
        }
    }
    WCTValue *compressedBlobCount = [self.database getValueFromStatement:WCDB::StatementSelect().select(WCDB::Column::all().count()).from(self.tableName).where(WCDB::Column("WCDB_CT_blob").notNull())];
    TestCaseAssertTrue(compressedBlobCount.numberValue.intValue > 0);
    if (self.compressionStatus == CompressionStatus_finishCompressed) {
        TestCaseAssertTrue(compressedBlobCount.numberValue.intValue == expectingObjs.count);
    }
}

- (void)executeSelect:(NSArray * (^)(WCTTable *) )select andSQLs:(NSArray<NSString *> *)sqls
{
    NSArray *expectingObjs = select(m_uncompressedTable);
    TestCaseAssertTrue(expectingObjs.count > 0);
    [self doTestObjects:expectingObjs
                andSQLs:sqls
            bySelecting:^NSArray<NSObject<WCTTableCoding> *> * {
                return select(self.table);
            }];
}

@end
