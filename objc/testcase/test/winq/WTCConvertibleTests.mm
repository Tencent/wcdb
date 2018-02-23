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

@interface WTCConvertibleTests : WTCWINQTestCase

@end

@implementation WTCConvertibleTests

- (void)testRaw
{
    int numericLiteral = 1;
    float floatLiteral = 2.1;
    std::string stringLiteral = "3";
    std::vector<unsigned char> blobLiteral;
    blobLiteral.push_back('4');

    //To Expression
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(numericLiteral),
                    @"SELECT testColumn FROM testTable WHERE 1");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(floatLiteral),
                    @"SELECT testColumn FROM testTable WHERE 2.1");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(stringLiteral),
                    @"SELECT testColumn FROM testTable WHERE '3'");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(blobLiteral),
                    @"SELECT testColumn FROM testTable WHERE '4'");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where("3"),
                    @"SELECT testColumn FROM testTable WHERE '3'");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(nullptr),
                    @"SELECT testColumn FROM testTable WHERE NULL");

    //To IndexedColumn
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, numericLiteral),
                    @"CREATE INDEX testIndex ON testTable(1)");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, floatLiteral),
                    @"CREATE INDEX testIndex ON testTable(2.1)");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, stringLiteral),
                    @"CREATE INDEX testIndex ON testTable('3')");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, blobLiteral),
                    @"CREATE INDEX testIndex ON testTable('4')");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, "3"),
                    @"CREATE INDEX testIndex ON testTable('3')");

    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, nullptr),
                    @"CREATE INDEX testIndex ON testTable(NULL)");

    //To ResultColumn
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(numericLiteral)
                        .from(self.class.tableOrSubquery),
                    @"SELECT 1 FROM testTable");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(floatLiteral)
                        .from(self.class.tableOrSubquery),
                    @"SELECT 2.1 FROM testTable");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(stringLiteral)
                        .from(self.class.tableOrSubquery),
                    @"SELECT '3' FROM testTable");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(blobLiteral)
                        .from(self.class.tableOrSubquery),
                    @"SELECT '4' FROM testTable");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select("3")
                        .from(self.class.tableOrSubquery),
                    @"SELECT '3' FROM testTable");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(nullptr)
                        .from(self.class.tableOrSubquery),
                    @"SELECT NULL FROM testTable");

    //To OrderingTerm
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(numericLiteral),
                    @"SELECT testColumn FROM testTable ORDER BY 1");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(floatLiteral),
                    @"SELECT testColumn FROM testTable ORDER BY 2.1");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(stringLiteral),
                    @"SELECT testColumn FROM testTable ORDER BY '3'");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(blobLiteral),
                    @"SELECT testColumn FROM testTable ORDER BY '4'");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy("3"),
                    @"SELECT testColumn FROM testTable ORDER BY '3'");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(nullptr),
                    @"SELECT testColumn FROM testTable ORDER BY NULL");
}

- (void)testLiteralValue
{
    //To IndexedColumn
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, self.class.literalValue),
                    @"CREATE INDEX testIndex ON testTable(1)");

    //To ResultColumn
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.literalValue)
                        .from(self.class.tableOrSubquery),
                    @"SELECT 1 FROM testTable");

    //To OrderingTerm
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(self.class.literalValue),
                    @"SELECT testColumn FROM testTable ORDER BY 1");
}

- (void)testColumn
{
    //To IndexedColumn
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, self.class.column),
                    @"CREATE INDEX testIndex ON testTable(testColumn)");

    //To ResultColumn
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.column)
                        .from(self.class.tableOrSubquery),
                    @"SELECT testColumn FROM testTable");

    //To OrderingTerm
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(self.class.column),
                    @"SELECT testColumn FROM testTable ORDER BY testColumn");
}

- (void)testBindParameter
{
    WCDB::BindParameter bindParameter;

    //To IndexedColumn
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, bindParameter),
                    @"CREATE INDEX testIndex ON testTable(?)");

    //To ResultColumn
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(bindParameter)
                        .from(self.class.tableOrSubquery),
                    @"SELECT ? FROM testTable");

    //To OrderingTerm
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(bindParameter),
                    @"SELECT testColumn FROM testTable ORDER BY ?");
}

- (void)testStatementSelect
{
    //To IndexedColumn
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, self.class.statementSelect),
                    @"CREATE INDEX testIndex ON testTable((SELECT testColumn FROM testTable))");

    //To ResultColumn
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.statementSelect)
                        .from(self.class.tableOrSubquery),
                    @"SELECT (SELECT testColumn FROM testTable) FROM testTable");

    //To OrderingTerm
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(self.class.statementSelect),
                    @"SELECT testColumn FROM testTable ORDER BY (SELECT testColumn FROM testTable)");
}

- (void)testRaiseFunction
{
    WCDB::RaiseFunction raiseFunction = WCDB::RaiseFunction().withIgnore();

    //To IndexedColumn
    WINQAssertEqual(WCDB::StatementCreateIndex()
                        .createIndex(self.class.indexName, false, false)
                        .on(self.class.tableName, raiseFunction),
                    @"CREATE INDEX testIndex ON testTable(RAISE(IGNORE))");

    //To ResultColumn
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(raiseFunction)
                        .from(self.class.tableOrSubquery),
                    @"SELECT RAISE(IGNORE) FROM testTable");

    //To OrderingTerm
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .orderBy(raiseFunction),
                    @"SELECT testColumn FROM testTable ORDER BY RAISE(IGNORE)");
}

@end
