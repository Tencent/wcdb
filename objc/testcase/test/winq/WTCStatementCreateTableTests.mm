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
#import "WTCBaseTestCase.h"

using namespace WCDB;

@interface WTCStatementCreateTableTests : WTCBaseTestCase

@end

@implementation WTCStatementCreateTableTests

- (void)testCreateTable
{
    //Give
    Column column1("column1");
    Column column2("column2");
    
    ColumnDef def1(column1, ColumnType::Integer32);
    ColumnDef def2(column2, ColumnType::Text);
    
    TableConstraint constraint1 = TableConstraint("constraint1").makePrimary(ColumnIndex(column1));
    TableConstraint constraint2 = TableConstraint("constraint2").makeUnique(ColumnIndex(column2));
    
    std::string table1 = "table1";
    std::string table2 = "table2";
    
    StatementSelect statementSelect = StatementSelect().select(column1).from(table1);
    
    //Then
    XCTAssertEqual(StatementCreateTable().getStatementType(), Statement::Type::CreateTable);
    
    WINQAssertEqual(StatementCreateTable().create(table1, {def1, def2}), @"CREATE TABLE IF NOT EXISTS table1(column1 INTEGER, column2 TEXT)");
    
    WINQAssertEqual(StatementCreateTable().create(table1, {def1, def2}, false), @"CREATE TABLE table1(column1 INTEGER, column2 TEXT)");
    
    WINQAssertEqual(StatementCreateTable().create(table1, {def1, def2}, {constraint1, constraint2}), @"CREATE TABLE IF NOT EXISTS table1(column1 INTEGER, column2 TEXT, CONSTRAINT constraint1 PRIMARY KEY(column1), CONSTRAINT constraint2 UNIQUE(column2))");
    
    WINQAssertEqual(StatementCreateTable().create(table2).as(statementSelect), @"CREATE TABLE IF NOT EXISTS table2 AS SELECT column1 FROM table1");
    
    WINQAssertEqual(StatementCreateTable().create(table2, false).as(statementSelect), @"CREATE TABLE table2 AS SELECT column1 FROM table1");
}

@end
