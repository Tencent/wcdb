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

@interface WTCStatementSelectTests : WTCBaseTestCase

@end

@implementation WTCStatementSelectTests

- (void)testStatementSelect
{
    //Give
    WCDB::Column column1("column1");
    WCDB::Column column2("column2");

    std::string table1 = "table1";
    std::string table2 = "table2";

    XCTAssertEqual(WCDB::StatementSelect().getStatementType(), WCDB::Statement::Type::Select);

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1), @"SELECT column1 FROM table1");

    WINQAssertEqual(WCDB::StatementSelect().select({column1, column2}).from(table1), @"SELECT column1, column2 FROM table1");

    WINQAssertEqual(WCDB::StatementSelect().select({column1, column2}, true).from(table1), @"SELECT DISTINCT column1, column2 FROM table1");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from({table1, table2}), @"SELECT column1 FROM table1, table2");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).where(column1 > 1), @"SELECT column1 FROM table1 WHERE (column1 > 1)");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).orderBy({column1, column2}), @"SELECT column1 FROM table1 ORDER BY column1, column2");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).limit(1), @"SELECT column1 FROM table1 LIMIT 1");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).limit(1, 2), @"SELECT column1 FROM table1 LIMIT 1, 2");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).limit(1).offset(3), @"SELECT column1 FROM table1 LIMIT 1 OFFSET 3");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).groupBy(column1), @"SELECT column1 FROM table1 GROUP BY column1");

    WINQAssertEqual(WCDB::StatementSelect().select(column1).from(table1).groupBy({column1, column2}).having(column1 > 1), @"SELECT column1 FROM table1 GROUP BY column1, column2 HAVING (column1 > 1)");
}

@end
