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

@interface CTETableTests : WINQTestCase

@end

@implementation CTETableTests {
    NSString* table;
    WCDB::Column column1;
    WCDB::Column column2;
}

- (void)setUp
{
    [super setUp];
    table = @"testTable";
    column1 = WCDB::Column(@"testColumn1");
    column2 = WCDB::Column(@"testColumn2");
}

- (void)test_default_constructible
{
    WCDB::CTETable constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::CTETable().getType(), WCDB::SQL::Type::CTETableName);
    XCTAssertEqual(WCDB::CTETable::type, WCDB::SQL::Type::CTETableName);
}

- (void)test_cte_table
{
    auto testingSQL = WCDB::CTETable(table).column(column1);

    auto testingTypes = { WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable(testColumn1)");
}

- (void)test_cte_table_with_columns
{
    auto testingSQL = WCDB::CTETable(table).column(column1).column(column2);

    auto testingTypes = { WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable(testColumn1, testColumn2)");
}

- (void)test_cte_table_without_column
{
    auto testingSQL = WCDB::CTETable(table);

    auto testingTypes = { WCDB::SQL::Type::CTETableName };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable");
}

@end
