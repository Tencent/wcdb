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

@interface LiteralValueTests : WINQTestCase

@end

@implementation LiteralValueTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::LiteralValue constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::LiteralValue().getType(), WCDB::SQL::Type::LiteralValue);
    XCTAssertEqual(WCDB::LiteralValue::type, WCDB::SQL::Type::LiteralValue);
}

- (void)test_uint
{
    auto testingSQL = WCDB::LiteralValue(1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_int
{
    auto testingSQL = WCDB::LiteralValue(-1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"-1");
}

- (void)test_ns_number
{
    auto testingSQL = WCDB::LiteralValue(@(1));
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_true
{
    auto testingSQL = WCDB::LiteralValue(true);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_false
{
    auto testingSQL = WCDB::LiteralValue(false);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0");
}

- (void)test_yes
{
    auto testingSQL = WCDB::LiteralValue(YES);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_no
{
    auto testingSQL = WCDB::LiteralValue(NO);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0");
}

- (void)test_int32_max
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int32_t>::max());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"2147483647");
}

- (void)test_int32_min
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int32_t>::min());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"-2147483648");
}

- (void)test_int64_max
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int64_t>::max());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"9223372036854775807");
}

- (void)test_int64_min
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int64_t>::min());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"-9223372036854775808");
}

- (void)test_float
{
    auto testingSQL = WCDB::LiteralValue((float) 0.1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0.1");
}

- (void)test_double
{
    auto testingSQL = WCDB::LiteralValue((float) 0.1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0.1");
}

- (void)test_string
{
    auto testingSQL = WCDB::LiteralValue("test");
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'test'");
}

- (void)test_cpp_string
{
    auto testingSQL = WCDB::LiteralValue(std::string("test"));
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'test'");
}

- (void)test_ns_string
{
    auto testingSQL = WCDB::LiteralValue(@"test");
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'test'");
}

- (void)test_null
{
    auto testingSQL = WCDB::LiteralValue(nullptr);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"NULL");
}

- (void)test_nil
{
    auto testingSQL = WCDB::LiteralValue(nil);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"NULL");
}

- (void)test_current_time
{
    auto testingSQL = WCDB::LiteralValue::currentTime();
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CURRENT_TIME");
}

- (void)test_current_date
{
    auto testingSQL = WCDB::LiteralValue::currentDate();
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CURRENT_DATE");
}

- (void)test_current_timestamp
{
    auto testingSQL = WCDB::LiteralValue::currentTimestamp();
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CURRENT_TIMESTAMP");
}

- (void)test_null_convertible
{
    std::nullptr_t sql = nullptr;
    NSString* expected = @"NULL";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_nulls_convertible
{
    std::list<std::nullptr_t> sqls = { nullptr, nullptr };
    NSString* expected = @"NULL, NULL";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_int32_convertible
{
    int32_t sql = 1;
    NSString* expected = @"1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_int32s_convertible
{
    std::list<int32_t> sqls = { 1, 2 };
    NSString* expected = @"1, 2";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_int64_convertible
{
    int64_t sql = 1;
    NSString* expected = @"1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_int64s_convertible
{
    std::list<int32_t> sqls = { 1, 2 };
    NSString* expected = @"1, 2";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_float_convertible
{
    float sql = 1.1;
    NSString* expected = @"1.1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_floats_convertible
{
    std::list<float> sqls = { 1.1, 2.2 };
    NSString* expected = @"1.1, 2.2";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_double_convertible
{
    double sql = 1.1;
    NSString* expected = @"1.1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_doubles_convertible
{
    std::list<double> sqls = { 1.1, 2.2 };
    NSString* expected = @"1.1, 2.2";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_bool_convertible
{
    bool sql = true;
    NSString* expected = @"1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_bools_convertible
{
    std::list<bool> sqls = { true, false };
    NSString* expected = @"1, 0";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_ns_number_convertible
{
    NSNumber* sql = @(1);
    NSString* expected = @"1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_ns_numbers_convertible
{
    std::list<NSNumber*> sqls = { @(1),
                                  @(2) };
    NSString* expected = @"1, 2";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_const_char_p_convertible
{
    const char* sql = "t";

    NSString* expected = @"'t'";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);

    WINQConvertibleTest(WCDB::QualifiedTable, sql, @"main.t");
    WINQConvertibleTest(WCDB::CTETable, sql, @"t");
    WINQConvertibleTest(WCDB::TableOrSubquery, sql, @"main.t");
    WINQConvertibleTest(WCDB::TablesOrSubqueries, sql, @"main.t");
}

- (void)test_const_char_ps_convertible
{
    std::list<const char*> sqls = {
        "t1",
        "t2",
    };

    NSString* expected = @"'t1', 't2'";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);

    WINQConvertibleTest(WCDB::TablesOrSubqueries, sqls, @"main.t1, main.t2");
}

- (void)test_std_string_convertible
{
    std::string sql = "t";

    NSString* expected = @"'t'";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);

    WINQConvertibleTest(WCDB::QualifiedTable, sql, @"main.t");
    WINQConvertibleTest(WCDB::CTETable, sql, @"t");
    WINQConvertibleTest(WCDB::TableOrSubquery, sql, @"main.t");
    WINQConvertibleTest(WCDB::TablesOrSubqueries, sql, @"main.t");
}

- (void)test_std_strings_convertible
{
    std::list<std::string> sqls = {
        "t1",
        "t2",
    };

    NSString* expected = @"'t1', 't2'";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);

    WINQConvertibleTest(WCDB::TablesOrSubqueries, sqls, @"main.t1, main.t2");
}

- (void)test_ns_string_convertible
{
    NSString* sql = @"t";

    NSString* expected = @"'t'";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);

    WINQConvertibleTest(WCDB::QualifiedTable, sql, @"main.t");
    WINQConvertibleTest(WCDB::CTETable, sql, @"t");
    WINQConvertibleTest(WCDB::TableOrSubquery, sql, @"main.t");
    WINQConvertibleTest(WCDB::TablesOrSubqueries, sql, @"main.t");
}

- (void)test_ns_strings_convertible
{
    std::list<NSString*> sqls = {
        @"t1",
        @"t2",
    };

    NSString* expected = @"'t1', 't2'";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);

    WINQConvertibleTest(WCDB::TablesOrSubqueries, sqls, @"main.t1, main.t2");
}

- (void)test_literal_value_convertible
{
    WCDB::LiteralValue sql = 1;
    NSString* expected = @"1";
    WINQConvertibleTest(WCDB::LiteralValue, sql, expected);
    WINQConvertibleTest(WCDB::LiteralValues, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_literal_values_convertible
{
    WCDB::LiteralValues sqls = {
        (bool) true,
        (int32_t) 1,
        (std::nullptr_t) nullptr,
        (int64_t) 2,
        (NSNumber*) @(3),
        (float) 1.1,
        (double) 2.2,
        (const char*) "t1",
        (std::string) "t2",
        (NSString*) @"t3",
    };
    NSString* expected = @"1, 1, NULL, 2, 3, 1.1, 2.2, 't1', 't2', 't3'";
    WINQConvertibleTest(WCDB::LiteralValues, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_anti_injection
{
    NSString* injection = @";'DROP TABLE testInjection;--";
    WINQAssertEqual(WCDB::StatementSelect().select(injection).where(1), @"SELECT ';''DROP TABLE testInjection;--' WHERE 1");
}

@end
