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

#import "WTCWINQTestCase.h"

@interface WTCStatementCreateIndexTests : WTCWINQTestCase

@end

@implementation WTCStatementCreateIndexTests

- (void)testStatementCreateIndex
{
    XCTAssertEqual(WCDB::StatementCreateIndex().getType(), WCDB::Statement::Type::CreateIndex);

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .ifNotExists(false)
                        .withSchema(self.class.schemaName)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumn),
                    @"CREATE INDEX testSchema.testIndex ON testTable(testColumn)");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .ifNotExists(false)
                        .withSchema(self.class.schemaName)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumn)
                        .where(self.class.condition),
                    @"CREATE INDEX testSchema.testIndex ON testTable(testColumn) WHERE testColumn NOTNULL");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .ifNotExists(false)
                        .withSchema(self.class.schemaName)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumns),
                    @"CREATE INDEX testSchema.testIndex ON testTable(testColumn, testColumn2)");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .ifNotExists(false)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumn),
                    @"CREATE INDEX testIndex ON testTable(testColumn)");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .ifNotExists(true)
                        .withSchema(self.class.schemaName)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumn),
                    @"CREATE INDEX IF NOT EXISTS testSchema.testIndex ON testTable(testColumn)");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .ifNotExists(false)
                        .unique()
                        .withSchema(self.class.schemaName)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumn),
                    @"CREATE UNIQUE INDEX testSchema.testIndex ON testTable(testColumn)");

    //Default
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.tableName)
                        .indexedBy(self.class.indexedColumn),
                    @"CREATE INDEX IF NOT EXISTS testSchema.testIndex ON testTable(testColumn)");
}

@end
