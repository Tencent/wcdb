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

#import "WINQAssertion.h"

@interface ColumnTypeTests : BaseTestCase

@end

@implementation ColumnTypeTests {
    WCDB::Column column;
}

- (void)setUp
{
    [super setUp];
    column = WCDB::Column(@"testColumn");
}

- (void)test_default_constructible
{
    WCDB::ColumnType constructible;
    WCDB_UNUSED(constructible);
}

- (void)test_type_integer32
{
    auto testingSQL = WCDB::ColumnDef(column, WCDB::ColumnType::Integer32);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn INTEGER");
}

- (void)test_type_integer64
{
    auto testingSQL = WCDB::ColumnDef(column, WCDB::ColumnType::Integer64);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn INTEGER");
}

- (void)test_type_text
{
    auto testingSQL = WCDB::ColumnDef(column, WCDB::ColumnType::Text);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn TEXT");
}

- (void)test_type_blob
{
    auto testingSQL = WCDB::ColumnDef(column, WCDB::ColumnType::BLOB);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn BLOB");
}

- (void)test_type_float
{
    auto testingSQL = WCDB::ColumnDef(column, WCDB::ColumnType::Float);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn REAL");
}

- (void)test_type_null
{
    auto testingSQL = WCDB::ColumnDef(column, WCDB::ColumnType::Null);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NULL");
}

@end
