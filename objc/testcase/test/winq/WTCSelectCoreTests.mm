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

@interface WTCSelectCoreTests : XCTestCase

@end

@implementation WTCSelectCoreTests

- (void)testSelectCore
{
    WCDB::Expression column1 = WCDB::Expression::ColumnNamed("testColumn1");
    WCDB::Expression column2 = WCDB::Expression::ColumnNamed("testColumn2");

    WCDB::ResultColumn resultColumn1 = column1;
    WCDB::ResultColumn resultColumn2 = column2;
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn1, resultColumn2};

    WCDB::TableOrSubquery tableOrSubquery1 = WCDB::TableOrSubquery("testTable1");
    WCDB::TableOrSubquery tableOrSubquery2 = WCDB::TableOrSubquery("testTable2");
    std::list<WCDB::TableOrSubquery> tableOrSubquerys = {tableOrSubquery1, tableOrSubquery2};

    WCDB::JoinClause joinClause = tableOrSubquery1;

    WCDB::Expression condition = column1.notNull();

    std::list<WCDB::Expression> groupList = {column1, column2};

    WCDB::Expression having = column2.notNull();

    WCDB::Expression value1 = WCDB::LiteralValue(1);
    WCDB::Expression value2 = WCDB::LiteralValue("2");
    std::list<WCDB::Expression> values = {value1, value2};

    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(tableOrSubquery1).where(condition).groupBy(column1).having(having), @"SELECT testColumn1 FROM testTable1 WHERE testColumn1 NOTNULL GROUP BY testColumn1 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(tableOrSubquery1).where(condition).groupBy(column1), @"SELECT testColumn1 FROM testTable1 WHERE testColumn1 NOTNULL GROUP BY testColumn1");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(tableOrSubquery1).where(condition).groupBy(groupList).having(having), @"SELECT testColumn1 FROM testTable1 WHERE testColumn1 NOTNULL GROUP BY testColumn1, testColumn2 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(tableOrSubquery1).where(condition), @"SELECT testColumn1 FROM testTable1 WHERE testColumn1 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(tableOrSubquery1).groupBy(column1).having(having), @"SELECT testColumn1 FROM testTable1 GROUP BY testColumn1 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(tableOrSubquerys).where(condition).groupBy(column1).having(having), @"SELECT testColumn1 FROM testTable1, testTable2 WHERE testColumn1 NOTNULL GROUP BY testColumn1 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).from(joinClause).where(condition).groupBy(column1).having(having), @"SELECT testColumn1 FROM testTable1 WHERE testColumn1 NOTNULL GROUP BY testColumn1 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).where(condition).groupBy(column1).having(having), @"SELECT testColumn1 WHERE testColumn1 NOTNULL GROUP BY testColumn1 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumns).from(tableOrSubquery1).where(condition).groupBy(column1).having(having), @"SELECT testColumn1, testColumn2 FROM testTable1 WHERE testColumn1 NOTNULL GROUP BY testColumn1 HAVING testColumn2 NOTNULL");
    WINQAssertEqual(WCDB::SelectCore().select(resultColumn1).distinct().from(tableOrSubquery1).where(condition).groupBy(column1).having(having), @"SELECT DISTINCT testColumn1 FROM testTable1 WHERE testColumn1 NOTNULL GROUP BY testColumn1 HAVING testColumn2 NOTNULL");

    WINQAssertEqual(WCDB::SelectCore().values(value1), @"VALUES(1)");

    WINQAssertEqual(WCDB::SelectCore().values(values), @"VALUES(1, '2')");
}

@end
