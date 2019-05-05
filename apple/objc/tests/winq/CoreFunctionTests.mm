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

@interface CoreFunctionTests : BaseTestCase

@end

@implementation CoreFunctionTests {
    WCDB::Expression expression1;
    WCDB::Expression expression2;
    WCDB::Expression expression3;
    WCDB::Expressions expressions;
}

- (void)setUp
{
    [super setUp];
    expression1 = 1;
    expression2 = 2;
    expression3 = 3;
    expressions = { expression1, expression2 };
}

- (void)test
{
    TestCaseAssertSQLEqual(WCDB::CoreFunction::abs(expression1), @"abs(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::changes(), @"changes()");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::char_(expressions), @"char(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::coalesce(expressions), @"coalesce(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::glob(expression1, expression2), @"glob(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::hex(expression1), @"hex(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::ifNull(expression1, expression2), @"ifnull(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::instr(expression1, expression2), @"instr(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::lastInsertRowID(), @"last_insert_rowid()");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::length(expression1), @"length(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::like(expression1, expression2), @"like(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::like(expression1, expression2, expression3), @"like(1, 2, 3)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::likelihood(expression1, expression2), @"likelihood(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::likely(expression1), @"likely(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::lower(expression1), @"lower(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::ltrim(expression1), @"ltrim(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::ltrim(expression1, expression2), @"ltrim(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::max(expressions), @"max(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::min(expressions), @"min(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::nullIf(expression1, expression2), @"nullif(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::printf(expressions), @"printf(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::quote(expression1), @"quote(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::random(), @"random()");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::randomBLOB(expression1), @"randomblob(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::replace(expression1, expression2, expression3), @"replace(1, 2, 3)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::round(expression1), @"round(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::round(expression1, expression2), @"round(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::rtrim(expression1), @"rtrim(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::rtrim(expression1, expression2), @"rtrim(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::soundex(expression1), @"soundex(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::sqliteCompileOptionGet(expression1), @"sqlite_compileoption_get(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::sqliteCompileOptionUsed(expression1), @"sqlite_compileoption_used(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::sqliteOffset(expression1), @"sqlite_offset(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::sqliteSourceID(), @"sqlite_source_id()");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::sqliteVersion(), @"sqlite_version()");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::substr(expression1, expression2), @"substr(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::substr(expression1, expression2, expression3), @"substr(1, 2, 3)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::totalChanges(), @"total_changes()");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::trim(expression1), @"trim(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::trim(expression1, expression2), @"trim(1, 2)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::typeof_(expression1), @"typeof(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::unicode(expression1), @"unicode(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::unlikely(expression1), @"unlikely(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::upper(expression1), @"upper(1)");
    TestCaseAssertSQLEqual(WCDB::CoreFunction::zeroBLOB(expression1), @"zeroblob(1)");
}

- (void)test_column_operate
{
    WCDB::Column column(@"testColumn");
    TestCaseAssertSQLEqual(column.abs(), @"abs(testColumn)");
    TestCaseAssertSQLEqual(column.hex(), @"hex(testColumn)");
    TestCaseAssertSQLEqual(column.length(), @"length(testColumn)");
    TestCaseAssertSQLEqual(column.lower(), @"lower(testColumn)");
    TestCaseAssertSQLEqual(column.ltrim(), @"ltrim(testColumn)");
    TestCaseAssertSQLEqual(column.ltrim(expression1), @"ltrim(testColumn, 1)");
    TestCaseAssertSQLEqual(column.replace(expression1, expression2), @"replace(testColumn, 1, 2)");
    TestCaseAssertSQLEqual(column.round(), @"round(testColumn)");
    TestCaseAssertSQLEqual(column.round(expression1), @"round(testColumn, 1)");
    TestCaseAssertSQLEqual(column.rtrim(), @"rtrim(testColumn)");
    TestCaseAssertSQLEqual(column.rtrim(expression1), @"rtrim(testColumn, 1)");
    TestCaseAssertSQLEqual(column.substr(expression1), @"substr(testColumn, 1)");
    TestCaseAssertSQLEqual(column.substr(expression1, expression2), @"substr(testColumn, 1, 2)");
    TestCaseAssertSQLEqual(column.trim(), @"trim(testColumn)");
    TestCaseAssertSQLEqual(column.trim(expression1), @"trim(testColumn, 1)");
    TestCaseAssertSQLEqual(column.typeof_(), @"typeof(testColumn)");
    TestCaseAssertSQLEqual(column.upper(), @"upper(testColumn)");
}

- (void)test_expression_operate
{
    WCDB::Expression column = WCDB::Column(@"testColumn");
    TestCaseAssertSQLEqual(column.abs(), @"abs(testColumn)");
    TestCaseAssertSQLEqual(column.hex(), @"hex(testColumn)");
    TestCaseAssertSQLEqual(column.length(), @"length(testColumn)");
    TestCaseAssertSQLEqual(column.lower(), @"lower(testColumn)");
    TestCaseAssertSQLEqual(column.ltrim(), @"ltrim(testColumn)");
    TestCaseAssertSQLEqual(column.ltrim(expression1), @"ltrim(testColumn, 1)");
    TestCaseAssertSQLEqual(column.replace(expression1, expression2), @"replace(testColumn, 1, 2)");
    TestCaseAssertSQLEqual(column.round(), @"round(testColumn)");
    TestCaseAssertSQLEqual(column.round(expression1), @"round(testColumn, 1)");
    TestCaseAssertSQLEqual(column.rtrim(), @"rtrim(testColumn)");
    TestCaseAssertSQLEqual(column.rtrim(expression1), @"rtrim(testColumn, 1)");
    TestCaseAssertSQLEqual(column.substr(expression1), @"substr(testColumn, 1)");
    TestCaseAssertSQLEqual(column.substr(expression1, expression2), @"substr(testColumn, 1, 2)");
    TestCaseAssertSQLEqual(column.trim(), @"trim(testColumn)");
    TestCaseAssertSQLEqual(column.trim(expression1), @"trim(testColumn, 1)");
    TestCaseAssertSQLEqual(column.typeof_(), @"typeof(testColumn)");
    TestCaseAssertSQLEqual(column.upper(), @"upper(testColumn)");
}

@end
