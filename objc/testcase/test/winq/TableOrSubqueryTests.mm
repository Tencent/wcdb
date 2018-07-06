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

#import "WINQTestCase.h"

@interface TableOrSubqueryTests : WINQTestCase

@end

@implementation TableOrSubqueryTests

- (void)testTableOrSubquery
{
    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName), @"main.testTable");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                    .indexedBy(self.class.indexName),
                    @"main.testTable INDEXED BY testIndex");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                    .notIndexed(),
                    @"main.testTable NOT INDEXED");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                    .as("testTableAlias"),
                    @"main.testTable AS testTableAlias");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableName)
                    .withSchema(self.class.schemaName),
                    @"testSchema.testTable");
}

- (void)testTableOrSubqueryWithTableFunction
{
    WINQAssertEqual(WCDB::TableOrSubquery::function(self.class.functionName, self.class.value), @"main.testFunction(1)");

    WINQAssertEqual(WCDB::TableOrSubquery::function(self.class.functionName, self.class.value)
                    .as("testTableAlias"),
                    @"main.testFunction(1) AS testTableAlias");

    WINQAssertEqual(WCDB::TableOrSubquery::function(self.class.functionName, self.class.values), @"main.testFunction(1, 'testValue')");

    WINQAssertEqual(WCDB::TableOrSubquery::function(self.class.functionName), @"main.testFunction()");

    WINQAssertEqual(WCDB::TableOrSubquery::function(self.class.functionName, self.class.value)
                    .withSchema(self.class.schemaName),
                    @"testSchema.testFunction(1)");
}

- (void)testTableOrSubqueryWithListOrJoinClause
{
    WINQAssertEqual(WCDB::TableOrSubquery(self.class.tableOrSubquerys), @"(main.testTable, main.testTable2)");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.joinClause), @"(main.testTable)");
}

- (void)testTableOrSubqueryWithStatementSelect
{
    WINQAssertEqual(WCDB::TableOrSubquery(self.class.statementSelect), @"(SELECT testColumn FROM main.testTable)");

    WINQAssertEqual(WCDB::TableOrSubquery(self.class.statementSelect)
                    .as("testTableAlias"),
                    @"(SELECT testColumn FROM main.testTable) AS testTableAlias");
}

@end
