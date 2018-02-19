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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCStatementCreateIndexTests : XCTestCase

@end

@implementation WTCStatementCreateIndexTests

- (void)testStatementCreateIndex
{
    std::string schemaName = "testSchema";
    std::string indexName = "testIndex";
    std::string tableName = "testTable";
    WCDB::IndexedColumn indexedColumn1 = "testColumn1";
    std::list<WCDB::IndexedColumn> indexedColumns = {indexedColumn1, "testColumn2"};
    WCDB::Expression expr = WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn1")).notNull();

    WINQAssertEqual(WCDB::StatementCreateIndex().createIndex(indexName, false, false).withSchema(schemaName).on(tableName, indexedColumn1), @"CREATE INDEX testSchema.testIndex ON testTable(testColumn1)");

    WINQAssertEqual(WCDB::StatementCreateIndex().createIndex(indexName, false, false).withSchema(schemaName).on(tableName, indexedColumn1).where(expr), @"CREATE INDEX testSchema.testIndex ON testTable(testColumn1) WHERE testColumn1 NOTNULL");

    WINQAssertEqual(WCDB::StatementCreateIndex().createIndex(indexName, false, false).withSchema(schemaName).on(tableName, indexedColumns), @"CREATE INDEX testSchema.testIndex ON testTable(testColumn1, testColumn2)");

    WINQAssertEqual(WCDB::StatementCreateIndex().createIndex(indexName, false, false).on(tableName, indexedColumns), @"CREATE INDEX testIndex ON testTable(testColumn1, testColumn2)");

    WINQAssertEqual(WCDB::StatementCreateIndex().createIndex(indexName, true, false).withSchema(schemaName).on(tableName, indexedColumn1), @"CREATE INDEX IF NOT EXISTS testSchema.testIndex ON testTable(testColumn1)");

    WINQAssertEqual(WCDB::StatementCreateIndex().createIndex(indexName, false, true).withSchema(schemaName).on(tableName, indexedColumn1), @"CREATE UNIQUE INDEX testSchema.testIndex ON testTable(testColumn1)");
}

@end
