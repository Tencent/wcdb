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

@interface WTCStatementCreateIndexTests : WTCBaseTestCase

@end

@implementation WTCStatementCreateIndexTests

- (void)testCreateIndex
{
    //Give
    Column column1("column1");
    
    ColumnIndex index1(column1);
    ColumnIndex index2(Column("column2"), OrderTerm::ASC);
    
    std::string indexName1 = "index1";
    std::string table1 = "table1";
    
    //Then
    XCTAssertEqual(StatementCreateIndex().getStatementType(), Statement::Type::CreateIndex);
    
    WINQAssertEqual(StatementCreateIndex().create(indexName1).on(table1, {index1, index2}), @"CREATE INDEX IF NOT EXISTS index1 ON table1(column1, column2 ASC)");
    
    WINQAssertEqual(StatementCreateIndex().create(indexName1, true).on(table1, {index1, index2}), @"CREATE UNIQUE INDEX IF NOT EXISTS index1 ON table1(column1, column2 ASC)");
    
    WINQAssertEqual(StatementCreateIndex().create(indexName1, false, false).on(table1, {index1, index2}), @"CREATE INDEX index1 ON table1(column1, column2 ASC)");
    
    WINQAssertEqual(StatementCreateIndex().create(indexName1).on(table1, {index1, index2}).where(column1 >= 1), @"CREATE INDEX IF NOT EXISTS index1 ON table1(column1, column2 ASC) WHERE (column1 >= 1)");
}

@end
