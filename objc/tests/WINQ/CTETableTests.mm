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
#import <WCDB/WCDB.h>

@interface CTETableTests : WINQTestCase

@end

@implementation CTETableTests {
    NSString* table;
    WCDB::Columns columns;
}

- (void)setUp
{
    [super setUp];
    table = @"testTable";
    columns = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
}

- (void)test_default_constructible
{
    WCDB::CTETable constructible __attribute((unused));
}

- (void)test_cte_table
{
    auto testingSQL = WCDB::CTETable(table).columns(columns);

    auto testingTypes = { WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable(testColumn1, testColumn2)");
}

- (void)test_cte_table_without_columns
{
    auto testingSQL = WCDB::CTETable(table);

    auto testingTypes = { WCDB::SQL::Type::CTETableName };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable");
}

@end
