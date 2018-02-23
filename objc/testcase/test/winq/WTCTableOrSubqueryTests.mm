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

@interface WTCTableOrSubqueryTests : WTCWINQTestCase

@end

@implementation WTCTableOrSubqueryTests

- (void)testTableOrSubquery
{
    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName), @"testTable");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                        .indexedBy(self.class.indexName),
                    @"testTable INDEXED BY testIndex");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                        .notIndexed(),
                    @"testTable NOT INDEXED");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                        .as("testTableAlias"),
                    @"testTable AS testTableAlias");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                        .withSchema(self.class.schemaName),
                    @"testSchema.testTable");
}

- (void)testTableOrSubqueryWithTableFunction
{
    WINQAssertEqual(WCDB::TableOrSubquery::Function(self.class.functionName, self.class.value), @"testFunction(1)");

    WINQAssertEqual(WCDB::TableOrSubquery::Function(self.class.functionName, self.class.value)
                        .as("testTableAlias"),
                    @"testFunction(1) AS testTableAlias");

    WINQAssertEqual(WCDB::TableOrSubquery::Function(self.class.functionName, self.class.values), @"testFunction(1, 'testValue')");

    WINQAssertEqual(WCDB::TableOrSubquery::Function(self.class.functionName), @"testFunction()");

    WINQAssertEqual(WCDB::TableOrSubquery::Function(self.class.functionName, self.class.value)
                        .withSchema(self.class.schemaName),
                    @"testSchema.testFunction(1)");
}

- (void)testTableOrSubqueryWithListOrJoinClause
{
    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableOrSubquerys), @"(testTable, testTable2)");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.joinClause), @"(testTable)");
}

- (void)testTableOrSubqueryWithStatementSelect
{
    WINQAssertEqual(WCDB::TableOrSubquery(self.class.statementSelect), @"(SELECT testColumn FROM testTable)");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.statementSelect)
                        .as("testTableAlias"),
                    @"(SELECT testColumn FROM testTable) AS testTableAlias");
}

@end
