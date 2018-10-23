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

@interface CoreFunctionTests : WINQTestCase

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
    WINQAssertEqual(WCDB::CoreFunction::abs(expression1), @"abs(1)");
    WINQAssertEqual(WCDB::CoreFunction::changes(), @"changes()");
    WINQAssertEqual(WCDB::CoreFunction::coalesce(expressions), @"coalesce(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::glob(expression1, expression2), @"glob(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::hex(expression1), @"hex(1)");
    WINQAssertEqual(WCDB::CoreFunction::ifNull(expression1, expression2), @"ifnull(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::instr(expression1, expression2), @"instr(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::lastInsertRowID(), @"last_insert_rowid()");
    WINQAssertEqual(WCDB::CoreFunction::length(expression1), @"length(1)");
    WINQAssertEqual(WCDB::CoreFunction::like(expression1, expression2), @"like(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::like(expression1, expression2, expression3), @"like(1, 2, 3)");
    WINQAssertEqual(WCDB::CoreFunction::likelihood(expression1, expression2), @"likelihood(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::likely(expression1), @"likely(1)");
    WINQAssertEqual(WCDB::CoreFunction::lower(expression1), @"lower(1)");
    WINQAssertEqual(WCDB::CoreFunction::ltrim(expression1), @"ltrim(1)");
    WINQAssertEqual(WCDB::CoreFunction::ltrim(expression1, expression2), @"ltrim(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::max(expressions), @"max(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::min(expressions), @"min(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::nullIf(expression1, expression2), @"nullif(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::printf(expressions), @"printf(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::quote(expression1), @"quote(1)");
    WINQAssertEqual(WCDB::CoreFunction::random(), @"random()");
    WINQAssertEqual(WCDB::CoreFunction::randomBLOB(expression1), @"randomblob(1)");
    WINQAssertEqual(WCDB::CoreFunction::replace(expression1, expression2, expression3), @"replace(1, 2, 3)");
    WINQAssertEqual(WCDB::CoreFunction::round(expression1), @"round(1)");
    WINQAssertEqual(WCDB::CoreFunction::round(expression1, expression2), @"round(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::rtrim(expression1), @"rtrim(1)");
    WINQAssertEqual(WCDB::CoreFunction::rtrim(expression1, expression2), @"rtrim(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::soundex(expression1), @"soundex(1)");
    WINQAssertEqual(WCDB::CoreFunction::sqliteCompileOptionGet(expression1), @"sqlite_compileoption_get(1)");
    WINQAssertEqual(WCDB::CoreFunction::sqliteCompileOptionUsed(expression1), @"sqlite_compileoption_used(1)");
    WINQAssertEqual(WCDB::CoreFunction::sqliteOffset(expression1), @"sqlite_offset(1)");
    WINQAssertEqual(WCDB::CoreFunction::sqliteSourceID(), @"sqlite_source_id()");
    WINQAssertEqual(WCDB::CoreFunction::sqliteVersion(), @"sqlite_version()");
    WINQAssertEqual(WCDB::CoreFunction::substr(expression1, expression2), @"substr(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::substr(expression1, expression2, expression3), @"substr(1, 2, 3)");
    WINQAssertEqual(WCDB::CoreFunction::totalChanges(), @"total_changes()");
    WINQAssertEqual(WCDB::CoreFunction::trim(expression1), @"trim(1)");
    WINQAssertEqual(WCDB::CoreFunction::trim(expression1, expression2), @"trim(1, 2)");
    WINQAssertEqual(WCDB::CoreFunction::typeof_(expression1), @"typeof(1)");
    WINQAssertEqual(WCDB::CoreFunction::unicode(expression1), @"unicode(1)");
    WINQAssertEqual(WCDB::CoreFunction::unlikely(expression1), @"unlikely(1)");
    WINQAssertEqual(WCDB::CoreFunction::upper(expression1), @"upper(1)");
    WINQAssertEqual(WCDB::CoreFunction::zeroBLOB(expression1), @"zeroblob(1)");
}

- (void)test_column_operate
{
    WCDB::Column column(@"testColumn");
    WINQAssertEqual(column.abs(), @"abs(testColumn)");
    WINQAssertEqual(column.hex(), @"hex(testColumn)");
    WINQAssertEqual(column.length(), @"length(testColumn)");
    WINQAssertEqual(column.lower(), @"lower(testColumn)");
    WINQAssertEqual(column.ltrim(), @"ltrim(testColumn)");
    WINQAssertEqual(column.ltrim(expression1), @"ltrim(testColumn, 1)");
    WINQAssertEqual(column.replace(expression1, expression2), @"replace(testColumn, 1, 2)");
    WINQAssertEqual(column.round(), @"round(testColumn)");
    WINQAssertEqual(column.round(expression1), @"round(testColumn, 1)");
    WINQAssertEqual(column.rtrim(), @"rtrim(testColumn)");
    WINQAssertEqual(column.rtrim(expression1), @"rtrim(testColumn, 1)");
    WINQAssertEqual(column.substr(expression1), @"substr(testColumn, 1)");
    WINQAssertEqual(column.substr(expression1, expression2), @"substr(testColumn, 1, 2)");
    WINQAssertEqual(column.trim(), @"trim(testColumn)");
    WINQAssertEqual(column.trim(expression1), @"trim(testColumn, 1)");
    WINQAssertEqual(column.typeof_(), @"typeof(testColumn)");
    WINQAssertEqual(column.upper(), @"upper(testColumn)");
}

- (void)test_expression_operate
{
    WCDB::Expression column = WCDB::Column(@"testColumn");
    WINQAssertEqual(column.abs(), @"abs(testColumn)");
    WINQAssertEqual(column.hex(), @"hex(testColumn)");
    WINQAssertEqual(column.length(), @"length(testColumn)");
    WINQAssertEqual(column.lower(), @"lower(testColumn)");
    WINQAssertEqual(column.ltrim(), @"ltrim(testColumn)");
    WINQAssertEqual(column.ltrim(expression1), @"ltrim(testColumn, 1)");
    WINQAssertEqual(column.replace(expression1, expression2), @"replace(testColumn, 1, 2)");
    WINQAssertEqual(column.round(), @"round(testColumn)");
    WINQAssertEqual(column.round(expression1), @"round(testColumn, 1)");
    WINQAssertEqual(column.rtrim(), @"rtrim(testColumn)");
    WINQAssertEqual(column.rtrim(expression1), @"rtrim(testColumn, 1)");
    WINQAssertEqual(column.substr(expression1), @"substr(testColumn, 1)");
    WINQAssertEqual(column.substr(expression1, expression2), @"substr(testColumn, 1, 2)");
    WINQAssertEqual(column.trim(), @"trim(testColumn)");
    WINQAssertEqual(column.trim(expression1), @"trim(testColumn, 1)");
    WINQAssertEqual(column.typeof_(), @"typeof(testColumn)");
    WINQAssertEqual(column.upper(), @"upper(testColumn)");
}

@end
