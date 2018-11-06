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

@interface ModuleArgumentTests : WINQTestCase

@end

@implementation ModuleArgumentTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::ModuleArgument constructible;
}

- (void)test_module_argument_with_column_def
{
    auto testingSQL = WCDB::ModuleArgument(WCDB::ColumnDef(WCDB::Column(@"testColumn"), WCDB::ColumnType::Integer32));

    auto testingTypes = { WCDB::SQL::Type::ModuleArgument, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testColumn INTEGER");
}

- (void)test_module_argument_with_table_constraint
{
    auto testingSQL = WCDB::ModuleArgument(WCDB::TableConstraint().check(1));

    auto testingTypes = { WCDB::SQL::Type::ModuleArgument, WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CHECK(1)");
}

- (void)test_module_argument_with_left_and_right
{
    auto testingSQL = WCDB::ModuleArgument(@"tokenize", @"WCDB");

    auto testingTypes = { WCDB::SQL::Type::ModuleArgument, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'tokenize' = 'WCDB'");
}

@end
